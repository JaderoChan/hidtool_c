// C language bindings for the hidtool C++ library.
// This file is compiled as C++ but exports a pure C interface.

#include <cstring>
#include <atomic>
#include <vector>

#include <hidtool/hid_hooker.hpp>
#include <hidtool/hid_simulator.hpp>
#include <hidtool/hidtype.hpp>

#ifdef HIDTOOL_HAS_KEYBOARD
    #include <hidtool/keyboard/keyboard_hooker.hpp>
    #include <hidtool/keyboard/keyboard_key.hpp>
    #include <hidtool/keyboard/keyboard_simulator.hpp>
#endif

#ifdef HIDTOOL_HAS_MOUSE
    #include <hidtool/mouse/mouse_hooker.hpp>
    #include <hidtool/mouse/mouse_simulator.hpp>
#endif

// Include the C header inside extern "C" to get function declarations.
extern "C"
{
#include <hidtool_c/hidtool_c.h>
}

// ---------------------------------------------------------------------------
// Internal type-conversion helpers
// ---------------------------------------------------------------------------

#ifdef HIDTOOL_HAS_KEYBOARD

static HidtKeyboardEvent convert_keyboard_event(const hidt::KeyboardEvent& src) noexcept
{
    HidtKeyboardEvent dst;
    std::memset(&dst, 0, sizeof(dst));
    dst.type = static_cast<HidtKeyboardEventType>(src.type);
    dst.timestamp = src.timestamp;
    if (src.type == hidt::KeyboardEvent::ET_SLEEP)
        dst.u.sleep_ms = src.sleepMs;
    else
        dst.u.native_key = src.nativeKey;
    return dst;
}

static hidt::KeyboardEvent convert_to_cpp_keyboard_event(const HidtKeyboardEvent& src) noexcept
{
    hidt::KeyboardEvent dst(static_cast<hidt::KeyboardEvent::EventType>(src.type));
    dst.timestamp = src.timestamp;
    if (src.type == HIDT_KBD_ET_SLEEP)
        dst.sleepMs = src.u.sleep_ms;
    else
        dst.nativeKey = src.u.native_key;
    return dst;
}

#endif // HIDTOOL_HAS_KEYBOARD

#ifdef HIDTOOL_HAS_MOUSE

static HidtMouseEvent convert_mouse_event(const hidt::MouseEvent& src) noexcept
{
    HidtMouseEvent dst;
    std::memset(&dst, 0, sizeof(dst));
    dst.type = static_cast<HidtMouseEventType>(src.type);
    dst.timestamp = src.timestamp;
    switch (src.type)
    {
        case hidt::MouseEvent::ET_ABS_MOVE:
            dst.u.abs_pos.x = src.absPos.x;
            dst.u.abs_pos.y = src.absPos.y;
            break;
        case hidt::MouseEvent::ET_REL_MOVE:
            dst.u.rel_pos.dx = src.relPos.dx;
            dst.u.rel_pos.dy = src.relPos.dy;
            break;
        case hidt::MouseEvent::ET_WHEEL:
            dst.u.wheel_delta = src.wheelDelta;
            break;
        case hidt::MouseEvent::ET_DRAG:
            dst.u.drag.pos.x = src.drag.pos.x;
            dst.u.drag.pos.y = src.drag.pos.y;
            dst.u.drag.button = static_cast<HidtMouseButton>(src.drag.button);
            break;
        case hidt::MouseEvent::ET_PRESS:
        case hidt::MouseEvent::ET_RELEASE:
            dst.u.button = static_cast<HidtMouseButton>(src.button);
            break;
        case hidt::MouseEvent::ET_SLEEP:
            dst.u.sleep_ms = src.sleepMs;
            break;
        default:
            break;
    }
    return dst;
}

static hidt::MouseEvent convert_to_cpp_mouse_event(const HidtMouseEvent& src) noexcept
{
    hidt::MouseEvent dst(static_cast<hidt::MouseEvent::EventType>(src.type));
    dst.timestamp = src.timestamp;
    switch (src.type)
    {
        case HIDT_MS_ET_ABS_MOVE:
            dst.absPos = hidt::AbsolutePos(src.u.abs_pos.x, src.u.abs_pos.y);
            break;
        case HIDT_MS_ET_REL_MOVE:
            dst.relPos = hidt::RelativePos(src.u.rel_pos.dx, src.u.rel_pos.dy);
            break;
        case HIDT_MS_ET_WHEEL:
            dst.wheelDelta = src.u.wheel_delta;
            break;
        case HIDT_MS_ET_DRAG:
            dst.drag.pos = hidt::AbsolutePos(src.u.drag.pos.x, src.u.drag.pos.y);
            dst.drag.button = static_cast<hidt::MouseButton>(src.u.drag.button);
            break;
        case HIDT_MS_ET_PRESS:
        case HIDT_MS_ET_RELEASE:
            dst.button = static_cast<hidt::MouseButton>(src.u.button);
            break;
        case HIDT_MS_ET_SLEEP:
            dst.sleepMs = src.u.sleep_ms;
            break;
        default:
            break;
    }
    return dst;
}

#endif // HIDTOOL_HAS_MOUSE

// ---------------------------------------------------------------------------
// Callback wrapper state (one set per hooker type)
// Using intptr_t to atomically store function pointers and user-data pointers.
// ---------------------------------------------------------------------------

#ifdef HIDTOOL_HAS_KEYBOARD

static std::atomic<intptr_t> g_kbd_c_handler{0};
static std::atomic<intptr_t> g_kbd_c_user_data{0};

static bool kbd_hooker_wrapper(const hidt::KeyboardEvent& event, void* /*user_data*/)
{
    HidtKeyboardEventHandler fn =
        reinterpret_cast<HidtKeyboardEventHandler>(g_kbd_c_handler.load());
    if (fn == nullptr)
        return true;
    HidtKeyboardEvent c_event = convert_keyboard_event(event);
    void* ud = reinterpret_cast<void*>(g_kbd_c_user_data.load());
    return fn(&c_event, ud) != 0;
}

#endif // HIDTOOL_HAS_KEYBOARD

#ifdef HIDTOOL_HAS_MOUSE

static std::atomic<intptr_t> g_ms_c_handler{0};
static std::atomic<intptr_t> g_ms_c_user_data{0};

static bool ms_hooker_wrapper(const hidt::MouseEvent& event, void* /*user_data*/)
{
    HidtMouseEventHandler fn =
        reinterpret_cast<HidtMouseEventHandler>(g_ms_c_handler.load());
    if (fn == nullptr)
        return true;
    HidtMouseEvent c_event = convert_mouse_event(event);
    void* ud = reinterpret_cast<void*>(g_ms_c_user_data.load());
    return fn(&c_event, ud) != 0;
}

#endif // HIDTOOL_HAS_MOUSE

static std::atomic<intptr_t> g_hid_c_handler{0};
static std::atomic<intptr_t> g_hid_c_user_data{0};

static bool hid_hooker_wrapper(const hidt::HidEvent& event, void* /*user_data*/)
{
    HidtHidEventHandler fn =
        reinterpret_cast<HidtHidEventHandler>(g_hid_c_handler.load());
    if (fn == nullptr)
        return true;

    HidtHidEvent c_event;
    std::memset(&c_event, 0, sizeof(c_event));
    c_event.type = static_cast<HidtHidEventType>(event.type);

    switch (event.type)
    {
    #ifdef HIDTOOL_HAS_KEYBOARD
        case hidt::HidEvent::ET_KEYBOARD:
            c_event.u.keyboard_event = convert_keyboard_event(event.keyboardEvent);
            break;
    #endif
    #ifdef HIDTOOL_HAS_MOUSE
        case hidt::HidEvent::ET_MOUSE:
            c_event.u.mouse_event = convert_mouse_event(event.mouseEvent);
            break;
    #endif
        case hidt::HidEvent::ET_SLEEP:
            c_event.u.sleep_ms = event.sleepMs;
            break;
        default:
            break;
    }

    void* ud = reinterpret_cast<void*>(g_hid_c_user_data.load());
    return fn(&c_event, ud) != 0;
}

// ===========================================================================
// C API implementation
// ===========================================================================

extern "C"
{

// ---------------------------------------------------------------------------
// HID Type
// ---------------------------------------------------------------------------

HIDTC_API int hidt_is_hid_type_supported(HidtHidType hid_type)
{
    return hidt::isHidTypeSupported(static_cast<hidt::HidType>(hid_type)) ? 1 : 0;
}

// ---------------------------------------------------------------------------
// Keyboard Key Utilities
// ---------------------------------------------------------------------------

#ifdef HIDTOOL_HAS_KEYBOARD

HIDTC_API int32_t hidt_keyboard_key_to_native_key(HidtKeyboardKey key)
{
    return hidt::keyboardKeyToNativeKey(static_cast<hidt::KeyboardKey>(key));
}

HIDTC_API HidtKeyboardKey hidt_keyboard_key_from_native_key(int32_t native_key)
{
    return static_cast<HidtKeyboardKey>(hidt::keyboardKeyFromNativeKey(native_key));
}

// ---------------------------------------------------------------------------
// Keyboard Hooker
// ---------------------------------------------------------------------------

HIDTC_API int hidt_keyboard_hooker_is_key_pressed(int32_t native_key)
{
    return hidt::KeyboardHooker::isKeyPressed(native_key) ? 1 : 0;
}

HIDTC_API int hidt_keyboard_hooker_run(void)
{
    return hidt::KeyboardHooker::getInstance().run() ? 1 : 0;
}

HIDTC_API void hidt_keyboard_hooker_stop(void)
{
    hidt::KeyboardHooker::getInstance().stop();
}

HIDTC_API int hidt_keyboard_hooker_is_running(void)
{
    return hidt::KeyboardHooker::getInstance().isRunning() ? 1 : 0;
}

HIDTC_API int hidt_keyboard_hooker_set_event_handler(
    HidtKeyboardEventHandler handler, void* user_data)
{
    g_kbd_c_handler.store(reinterpret_cast<intptr_t>(handler));
    g_kbd_c_user_data.store(reinterpret_cast<intptr_t>(user_data));

    hidt::KeyboardHooker& hooker = hidt::KeyboardHooker::getInstance();
    if (handler == nullptr)
        return hooker.setEventHandler(nullptr) ? 1 : 0;
    return hooker.setEventHandler(kbd_hooker_wrapper) ? 1 : 0;
}

HIDTC_API int hidt_keyboard_hooker_set_user_data(void* user_data)
{
    g_kbd_c_user_data.store(reinterpret_cast<intptr_t>(user_data));
    return hidt::KeyboardHooker::getInstance().setUserData(nullptr) ? 1 : 0;
}

// ---------------------------------------------------------------------------
// Keyboard Simulator
// ---------------------------------------------------------------------------

HIDTC_API int hidt_keyboard_simulator_initialize(void)
{
    return hidt::KeyboardSimulator::getInstance().initialize() ? 1 : 0;
}

HIDTC_API void hidt_keyboard_simulator_destroy(void)
{
    hidt::KeyboardSimulator::getInstance().destroy();
}

HIDTC_API int hidt_keyboard_simulator_is_initialized(void)
{
    return hidt::KeyboardSimulator::getInstance().isInitialized() ? 1 : 0;
}

HIDTC_API int hidt_keyboard_simulator_send_event(const HidtKeyboardEvent* event)
{
    if (event == nullptr)
        return 0;
    hidt::KeyboardEvent cpp_event = convert_to_cpp_keyboard_event(*event);
    return hidt::KeyboardSimulator::getInstance().sendEvent(cpp_event) ? 1 : 0;
}

HIDTC_API size_t hidt_keyboard_simulator_send_events(
    const HidtKeyboardEvent* events, size_t count)
{
    if (events == nullptr || count == 0)
        return 0;
    std::vector<hidt::KeyboardEvent> cpp_events(count);
    for (size_t i = 0; i < count; ++i)
        cpp_events[i] = convert_to_cpp_keyboard_event(events[i]);
    return hidt::KeyboardSimulator::getInstance().sendEvent(cpp_events.data(), count);
}

HIDTC_API int hidt_keyboard_simulator_press_key_native(int32_t native_key)
{
    return hidt::KeyboardSimulator::getInstance().pressKey(native_key) ? 1 : 0;
}

HIDTC_API int hidt_keyboard_simulator_press_key(HidtKeyboardKey key)
{
    return hidt::KeyboardSimulator::getInstance()
        .pressKey(static_cast<hidt::KeyboardKey>(key)) ? 1 : 0;
}

HIDTC_API int hidt_keyboard_simulator_release_key_native(int32_t native_key)
{
    return hidt::KeyboardSimulator::getInstance().releaseKey(native_key) ? 1 : 0;
}

HIDTC_API int hidt_keyboard_simulator_release_key(HidtKeyboardKey key)
{
    return hidt::KeyboardSimulator::getInstance()
        .releaseKey(static_cast<hidt::KeyboardKey>(key)) ? 1 : 0;
}

HIDTC_API int hidt_keyboard_simulator_click_key_native(int32_t native_key, uint64_t interval)
{
    return hidt::KeyboardSimulator::getInstance().clickKey(native_key, interval) ? 1 : 0;
}

HIDTC_API int hidt_keyboard_simulator_click_key(HidtKeyboardKey key, uint64_t interval)
{
    return hidt::KeyboardSimulator::getInstance()
        .clickKey(static_cast<hidt::KeyboardKey>(key), interval) ? 1 : 0;
}

#endif // HIDTOOL_HAS_KEYBOARD

// ---------------------------------------------------------------------------
// Mouse Hooker
// ---------------------------------------------------------------------------

#ifdef HIDTOOL_HAS_MOUSE

HIDTC_API int hidt_mouse_hooker_is_button_pressed(HidtMouseButton button)
{
    return hidt::MouseHooker::isButtonPressed(static_cast<hidt::MouseButton>(button)) ? 1 : 0;
}

HIDTC_API HidtAbsolutePos hidt_mouse_hooker_get_cursor_pos(void)
{
    hidt::AbsolutePos pos = hidt::MouseHooker::getCursorPos();
    HidtAbsolutePos result;
    result.x = pos.x;
    result.y = pos.y;
    return result;
}

HIDTC_API int hidt_mouse_hooker_run(void)
{
    return hidt::MouseHooker::getInstance().run() ? 1 : 0;
}

HIDTC_API void hidt_mouse_hooker_stop(void)
{
    hidt::MouseHooker::getInstance().stop();
}

HIDTC_API int hidt_mouse_hooker_is_running(void)
{
    return hidt::MouseHooker::getInstance().isRunning() ? 1 : 0;
}

HIDTC_API int hidt_mouse_hooker_set_event_handler(
    HidtMouseEventHandler handler, void* user_data)
{
    g_ms_c_handler.store(reinterpret_cast<intptr_t>(handler));
    g_ms_c_user_data.store(reinterpret_cast<intptr_t>(user_data));

    hidt::MouseHooker& hooker = hidt::MouseHooker::getInstance();
    if (handler == nullptr)
        return hooker.setEventHandler(nullptr) ? 1 : 0;
    return hooker.setEventHandler(ms_hooker_wrapper) ? 1 : 0;
}

HIDTC_API int hidt_mouse_hooker_set_user_data(void* user_data)
{
    g_ms_c_user_data.store(reinterpret_cast<intptr_t>(user_data));
    return hidt::MouseHooker::getInstance().setUserData(nullptr) ? 1 : 0;
}

// ---------------------------------------------------------------------------
// Mouse Simulator
// ---------------------------------------------------------------------------

HIDTC_API HidtAbsolutePosRange hidt_mouse_simulator_get_absolute_move_range(void)
{
    hidt::AbsolutePosRange range = hidt::MouseSimulator::getAbsoluteMoveRange();
    HidtAbsolutePosRange result;
    result.min_x = range.minX;
    result.max_x = range.maxX;
    result.min_y = range.minY;
    result.max_y = range.maxY;
    return result;
}

HIDTC_API int hidt_mouse_simulator_initialize(void)
{
    return hidt::MouseSimulator::getInstance().initialize() ? 1 : 0;
}

HIDTC_API void hidt_mouse_simulator_destroy(void)
{
    hidt::MouseSimulator::getInstance().destroy();
}

HIDTC_API int hidt_mouse_simulator_is_initialized(void)
{
    return hidt::MouseSimulator::getInstance().isInitialized() ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_send_event(const HidtMouseEvent* event)
{
    if (event == nullptr)
        return 0;
    hidt::MouseEvent cpp_event = convert_to_cpp_mouse_event(*event);
    return hidt::MouseSimulator::getInstance().sendEvent(cpp_event) ? 1 : 0;
}

HIDTC_API size_t hidt_mouse_simulator_send_events(const HidtMouseEvent* events, size_t count)
{
    if (events == nullptr || count == 0)
        return 0;
    std::vector<hidt::MouseEvent> cpp_events(count);
    for (size_t i = 0; i < count; ++i)
        cpp_events[i] = convert_to_cpp_mouse_event(events[i]);
    return hidt::MouseSimulator::getInstance().sendEvent(cpp_events.data(), count);
}

HIDTC_API int hidt_mouse_simulator_move_to(int32_t x, int32_t y)
{
    return hidt::MouseSimulator::getInstance().moveTo(hidt::AbsolutePos(x, y)) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_move_by(int32_t dx, int32_t dy)
{
    return hidt::MouseSimulator::getInstance().moveBy(hidt::RelativePos(dx, dy)) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_wheel(int32_t wheel_delta)
{
    return hidt::MouseSimulator::getInstance().wheel(wheel_delta) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_press_button(HidtMouseButton button)
{
    return hidt::MouseSimulator::getInstance()
        .pressButton(static_cast<hidt::MouseButton>(button)) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_release_button(HidtMouseButton button)
{
    return hidt::MouseSimulator::getInstance()
        .releaseButton(static_cast<hidt::MouseButton>(button)) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_click_button(HidtMouseButton button, uint64_t interval)
{
    return hidt::MouseSimulator::getInstance()
        .clickButton(static_cast<hidt::MouseButton>(button), interval) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_double_click_button(
    HidtMouseButton button, uint64_t interval1, uint64_t interval2)
{
    return hidt::MouseSimulator::getInstance()
        .doubleClickButton(static_cast<hidt::MouseButton>(button), interval1, interval2) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_wheel_at(
    int32_t x, int32_t y, int32_t wheel_delta, uint64_t interval)
{
    return hidt::MouseSimulator::getInstance()
        .wheel(hidt::AbsolutePos(x, y), wheel_delta, interval) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_press_button_at(
    int32_t x, int32_t y, HidtMouseButton button, uint64_t interval)
{
    return hidt::MouseSimulator::getInstance()
        .pressButton(
            hidt::AbsolutePos(x, y),
            static_cast<hidt::MouseButton>(button), interval) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_release_button_at(
    int32_t x, int32_t y, HidtMouseButton button, uint64_t interval)
{
    return hidt::MouseSimulator::getInstance()
        .releaseButton(
            hidt::AbsolutePos(x, y),
            static_cast<hidt::MouseButton>(button), interval) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_click_button_at(
    int32_t x, int32_t y, HidtMouseButton button, uint64_t interval1, uint64_t interval2)
{
    return hidt::MouseSimulator::getInstance()
        .clickButton(
            hidt::AbsolutePos(x, y),
            static_cast<hidt::MouseButton>(button),
            interval1, interval2) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_double_click_button_at(
    int32_t x, int32_t y, HidtMouseButton button,
    uint64_t interval1, uint64_t interval2, uint64_t interval3)
{
    return hidt::MouseSimulator::getInstance()
        .doubleClickButton(
            hidt::AbsolutePos(x, y),
            static_cast<hidt::MouseButton>(button),
            interval1, interval2, interval3) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_drag_to(int32_t x, int32_t y, HidtMouseButton button)
{
    return hidt::MouseSimulator::getInstance()
        .dragTo(
            hidt::AbsolutePos(x, y),
            static_cast<hidt::MouseButton>(button)) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_drag_combo(
    int32_t end_x, int32_t end_y, HidtMouseButton button, uint64_t interval)
{
    return hidt::MouseSimulator::getInstance()
        .dragCombo(
            hidt::AbsolutePos(end_x, end_y),
            static_cast<hidt::MouseButton>(button), interval) ? 1 : 0;
}

HIDTC_API int hidt_mouse_simulator_drag_combo_from(
    int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, HidtMouseButton button,
    uint64_t interval1, uint64_t interval2)
{
    return hidt::MouseSimulator::getInstance()
        .dragCombo(
            hidt::AbsolutePos(start_y, start_y),
            hidt::AbsolutePos(end_x, end_y),
            static_cast<hidt::MouseButton>(button),
            interval1, interval2) ? 1 : 0;
}

#endif // HIDTOOL_HAS_MOUSE

// ---------------------------------------------------------------------------
// HID Hooker
// ---------------------------------------------------------------------------

HIDTC_API int hidt_hid_hooker_is_support_block_event_propagation(void)
{
    return hidt::HidHooker::isSupportBlockEventPropagation() ? 1 : 0;
}

#ifdef HIDTOOL_HAS_KEYBOARD
HIDTC_API int hidt_hid_hooker_is_key_pressed(int32_t native_key)
{
    return hidt::HidHooker::isKeyPressed(native_key) ? 1 : 0;
}
#endif

#ifdef HIDTOOL_HAS_MOUSE
HIDTC_API int hidt_hid_hooker_is_button_pressed(HidtMouseButton button)
{
    return hidt::HidHooker::isButtonPressed(static_cast<hidt::MouseButton>(button)) ? 1 : 0;
}

HIDTC_API HidtAbsolutePos hidt_hid_hooker_get_cursor_pos(void)
{
    hidt::AbsolutePos pos = hidt::HidHooker::getCursorPos();
    HidtAbsolutePos result;
    result.x = pos.x;
    result.y = pos.y;
    return result;
}
#endif

HIDTC_API int hidt_hid_hooker_run(void)
{
    return hidt::HidHooker::getInstance().run() ? 1 : 0;
}

HIDTC_API void hidt_hid_hooker_stop(void)
{
    hidt::HidHooker::getInstance().stop();
}

HIDTC_API int hidt_hid_hooker_is_running(void)
{
    return hidt::HidHooker::getInstance().isRunning() ? 1 : 0;
}

HIDTC_API int hidt_hid_hooker_set_event_handler(HidtHidEventHandler handler, void* user_data)
{
    g_hid_c_handler.store(reinterpret_cast<intptr_t>(handler));
    g_hid_c_user_data.store(reinterpret_cast<intptr_t>(user_data));

    hidt::HidHooker& hooker = hidt::HidHooker::getInstance();
    if (handler == nullptr)
        return hooker.setEventHandler(nullptr) ? 1 : 0;
    return hooker.setEventHandler(hid_hooker_wrapper) ? 1 : 0;
}

HIDTC_API int hidt_hid_hooker_set_user_data(void* user_data)
{
    g_hid_c_user_data.store(reinterpret_cast<intptr_t>(user_data));
    return hidt::HidHooker::getInstance().setUserData(nullptr) ? 1 : 0;
}

// ---------------------------------------------------------------------------
// HID Simulator
// ---------------------------------------------------------------------------

HIDTC_API int hidt_hid_simulator_initialize(void)
{
    return hidt::HidSimulator::getInstance().initialize() ? 1 : 0;
}

HIDTC_API void hidt_hid_simulator_destroy(void)
{
    hidt::HidSimulator::getInstance().destroy();
}

HIDTC_API int hidt_hid_simulator_is_initialized(void)
{
    return hidt::HidSimulator::getInstance().isInitialized() ? 1 : 0;
}

HIDTC_API int hidt_hid_simulator_send_event(const HidtHidEvent* event)
{
    if (event == nullptr)
        return 0;

    hidt::HidEvent cpp_event;
    cpp_event.type = static_cast<hidt::HidEvent::EventType>(event->type);

    switch (event->type)
    {
    #ifdef HIDTOOL_HAS_KEYBOARD
        case HIDT_HID_ET_KEYBOARD:
            cpp_event.keyboardEvent = convert_to_cpp_keyboard_event(event->u.keyboard_event);
            break;
    #endif
    #ifdef HIDTOOL_HAS_MOUSE
        case HIDT_HID_ET_MOUSE:
            cpp_event.mouseEvent = convert_to_cpp_mouse_event(event->u.mouse_event);
            break;
    #endif
        case HIDT_HID_ET_SLEEP:
            cpp_event.sleepMs = event->u.sleep_ms;
            break;
        default:
            break;
    }

    return hidt::HidSimulator::getInstance().sendEvent(cpp_event) ? 1 : 0;
}

HIDTC_API size_t hidt_hid_simulator_send_events(const HidtHidEvent* events, size_t count)
{
    if (events == nullptr || count == 0)
        return 0;

    std::vector<hidt::HidEvent> cpp_events(count);
    for (size_t i = 0; i < count; ++i) {
        const HidtHidEvent& src = events[i];
        hidt::HidEvent& dst      = cpp_events[i];
        dst.type = static_cast<hidt::HidEvent::EventType>(src.type);
        switch (src.type)
        {
        #ifdef HIDTOOL_HAS_KEYBOARD
            case HIDT_HID_ET_KEYBOARD:
                dst.keyboardEvent = convert_to_cpp_keyboard_event(src.u.keyboard_event);
                break;
        #endif
        #ifdef HIDTOOL_HAS_MOUSE
            case HIDT_HID_ET_MOUSE:
                dst.mouseEvent = convert_to_cpp_mouse_event(src.u.mouse_event);
                break;
        #endif
            case HIDT_HID_ET_SLEEP:
                dst.sleepMs = src.u.sleep_ms;
                break;
            default:
                break;
        }
    }

    return hidt::HidSimulator::getInstance().sendEvent(cpp_events.data(), count);
}

} // extern "C"
