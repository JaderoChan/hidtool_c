/**
 * @file hidtool_c.h
 * @brief C language bindings for the hidtool C++ library.
 *
 * Provides a pure C API that wraps the hidtool C++ library's keyboard and mouse
 * hooking and simulation functionality.
 */

#ifndef HIDTOOL_C_H
#define HIDTOOL_C_H

#include <stddef.h>
#include <stdint.h>

#include <hidtool_c/hidtool_c_config.h>

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32
    #ifdef HIDTOOL_C_BUILD_SHARED
        #define HIDTC_API __declspec(dllexport)
    #elif defined(HIDTOOL_C_SHARED)
        #define HIDTC_API __declspec(dllimport)
    #else
        #define HIDTC_API
    #endif
#else
    #define HIDTC_API __attribute__((visibility("default")))
#endif

/** @brief HID device types (mirrors hidt::HidType) */
typedef enum HidtHidType
{
    HIDT_HIDTYPE_KEYBOARD   = 0,
    HIDT_HIDTYPE_MOUSE      = 1
} HidtHidType;

/** @brief Mouse button values (mirrors hidt::MouseButton) */
typedef enum HidtMouseButton
{
    HIDT_MSBTN_NONE         = 0,
    HIDT_MSBTN_LEFT         = 1,
    HIDT_MSBTN_RIGHT        = 2,
    HIDT_MSBTN_MIDDLE       = 3,
    HIDT_MSBTN_BACK         = 4,
    HIDT_MSBTN_FORWARD      = 5
} HidtMouseButton;

/** @brief Absolute screen coordinates */
typedef struct HidtAbsolutePos
{
    int32_t x;
    int32_t y;
} HidtAbsolutePos;

/** @brief Relative movement delta */
typedef struct HidtRelativePos
{
    int32_t dx;
    int32_t dy;
} HidtRelativePos;

/** @brief Coordinate range for absolute mouse movement */
typedef struct HidtAbsolutePosRange
{
    int32_t min_x;
    int32_t max_x;
    int32_t min_y;
    int32_t max_y;
} HidtAbsolutePosRange;

/* ==================== Keyboard Event ==================== */

/**
 * @brief Cross-platform keyboard key values (mirrors hidt::KeyboardKey).
 * All values are within the range of a 32-bit signed integer.
 */
typedef enum HidtKeyboardKey
{
    HIDT_KBDKEY_NONE             = 0x0000,

    /* Numeric keys */
    HIDT_KBDKEY_0                = 0x0030,
    HIDT_KBDKEY_1                = 0x0031,
    HIDT_KBDKEY_2                = 0x0032,
    HIDT_KBDKEY_3                = 0x0033,
    HIDT_KBDKEY_4                = 0x0034,
    HIDT_KBDKEY_5                = 0x0035,
    HIDT_KBDKEY_6                = 0x0036,
    HIDT_KBDKEY_7                = 0x0037,
    HIDT_KBDKEY_8                = 0x0038,
    HIDT_KBDKEY_9                = 0x0039,

    /* Letter keys */
    HIDT_KBDKEY_A                = 0x0041,
    HIDT_KBDKEY_B                = 0x0042,
    HIDT_KBDKEY_C                = 0x0043,
    HIDT_KBDKEY_D                = 0x0044,
    HIDT_KBDKEY_E                = 0x0045,
    HIDT_KBDKEY_F                = 0x0046,
    HIDT_KBDKEY_G                = 0x0047,
    HIDT_KBDKEY_H                = 0x0048,
    HIDT_KBDKEY_I                = 0x0049,
    HIDT_KBDKEY_J                = 0x004a,
    HIDT_KBDKEY_K                = 0x004b,
    HIDT_KBDKEY_L                = 0x004c,
    HIDT_KBDKEY_M                = 0x004d,
    HIDT_KBDKEY_N                = 0x004e,
    HIDT_KBDKEY_O                = 0x004f,
    HIDT_KBDKEY_P                = 0x0050,
    HIDT_KBDKEY_Q                = 0x0051,
    HIDT_KBDKEY_R                = 0x0052,
    HIDT_KBDKEY_S                = 0x0053,
    HIDT_KBDKEY_T                = 0x0054,
    HIDT_KBDKEY_U                = 0x0055,
    HIDT_KBDKEY_V                = 0x0056,
    HIDT_KBDKEY_W                = 0x0057,
    HIDT_KBDKEY_X                = 0x0058,
    HIDT_KBDKEY_Y                = 0x0059,
    HIDT_KBDKEY_Z                = 0x005a,

    /* Whitespace keys */
    HIDT_KBDKEY_TAB              = 0x8000,
    HIDT_KBDKEY_SPACE            = 0x8001,
    HIDT_KBDKEY_ENTER            = 0x8002,

    /* Navigation keys */
    HIDT_KBDKEY_HOME             = 0x8003,
    HIDT_KBDKEY_END              = 0x8004,
    HIDT_KBDKEY_PAGE_UP          = 0x8005,
    HIDT_KBDKEY_PAGE_DOWN        = 0x8006,
    HIDT_KBDKEY_LEFT             = 0x8007,
    HIDT_KBDKEY_UP               = 0x8008,
    HIDT_KBDKEY_RIGHT            = 0x8009,
    HIDT_KBDKEY_DOWN             = 0x800a,

    /* Editing keys */
    HIDT_KBDKEY_BACKSPACE        = 0x800b,
    HIDT_KBDKEY_INSERT           = 0x800c,
    HIDT_KBDKEY_DELETE           = 0x800d,
    HIDT_KBDKEY_CLEAR            = 0x800e,

    /* Function keys */
    HIDT_KBDKEY_F1               = 0x800f,
    HIDT_KBDKEY_F2               = 0x8010,
    HIDT_KBDKEY_F3               = 0x8011,
    HIDT_KBDKEY_F4               = 0x8012,
    HIDT_KBDKEY_F5               = 0x8013,
    HIDT_KBDKEY_F6               = 0x8014,
    HIDT_KBDKEY_F7               = 0x8015,
    HIDT_KBDKEY_F8               = 0x8016,
    HIDT_KBDKEY_F9               = 0x8017,
    HIDT_KBDKEY_F10              = 0x8018,
    HIDT_KBDKEY_F11              = 0x8019,
    HIDT_KBDKEY_F12              = 0x801a,
    HIDT_KBDKEY_F13              = 0x801b,
    HIDT_KBDKEY_F14              = 0x801c,
    HIDT_KBDKEY_F15              = 0x801d,
    HIDT_KBDKEY_F16              = 0x801e,
    HIDT_KBDKEY_F17              = 0x801f,
    HIDT_KBDKEY_F18              = 0x8020,
    HIDT_KBDKEY_F19              = 0x8021,
    HIDT_KBDKEY_F20              = 0x8022,
    HIDT_KBDKEY_F21              = 0x8023,
    HIDT_KBDKEY_F22              = 0x8024,
    HIDT_KBDKEY_F23              = 0x8025,
    HIDT_KBDKEY_F24              = 0x8026,

    /* Numpad numeric keys */
    HIDT_KBDKEY_NUMPAD_0         = 0x8027,
    HIDT_KBDKEY_NUMPAD_1         = 0x8028,
    HIDT_KBDKEY_NUMPAD_2         = 0x8029,
    HIDT_KBDKEY_NUMPAD_3         = 0x802a,
    HIDT_KBDKEY_NUMPAD_4         = 0x802b,
    HIDT_KBDKEY_NUMPAD_5         = 0x802c,
    HIDT_KBDKEY_NUMPAD_6         = 0x802d,
    HIDT_KBDKEY_NUMPAD_7         = 0x802e,
    HIDT_KBDKEY_NUMPAD_8         = 0x802f,
    HIDT_KBDKEY_NUMPAD_9         = 0x8030,

    /* Numpad operator keys */
    HIDT_KBDKEY_NUMPAD_ADD       = 0x8031,
    HIDT_KBDKEY_NUMPAD_SUBTRACT  = 0x8032,
    HIDT_KBDKEY_NUMPAD_MULTIPLY  = 0x8033,
    HIDT_KBDKEY_NUMPAD_DIVIDE    = 0x8034,
    HIDT_KBDKEY_NUMPAD_DECIMAL   = 0x8035,
    HIDT_KBDKEY_NUMPAD_SEPARATOR = 0x8036,
    HIDT_KBDKEY_NUMPAD_EQUAL     = 0x8037,
    HIDT_KBDKEY_NUMPAD_ENTER     = 0x8038,

    /* Application keys */
    HIDT_KBDKEY_ESCAPE           = 0x8039,
    HIDT_KBDKEY_PAUSE            = 0x803a,
    HIDT_KBDKEY_PLAY             = 0x803b,
    HIDT_KBDKEY_HELP             = 0x803c,
    HIDT_KBDKEY_MENU             = 0x803d,

    /* Multimedia keys */
    HIDT_KBDKEY_MEDIA_NEXT       = 0x803e,
    HIDT_KBDKEY_MEDIA_PREVIOUS   = 0x803f,
    HIDT_KBDKEY_MEDIA_PLAY_PAUSE = 0x8040,
    HIDT_KBDKEY_MEDIA_STOP       = 0x8041,

    /* Volume keys */
    HIDT_KBDKEY_VOLUME_UP        = 0x8042,
    HIDT_KBDKEY_VOLUME_DOWN      = 0x8043,
    HIDT_KBDKEY_VOLUME_MUTE      = 0x8044,

    /* Lock keys */
    HIDT_KBDKEY_CAPS_LOCK        = 0x8045,
    HIDT_KBDKEY_NUM_LOCK         = 0x8046,
    HIDT_KBDKEY_SCROLL_LOCK      = 0x8047,

    /* Device keys */
    HIDT_KBDKEY_PRINT_SCREEN     = 0x8048,
    HIDT_KBDKEY_SLEEP            = 0x8049,

    /* OEM keys (US standard keyboard layout) */
    HIDT_KBDKEY_LEFT_QUOTE       = 0x804a,  /* '`~' */
    HIDT_KBDKEY_MINUS            = 0x804b,  /* '-_' */
    HIDT_KBDKEY_EQUAL            = 0x804c,  /* '=+' */
    HIDT_KBDKEY_LEFT_BRACKET     = 0x804d,  /* '[{' */
    HIDT_KBDKEY_RIGHT_BRACKET    = 0x804e,  /* ']}' */
    HIDT_KBDKEY_SEMICOLON        = 0x804f,  /* ';:' */
    HIDT_KBDKEY_APOSTROPHE       = 0x8050,  /* ''"' */
    HIDT_KBDKEY_COMMA            = 0x8051,  /* ',<' */
    HIDT_KBDKEY_PERIOD           = 0x8052,  /* '.>' */
    HIDT_KBDKEY_SLASH            = 0x8053,  /* '/?' */
    HIDT_KBDKEY_BACKSLASH        = 0x8054,  /* '\|' */
    HIDT_KBDKEY_ANGLE_BRACKET    = 0x8055,  /* '<>' or '\|' on RT 102-key */

    /* Modifier keys */
    HIDT_KBDKEY_FUNCTION         = 0x8056,
    HIDT_KBDKEY_META             = 0x8057,
    HIDT_KBDKEY_META_LEFT        = 0x8058,
    HIDT_KBDKEY_META_RIGHT       = 0x8059,
    HIDT_KBDKEY_CTRL             = 0x805a,
    HIDT_KBDKEY_CTRL_LEFT        = 0x805b,
    HIDT_KBDKEY_CTRL_RIGHT       = 0x805c,
    HIDT_KBDKEY_ALT              = 0x805d,
    HIDT_KBDKEY_ALT_LEFT         = 0x805e,
    HIDT_KBDKEY_ALT_RIGHT        = 0x805f,
    HIDT_KBDKEY_SHIFT            = 0x8060,
    HIDT_KBDKEY_SHIFT_LEFT       = 0x8061,
    HIDT_KBDKEY_SHIFT_RIGHT      = 0x8062,

    /* Aliases */
    HIDT_KBDKEY_RETURN           = 0x8002, /* = HIDT_KBDKEY_ENTER */
    HIDT_KBDKEY_ESC              = 0x8039, /* = HIDT_KBDKEY_ESCAPE */
    HIDT_KBDKEY_NUMPAD_PLUS      = 0x8031, /* = HIDT_KBDKEY_NUMPAD_ADD */
    HIDT_KBDKEY_NUMPAD_MINUS     = 0x8032, /* = HIDT_KBDKEY_NUMPAD_SUBTRACT */
    HIDT_KBDKEY_NUMPAD_RETURN    = 0x8038, /* = HIDT_KBDKEY_NUMPAD_ENTER */
    HIDT_KBDKEY_STANDBY          = 0x8049, /* = HIDT_KBDKEY_SLEEP */
    HIDT_KBDKEY_GRAVE            = 0x804a, /* = HIDT_KBDKEY_LEFT_QUOTE */
    HIDT_KBDKEY_FN               = 0x8056, /* = HIDT_KBDKEY_FUNCTION */
    HIDT_KBDKEY_OPTION           = 0x805d, /* = HIDT_KBDKEY_ALT */
    HIDT_KBDKEY_OPTION_LEFT      = 0x805e, /* = HIDT_KBDKEY_ALT_LEFT */
    HIDT_KBDKEY_OPTION_RIGHT     = 0x805f  /* = HIDT_KBDKEY_ALT_RIGHT */
} HidtKeyboardKey;

/** @brief Keyboard event types (mirrors hidt::KeyboardEvent::EventType) */
typedef enum HidtKeyboardEventType
{
    HIDT_KBD_ET_NONE        = 0,
    HIDT_KBD_ET_PRESS       = 1,
    HIDT_KBD_ET_RELEASE     = 2,
    HIDT_KBD_ET_SLEEP       = 3
} HidtKeyboardEventType;

/** @brief Keyboard event (mirrors hidt::KeyboardEvent). */
typedef struct HidtKeyboardEvent
{
    HidtKeyboardEventType type;
    union
    {
        int32_t  native_key;    /**< Platform native key value (ET_PRESS / ET_RELEASE) */
        uint64_t sleep_ms;      /**< Sleep time in milliseconds (ET_SLEEP) */
    } u;
    uint64_t timestamp;         /**< Event timestamp in nanoseconds */
} HidtKeyboardEvent;

/* ==================== Mouse Event ==================== */

/** @brief Mouse event types (mirrors hidt::MouseEvent::EventType) */
typedef enum HidtMouseEventType
{
    HIDT_MS_ET_NONE         = 0,
    HIDT_MS_ET_ABS_MOVE     = 1,
    HIDT_MS_ET_REL_MOVE     = 2,
    HIDT_MS_ET_WHEEL        = 3,
    HIDT_MS_ET_DRAG         = 4,
    HIDT_MS_ET_PRESS        = 5,
    HIDT_MS_ET_RELEASE      = 6,
    HIDT_MS_ET_SLEEP        = 7
} HidtMouseEventType;

/** @brief Mouse event (mirrors hidt::MouseEvent) */
typedef struct HidtMouseEvent
{
    HidtMouseEventType type;
    union
    {
        HidtAbsolutePos abs_pos;        /**< Absolute movement coordinates (ET_ABS_MOVE) */
        HidtRelativePos rel_pos;        /**< Relative movement delta (ET_REL_MOVE) */
        int32_t         wheel_delta;    /**< Wheel scroll amount, unit 120 (ET_WHEEL) */
        struct
        {
            HidtAbsolutePos pos;        /**< Drag target coordinates */
            HidtMouseButton button;     /**< Drag button */
        } drag;                         /**< Drag event data (ET_DRAG) */
        HidtMouseButton button;         /**< Mouse button (ET_PRESS / ET_RELEASE) */
        uint64_t        sleep_ms;       /**< Sleep time in milliseconds (ET_SLEEP) */
    } u;
    uint64_t timestamp;                 /**< Event timestamp in nanoseconds */
} HidtMouseEvent;

/* ==================== HID Event ==================== */

/** @brief HID event types (mirrors hidt::HidEvent::EventType) */
typedef enum HidtHidEventType
{
    HIDT_HID_ET_NONE        = 0,
    HIDT_HID_ET_KEYBOARD    = 1,
    HIDT_HID_ET_MOUSE       = 2,
    HIDT_HID_ET_SLEEP       = 3
} HidtHidEventType;

/** @brief Unified HID event (mirrors hidt::HidEvent) */
typedef struct HidtHidEvent
{
    HidtHidEventType type;
    union
    {
    #ifdef HIDTOOL_C_HAS_KEYBOARD
        HidtKeyboardEvent keyboard_event;   /**< Keyboard event payload (HID_ET_KEYBOARD) */
    #endif
    #ifdef HIDTOOL_C_HAS_MOUSE
        HidtMouseEvent    mouse_event;      /**< Mouse event payload (HID_ET_MOUSE) */
    #endif
        uint64_t          sleep_ms;         /**< Sleep time in milliseconds (HID_ET_SLEEP) */
    } u;
} HidtHidEvent;

/* ==================== Callback Types ==================== */

/**
 * @brief Keyboard event callback.
 * @return Non-zero to propagate the event; zero to block propagation.
 * @note Blocking event propagation may not be supported on all platforms.
 */
typedef int (*HidtKeyboardEventHandler)(const HidtKeyboardEvent* event, void* user_data);

/**
 * @brief Mouse event callback.
 * @return Non-zero to propagate the event; zero to block propagation.
 * @note Blocking event propagation may not be supported on all platforms.
 */
typedef int (*HidtMouseEventHandler)(const HidtMouseEvent* event, void* user_data);

/**
 * @brief Unified HID event callback.
 * @return Non-zero to propagate the event; zero to block propagation.
 * @note Blocking event propagation may not be supported on all platforms.
 */
typedef int (*HidtHidEventHandler)(const HidtHidEvent* event, void* user_data);

/* ==================== HID Type ==================== */

/**
 * @brief Check whether the current environment supports the specified HID type module.
 * @return Non-zero if supported, zero otherwise.
 */
HIDTC_API int hidt_is_hid_type_supported(HidtHidType hid_type);

/* ==================== Keyboard Key Utilities ==================== */

#ifdef HIDTOOL_C_HAS_KEYBOARD

/**
 * @brief Convert a HidtKeyboardKey to its platform-native key value.
 * @return The native key value, or -1 if no mapping exists.
 */
HIDTC_API int32_t hidt_keyboard_key_to_native_key(HidtKeyboardKey key);

/**
 * @brief Convert a platform-native key value to HidtKeyboardKey.
 * @return The corresponding HidtKeyboardKey, or HIDT_KBDKEY_NONE if no mapping exists.
 */
HIDTC_API HidtKeyboardKey hidt_keyboard_key_from_native_key(int32_t native_key);

/* ==================== Keyboard Hooker ==================== */

/** @brief Check if the given native key is currently pressed. */
HIDTC_API int hidt_keyboard_hooker_is_key_pressed(int32_t native_key);

/** @brief Start the keyboard event listener. @return Non-zero on success. */
HIDTC_API int hidt_keyboard_hooker_run(void);

/** @brief Stop the keyboard event listener. */
HIDTC_API void hidt_keyboard_hooker_stop(void);

/** @return Non-zero if the keyboard listener is running. */
HIDTC_API int hidt_keyboard_hooker_is_running(void);

/**
 * @brief Set the keyboard event handler callback.
 * @param handler Callback function, or NULL to unset.
 * @param user_data Arbitrary pointer passed to each callback invocation.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_keyboard_hooker_set_event_handler(
    HidtKeyboardEventHandler handler, void* user_data);

/**
 * @brief Update the user data pointer without changing the callback.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_keyboard_hooker_set_user_data(void* user_data);

/* ==================== Keyboard Simulator ==================== */

/**
 * @brief Initialize the keyboard simulator.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_keyboard_simulator_initialize(void);

/** @brief Destroy (uninitialize) the keyboard simulator. */
HIDTC_API void hidt_keyboard_simulator_destroy(void);

/** @return Non-zero if the keyboard simulator is initialized. */
HIDTC_API int hidt_keyboard_simulator_is_initialized(void);

/**
 * @brief Send a single keyboard event.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_keyboard_simulator_send_event(const HidtKeyboardEvent* event);

/**
 * @brief Send multiple keyboard events.
 * @return The number of events successfully sent.
 */
HIDTC_API size_t hidt_keyboard_simulator_send_events(
    const HidtKeyboardEvent* events, size_t count);

/**
 * @brief Simulate pressing a key by native key value.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_keyboard_simulator_press_key_native(int32_t native_key);

/**
 * @brief Simulate pressing a key by HidtKeyboardKey.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_keyboard_simulator_press_key(HidtKeyboardKey key);

/**
 * @brief Simulate releasing a key by native key value.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_keyboard_simulator_release_key_native(int32_t native_key);

/**
 * @brief Simulate releasing a key by HidtKeyboardKey.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_keyboard_simulator_release_key(HidtKeyboardKey key);

/**
 * @brief Simulate a key click (press + release) by native key value.
 * @param interval Delay in milliseconds between press and release.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_keyboard_simulator_click_key_native(int32_t native_key, uint64_t interval);

/**
 * @brief Simulate a key click (press + release) by HidtKeyboardKey.
 * @param interval Delay in milliseconds between press and release.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_keyboard_simulator_click_key(HidtKeyboardKey key, uint64_t interval);

#endif /* HIDTOOL_C_HAS_KEYBOARD */

/* ==================== Mouse Hooker ==================== */

#ifdef HIDTOOL_C_HAS_MOUSE

/** @brief Check if the given mouse button is currently pressed. */
HIDTC_API int hidt_mouse_hooker_is_button_pressed(HidtMouseButton button);

/**
 * @brief Get the current cursor position.
 * @note Always returns {0, 0} on Linux.
 */
HIDTC_API HidtAbsolutePos hidt_mouse_hooker_get_cursor_pos(void);

/**
 * @brief Start the mouse event listener.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_hooker_run(void);

/** @brief Stop the mouse event listener. */
HIDTC_API void hidt_mouse_hooker_stop(void);

/** @return Non-zero if the mouse listener is running. */
HIDTC_API int hidt_mouse_hooker_is_running(void);

/**
 * @brief Set the mouse event handler callback.
 * @param handler Callback function, or NULL to unset.
 * @param user_data Arbitrary pointer passed to each callback invocation.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_hooker_set_event_handler(
    HidtMouseEventHandler handler, void* user_data);

/**
 * @brief Update the user data pointer without changing the callback.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_hooker_set_user_data(void* user_data);

/* ==================== Mouse Simulator ==================== */

/**
 * @brief Get the coordinate range for absolute mouse movement in the current environment.
 * @sa hidt_mouse_simulator_move_to()
 */
HIDTC_API HidtAbsolutePosRange hidt_mouse_simulator_get_absolute_move_range(void);

/**
 * @brief Initialize the mouse simulator.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_initialize(void);

/** @brief Destroy (uninitialize) the mouse simulator. */
HIDTC_API void hidt_mouse_simulator_destroy(void);

/** @return Non-zero if the mouse simulator is initialized. */
HIDTC_API int hidt_mouse_simulator_is_initialized(void);

/**
 * @brief Send a single mouse event.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_send_event(const HidtMouseEvent* event);

/**
 * @brief Send multiple mouse events.
 * @return The number of events successfully sent.
 */
HIDTC_API size_t hidt_mouse_simulator_send_events(const HidtMouseEvent* events, size_t count);

/**
 * @brief Move the cursor to an absolute position.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_move_to(int32_t x, int32_t y);

/**
 * @brief Move the cursor by a relative delta.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_move_by(int32_t dx, int32_t dy);

/**
 * @brief Scroll the mouse wheel.
 * @param wheel_delta Scroll amount in units of 120. Positive = away from user.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_wheel(int32_t wheel_delta);

/**
 * @brief Press a mouse button at the current position.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_press_button(HidtMouseButton button);

/**
 * @brief Release a mouse button at the current position.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_release_button(HidtMouseButton button);

/**
 * @brief Click a mouse button at the current position.
 * @param interval Delay in milliseconds between press and release.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_click_button(HidtMouseButton button, uint64_t interval);

/**
 * @brief Double-click a mouse button at the current position.
 * @param interval1 Delay between press and release within each click.
 * @param interval2 Delay between the two click events.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_double_click_button(
    HidtMouseButton button, uint64_t interval1, uint64_t interval2);

/**
 * @brief Scroll the mouse wheel at the specified position.
 * @param interval Delay between the move and wheel events.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_wheel_at(
    int32_t x, int32_t y, int32_t wheel_delta, uint64_t interval);

/**
 * @brief Press a mouse button at the specified position.
 * @param interval Delay between the move and press events.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_press_button_at(
    int32_t x, int32_t y, HidtMouseButton button, uint64_t interval);

/**
 * @brief Release a mouse button at the specified position.
 * @param interval Delay between the move and release events.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_release_button_at(
    int32_t x, int32_t y, HidtMouseButton button, uint64_t interval);

/**
 * @brief Click a mouse button at the specified position.
 * @param interval1 Delay between the move and click events.
 * @param interval2 Delay between press and release.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_click_button_at(
    int32_t x, int32_t y, HidtMouseButton button, uint64_t interval1, uint64_t interval2);

/**
 * @brief Double-click a mouse button at the specified position.
 * @param interval1 Delay between the move and first click.
 * @param interval2 Delay between press and release within each click.
 * @param interval3 Delay between the two click events.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_double_click_button_at(
    int32_t x, int32_t y, HidtMouseButton button,
    uint64_t interval1, uint64_t interval2, uint64_t interval3);

/**
 * @brief Move the mouse with drag semantics to the specified absolute position.
 * @note
 * On Windows and Linux this is equivalent to an absolute move.
 * On macOS it is a native drag event.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_drag_to(int32_t x, int32_t y, HidtMouseButton button);

/**
 * @brief Perform a drag from the current position to endPos (press + drag + release).
 * @param interval Delay between each sub-event.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_drag_combo(
    int32_t end_x, int32_t end_y, HidtMouseButton button, uint64_t interval);

/**
 * @brief Perform a drag from startPos to endPos (move + press + drag + release).
 * @param interval1 Delay between the move-to-start and press events.
 * @param interval2 Delay between the press, drag, and release events.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_mouse_simulator_drag_combo_from(
    int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y, HidtMouseButton button,
    uint64_t interval1, uint64_t interval2);

#endif /* HIDTOOL_C_HAS_MOUSE */

/* ==================== HID Hooker ==================== */

/**
 * @brief Check if the current environment supports blocking event propagation.
 * @return Non-zero if supported.
 */
HIDTC_API int hidt_hid_hooker_is_support_block_event_propagation(void);

#ifdef HIDTOOL_C_HAS_KEYBOARD
/** @brief Check if the given native key is currently pressed (via HidHooker). */
HIDTC_API int hidt_hid_hooker_is_key_pressed(int32_t native_key);
#endif

#ifdef HIDTOOL_C_HAS_MOUSE
/** @brief Check if the given mouse button is currently pressed (via HidHooker). */
HIDTC_API int hidt_hid_hooker_is_button_pressed(HidtMouseButton button);

/**
 * @brief Get the current cursor position (via HidHooker).
 * @note Always returns {0, 0} on Linux.
 */
HIDTC_API HidtAbsolutePos hidt_hid_hooker_get_cursor_pos(void);
#endif

/**
 * @brief Start the unified HID event listener.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_hid_hooker_run(void);

/** @brief Stop the unified HID event listener. */
HIDTC_API void hidt_hid_hooker_stop(void);

/** @return Non-zero if the unified HID listener is running. */
HIDTC_API int hidt_hid_hooker_is_running(void);

/**
 * @brief Set the unified HID event handler callback.
 * @param handler Callback function, or NULL to unset.
 * @param user_data Arbitrary pointer passed to each callback invocation.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_hid_hooker_set_event_handler(HidtHidEventHandler handler, void* user_data);

/**
 * @brief Update the user data pointer without changing the callback.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_hid_hooker_set_user_data(void* user_data);

/* ==================== HID Simulator ==================== */

/**
 * @brief Initialize the unified HID simulator.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_hid_simulator_initialize(void);

/** @brief Destroy (uninitialize) the unified HID simulator. */
HIDTC_API void hidt_hid_simulator_destroy(void);

/** @return Non-zero if the unified HID simulator is initialized. */
HIDTC_API int hidt_hid_simulator_is_initialized(void);

/**
 * @brief Send a single HID event via the unified simulator.
 * @return Non-zero on success.
 */
HIDTC_API int hidt_hid_simulator_send_event(const HidtHidEvent* event);

/**
 * @brief Send multiple HID events via the unified simulator.
 * @return The number of events successfully sent.
 */
HIDTC_API size_t hidt_hid_simulator_send_events(const HidtHidEvent* events, size_t count);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // !HIDTOOL_C_H
