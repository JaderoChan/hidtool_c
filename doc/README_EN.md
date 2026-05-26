# HID Tool C — English Documentation

[[**简体中文**](README_ZH.md) | **English**]

C language bindings for the [hidtool](https://github.com/JaderoChan/hidtool) C++ library.
Provides a pure C API for cross-platform keyboard and mouse input simulation and global event listening.

---

## Table of Contents

- [HID Tool C — English Documentation](#hid-tool-c--english-documentation)
  - [Table of Contents](#table-of-contents)
  - [Features](#features)
  - [Platform Support](#platform-support)
  - [Requirements](#requirements)
  - [Build](#build)
    - [CMake Options](#cmake-options)
  - [Integration](#integration)
    - [CMake — add\_subdirectory](#cmake--add_subdirectory)
  - [API Reference](#api-reference)
    - [Types](#types)
      - [Position Types](#position-types)
      - [`HidtcMouseButton`](#hidtcmousebutton)
      - [`HidtcKeyboardEvent`](#hidtckeyboardevent)
      - [`HidtcMouseEvent`](#hidtcmouseevent)
      - [`HidtcHidEvent`](#hidtchidevent)
      - [Callback Types](#callback-types)
    - [HID Type Utilities](#hid-type-utilities)
    - [Keyboard Key Utilities](#keyboard-key-utilities)
    - [Keyboard Hooker](#keyboard-hooker)
    - [Keyboard Simulator](#keyboard-simulator)
    - [Mouse Hooker](#mouse-hooker)
    - [Mouse Simulator](#mouse-simulator)
    - [HID Hooker (Unified)](#hid-hooker-unified)
    - [HID Simulator (Unified)](#hid-simulator-unified)
  - [Usage Examples](#usage-examples)
    - [Example 1 — Monitor All Keyboard Events](#example-1--monitor-all-keyboard-events)
    - [Example 2 — Simulate Keyboard Input (Type "Hello")](#example-2--simulate-keyboard-input-type-hello)
    - [Example 3 — Simulate Ctrl+C (Copy)](#example-3--simulate-ctrlc-copy)
    - [Example 4 — Monitor Mouse Events](#example-4--monitor-mouse-events)
    - [Example 5 — Simulate Mouse: Move, Click, Scroll](#example-5--simulate-mouse-move-click-scroll)
    - [Example 6 — Unified HID Listener with User Data](#example-6--unified-hid-listener-with-user-data)
    - [Example 7 — Block a Specific Key (Platform Support Required)](#example-7--block-a-specific-key-platform-support-required)

---

## Features

- **Keyboard Module**: Global keyboard event listening, keyboard input simulation
- **Mouse Module**: Global mouse event listening, mouse input simulation (move, click, wheel, drag)
- Pure C header (`hidtool_c.h`) — no C++ required in your project
- Cross-platform: Windows, macOS, Linux
- Supports static / shared library builds
- Thread-safe callback registration via atomic stores

---

## Platform Support

| Platform | Status | Notes |
|----------|--------|-------|
| Windows  | ✅     | —     |
| macOS    | ✅     | Requires Accessibility permissions |
| Linux    | ✅     | Requires administrator privileges  |

> **macOS**: Due to macOS API design, simulation functions cannot confirm success.
> Even when a function returns non-zero, the action may have no effect unless your application has been granted **Accessibility** permissions.

---

## Requirements

- CMake >= 3.26
- C11 compiler (anonymous unions used in event structs)
- C++11 compiler (for building the binding layer)
- **macOS**: CoreFoundation, Carbon, CoreGraphics (found automatically by CMake)
- **Linux**: pthreads

---

## Build

```bash
git clone --recurse-submodules <this-repo-url>
cd hidtool_c
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `HIDTOOL_BUILD_WITH_KEYBOARD`  | `ON`  | Build keyboard module |
| `HIDTOOL_BUILD_WITH_MOUSE`     | `ON`  | Build mouse module |
| `HIDTOOL_C_BUILD_SHARED`       | `OFF` | Build `hidtool_c` as a shared library |
| `HIDTOOL_C_BUILD_EXAMPLES`     | `OFF` | Build example programs in `example/` |

To build with examples:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DHIDTOOL_C_BUILD_EXAMPLES=ON
cmake --build build
```

```bash
# Build shared library
cmake -B build -DCMAKE_BUILD_TYPE=Release -DHIDTOOL_C_BUILD_SHARED=ON

# Keyboard module only
cmake -B build -DHIDTOOL_BUILD_WITH_MOUSE=OFF
```

---

## Integration

### CMake — add_subdirectory

```cmake
add_subdirectory(hidtool_c)
target_link_libraries(your_target PRIVATE hidtool_c)
```

Then include the header in your C source:

```c
#include <hidtool_c/hidtool_c.h>
```

---

## API Reference

### Types

#### Position Types

```c
typedef struct { int32_t x;  int32_t y;  } HidtcAbsolutePos;
typedef struct { int32_t dx; int32_t dy; } HidtcRelativePos;
typedef struct { int32_t min_x; int32_t max_x;
                 int32_t min_y; int32_t max_y; } HidtcAbsolutePosRange;
```

#### `HidtcMouseButton`

```c
typedef enum {
    HIDTC_MSBTN_NONE    = 0,
    HIDTC_MSBTN_LEFT    = 1,
    HIDTC_MSBTN_RIGHT   = 2,
    HIDTC_MSBTN_MIDDLE  = 3,
    HIDTC_MSBTN_BACK    = 4,
    HIDTC_MSBTN_FORWARD = 5
} HidtcMouseButton;
```

#### `HidtcKeyboardEvent`

```c
typedef struct {
    HidtcKeyboardEventType type;   /* HIDTC_KBD_ET_NONE/PRESS/RELEASE/SLEEP */
    union {
        int32_t nativeKey;         /* ET_PRESS / ET_RELEASE */
        size_t  sleepMs;           /* ET_SLEEP */
    };
    uint64_t timestamp;            /* nanoseconds */
} HidtcKeyboardEvent;
```

#### `HidtcMouseEvent`

```c
typedef struct {
    HidtcMouseEventType type;
    union {
        HidtcAbsolutePos absPos;   /* ET_ABS_MOVE */
        HidtcRelativePos relPos;   /* ET_REL_MOVE */
        int32_t          wheelDelta; /* ET_WHEEL, unit 120 */
        struct {
            HidtcAbsolutePos pos;
            HidtcMouseButton button;
        } drag;                    /* ET_DRAG */
        HidtcMouseButton button;   /* ET_PRESS / ET_RELEASE */
        size_t           sleepMs;  /* ET_SLEEP */
    };
    uint64_t timestamp;
} HidtcMouseEvent;
```

#### `HidtcHidEvent`

```c
typedef struct {
    HidtcHidEventType type;        /* HID_ET_NONE/KEYBOARD/MOUSE/SLEEP */
    union {
        HidtcKeyboardEvent keyboardEvent;
        HidtcMouseEvent    mouseEvent;
        size_t             sleepMs;
    };
} HidtcHidEvent;
```

#### Callback Types

```c
/* Return non-zero to propagate the event; zero to block it. */
typedef int (*HidtcKeyboardEventHandler)(const HidtcKeyboardEvent* event, void* userData);
typedef int (*HidtcMouseEventHandler)   (const HidtcMouseEvent*    event, void* userData);
typedef int (*HidtcHidEventHandler)     (const HidtcHidEvent*      event, void* userData);
```

> Blocking event propagation is not supported on all platforms. Check with
> `hidtc_hid_hooker_is_support_block_event_propagation()`.

---

### HID Type Utilities

```c
/* Returns non-zero if the specified HID type module is available at runtime. */
int hidtc_is_hid_type_supported(HidtcHidType hidType);
```

---

### Keyboard Key Utilities

```c
/* Convert HidtcKeyboardKey -> platform native key. Returns -1 if no mapping exists. */
int32_t hidtc_keyboard_key_to_native_key(HidtcKeyboardKey key);

/* Convert platform native key -> HidtcKeyboardKey. Returns HIDTC_KBDKEY_NONE if no mapping. */
HidtcKeyboardKey hidtc_keyboard_key_from_native_key(int32_t nativeKey);
```

Common key constants (`HidtcKeyboardKey`):

| Constant | Description |
|----------|-------------|
| `HIDTC_KBDKEY_0` – `HIDTC_KBDKEY_9` | Number row |
| `HIDTC_KBDKEY_A` – `HIDTC_KBDKEY_Z` | Letter keys |
| `HIDTC_KBDKEY_F1` – `HIDTC_KBDKEY_F24` | Function keys |
| `HIDTC_KBDKEY_NUMPAD_0` – `HIDTC_KBDKEY_NUMPAD_9` | Numpad digits |
| `HIDTC_KBDKEY_ENTER` / `HIDTC_KBDKEY_RETURN` | Enter |
| `HIDTC_KBDKEY_ESCAPE` / `HIDTC_KBDKEY_ESC` | Escape |
| `HIDTC_KBDKEY_SPACE` | Space |
| `HIDTC_KBDKEY_BACKSPACE` | Backspace |
| `HIDTC_KBDKEY_TAB` | Tab |
| `HIDTC_KBDKEY_LEFT/RIGHT/UP/DOWN` | Arrow keys |
| `HIDTC_KBDKEY_CTRL` / `HIDTC_KBDKEY_CTRL_LEFT` / `HIDTC_KBDKEY_CTRL_RIGHT` | Ctrl |
| `HIDTC_KBDKEY_SHIFT` / `HIDTC_KBDKEY_SHIFT_LEFT` / `HIDTC_KBDKEY_SHIFT_RIGHT` | Shift |
| `HIDTC_KBDKEY_ALT` / `HIDTC_KBDKEY_ALT_LEFT` / `HIDTC_KBDKEY_ALT_RIGHT` | Alt / Option |
| `HIDTC_KBDKEY_META` / `HIDTC_KBDKEY_META_LEFT` / `HIDTC_KBDKEY_META_RIGHT` | Win / Command |
| `HIDTC_KBDKEY_CAPS_LOCK` | Caps Lock |
| `HIDTC_KBDKEY_DELETE` | Delete |
| `HIDTC_KBDKEY_INSERT` | Insert |

---

### Keyboard Hooker

```c
/* Check if a key is currently held down (by native key value). */
int  hidtc_keyboard_hooker_is_key_pressed(int32_t nativeKey);

/* Start / stop the global keyboard listener. */
int  hidtc_keyboard_hooker_run(void);
void hidtc_keyboard_hooker_stop(void);
int  hidtc_keyboard_hooker_is_running(void);

/* Register a callback. Pass NULL to unregister. */
int hidtc_keyboard_hooker_set_event_handler(HidtcKeyboardEventHandler handler,
                                             void* userData);
/* Update userData without changing the callback. */
int hidtc_keyboard_hooker_set_user_data(void* userData);
```

> Do **not** call `hidtc_keyboard_hooker_*` functions from inside the event callback.

---

### Keyboard Simulator

```c
int    hidtc_keyboard_simulator_initialize(void);
void   hidtc_keyboard_simulator_destroy(void);
int    hidtc_keyboard_simulator_is_initialized(void);

/* Send individual events. */
int    hidtc_keyboard_simulator_send_event(const HidtcKeyboardEvent* event);
size_t hidtc_keyboard_simulator_send_events(const HidtcKeyboardEvent* events, size_t count);

/* Convenience functions. */
int hidtc_keyboard_simulator_press_key_native(int32_t nativeKey);
int hidtc_keyboard_simulator_press_key(HidtcKeyboardKey key);

int hidtc_keyboard_simulator_release_key_native(int32_t nativeKey);
int hidtc_keyboard_simulator_release_key(HidtcKeyboardKey key);

/* Click = press + optional delay + release. */
int hidtc_keyboard_simulator_click_key_native(int32_t nativeKey, size_t intervalMs);
int hidtc_keyboard_simulator_click_key(HidtcKeyboardKey key,     size_t intervalMs);
```

---

### Mouse Hooker

```c
int               hidtc_mouse_hooker_is_button_pressed(HidtcMouseButton button);
HidtcAbsolutePos  hidtc_mouse_hooker_get_cursor_pos(void); /* Always {0,0} on Linux */

int  hidtc_mouse_hooker_run(void);
void hidtc_mouse_hooker_stop(void);
int  hidtc_mouse_hooker_is_running(void);

int hidtc_mouse_hooker_set_event_handler(HidtcMouseEventHandler handler, void* userData);
int hidtc_mouse_hooker_set_user_data(void* userData);
```

---

### Mouse Simulator

```c
/* Get the valid coordinate range for absolute movement on the current display. */
HidtcAbsolutePosRange hidtc_mouse_simulator_get_absolute_move_range(void);

int    hidtc_mouse_simulator_initialize(void);
void   hidtc_mouse_simulator_destroy(void);
int    hidtc_mouse_simulator_is_initialized(void);

int    hidtc_mouse_simulator_send_event(const HidtcMouseEvent* event);
size_t hidtc_mouse_simulator_send_events(const HidtcMouseEvent* events, size_t count);

/* Movement */
int hidtc_mouse_simulator_move_to(int32_t x, int32_t y);
int hidtc_mouse_simulator_move_by(int32_t dx, int32_t dy);

/* Wheel — unit 120; positive = away from user */
int hidtc_mouse_simulator_wheel(int32_t wheelDelta);

/* Button actions at the current cursor position */
int hidtc_mouse_simulator_press_button(HidtcMouseButton button);
int hidtc_mouse_simulator_release_button(HidtcMouseButton button);
int hidtc_mouse_simulator_click_button(HidtcMouseButton button, size_t intervalMs);
int hidtc_mouse_simulator_double_click_button(HidtcMouseButton button,
                                               size_t interval1Ms, size_t interval2Ms);

/* Button actions at a specified absolute position (move first, then act) */
int hidtc_mouse_simulator_wheel_at(int32_t x, int32_t y, int32_t wheelDelta, size_t intervalMs);
int hidtc_mouse_simulator_press_button_at(int32_t x, int32_t y,
                                           HidtcMouseButton button, size_t intervalMs);
int hidtc_mouse_simulator_release_button_at(int32_t x, int32_t y,
                                             HidtcMouseButton button, size_t intervalMs);
int hidtc_mouse_simulator_click_button_at(int32_t x, int32_t y, HidtcMouseButton button,
                                           size_t interval1Ms, size_t interval2Ms);
int hidtc_mouse_simulator_double_click_button_at(int32_t x, int32_t y, HidtcMouseButton button,
                                                  size_t interval1Ms, size_t interval2Ms,
                                                  size_t interval3Ms);

/* Drag */
int hidtc_mouse_simulator_drag_to(int32_t x, int32_t y, HidtcMouseButton button);
int hidtc_mouse_simulator_drag_combo(int32_t endX, int32_t endY,
                                      HidtcMouseButton button, size_t intervalMs);
int hidtc_mouse_simulator_drag_combo_from(int32_t startX, int32_t startY,
                                           int32_t endX,   int32_t endY,
                                           HidtcMouseButton button,
                                           size_t interval1Ms, size_t interval2Ms);
```

---

### HID Hooker (Unified)

Combines both keyboard and mouse listeners into a single callback.

```c
int hidtc_hid_hooker_is_support_block_event_propagation(void);
int hidtc_hid_hooker_is_key_pressed(int32_t nativeKey);       /* keyboard only */
int hidtc_hid_hooker_is_button_pressed(HidtcMouseButton button); /* mouse only */
HidtcAbsolutePos hidtc_hid_hooker_get_cursor_pos(void);       /* mouse only */

int  hidtc_hid_hooker_run(void);
void hidtc_hid_hooker_stop(void);
int  hidtc_hid_hooker_is_running(void);

int hidtc_hid_hooker_set_event_handler(HidtcHidEventHandler handler, void* userData);
int hidtc_hid_hooker_set_user_data(void* userData);
```

---

### HID Simulator (Unified)

Dispatches keyboard, mouse, and sleep events through a single interface.

```c
int    hidtc_hid_simulator_initialize(void);
void   hidtc_hid_simulator_destroy(void);
int    hidtc_hid_simulator_is_initialized(void);

int    hidtc_hid_simulator_send_event(const HidtcHidEvent* event);
size_t hidtc_hid_simulator_send_events(const HidtcHidEvent* events, size_t count);
```

---

## Usage Examples

### Example 1 — Monitor All Keyboard Events

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

#ifdef _WIN32
#  include <windows.h>
#  define SLEEP_MS(ms) Sleep(ms)
#else
#  include <unistd.h>
#  define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

int on_key_event(const HidtcKeyboardEvent* event, void* userData)
{
    (void)userData;
    if (event->type == HIDTC_KBD_ET_PRESS)
    {
        HidtcKeyboardKey key = hidtc_keyboard_key_from_native_key(event->nativeKey);
        printf("[PRESS]   native=0x%X  key_enum=0x%X\n", event->nativeKey, (unsigned)key);
    }
    else if (event->type == HIDTC_KBD_ET_RELEASE)
    {
        printf("[RELEASE] native=0x%X\n", event->nativeKey);
    }
    return 1; /* propagate */
}

int main(void)
{
    hidtc_keyboard_hooker_set_event_handler(on_key_event, NULL);
    hidtc_keyboard_hooker_run();
    printf("Listening for keyboard events. Press any key (Ctrl+C to quit)...\n");

    /* Keep the program running */
    while (hidtc_keyboard_hooker_is_running())
        SLEEP_MS(100);

    return 0;
}
```

---

### Example 2 — Simulate Keyboard Input (Type "Hello")

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

int main(void)
{
    if (!hidtc_keyboard_simulator_initialize())
    {
        fprintf(stderr, "Failed to initialize keyboard simulator.\n");
        return 1;
    }

    /* Type "Hello" — shift+H, e, l, l, o */
    hidtc_keyboard_simulator_press_key(HIDTC_KBDKEY_SHIFT);
    hidtc_keyboard_simulator_click_key(HIDTC_KBDKEY_H, 0);
    hidtc_keyboard_simulator_release_key(HIDTC_KBDKEY_SHIFT);

    hidtc_keyboard_simulator_click_key(HIDTC_KBDKEY_E, 0);
    hidtc_keyboard_simulator_click_key(HIDTC_KBDKEY_L, 0);
    hidtc_keyboard_simulator_click_key(HIDTC_KBDKEY_L, 0);
    hidtc_keyboard_simulator_click_key(HIDTC_KBDKEY_O, 0);

    hidtc_keyboard_simulator_destroy();
    return 0;
}
```

---

### Example 3 — Simulate Ctrl+C (Copy)

```c
#include <hidtool_c/hidtool_c.h>

int main(void)
{
    hidtc_keyboard_simulator_initialize();

    /* Build and send events as a batch */
    HidtcKeyboardEvent events[4];

    events[0].type      = HIDTC_KBD_ET_PRESS;
    events[0].nativeKey = hidtc_keyboard_key_to_native_key(HIDTC_KBDKEY_CTRL);
    events[0].timestamp = 0;

    events[1].type      = HIDTC_KBD_ET_PRESS;
    events[1].nativeKey = hidtc_keyboard_key_to_native_key(HIDTC_KBDKEY_C);
    events[1].timestamp = 0;

    events[2].type      = HIDTC_KBD_ET_RELEASE;
    events[2].nativeKey = hidtc_keyboard_key_to_native_key(HIDTC_KBDKEY_C);
    events[2].timestamp = 0;

    events[3].type      = HIDTC_KBD_ET_RELEASE;
    events[3].nativeKey = hidtc_keyboard_key_to_native_key(HIDTC_KBDKEY_CTRL);
    events[3].timestamp = 0;

    hidtc_keyboard_simulator_send_events(events, 4);
    hidtc_keyboard_simulator_destroy();
    return 0;
}
```

---

### Example 4 — Monitor Mouse Events

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

int on_mouse_event(const HidtcMouseEvent* event, void* userData)
{
    (void)userData;
    switch (event->type)
    {
    case HIDTC_MS_ET_ABS_MOVE:
        printf("[MOVE]  x=%d  y=%d\n", event->absPos.x, event->absPos.y);
        break;
    case HIDTC_MS_ET_WHEEL:
        printf("[WHEEL] delta=%d\n", event->wheelDelta);
        break;
    case HIDTC_MS_ET_PRESS:
        printf("[PRESS]   button=%d\n", (int)event->button);
        break;
    case HIDTC_MS_ET_RELEASE:
        printf("[RELEASE] button=%d\n", (int)event->button);
        break;
    default:
        break;
    }
    return 1;
}

int main(void)
{
    hidtc_mouse_hooker_set_event_handler(on_mouse_event, NULL);
    hidtc_mouse_hooker_run();
    printf("Listening for mouse events...\n");

    /* Wait until the listener stops */
#ifdef _WIN32
    #include <windows.h>
    Sleep(10000);
#else
    #include <unistd.h>
    sleep(10);
#endif

    hidtc_mouse_hooker_stop();
    return 0;
}
```

---

### Example 5 — Simulate Mouse: Move, Click, Scroll

```c
#include <hidtool_c/hidtool_c.h>

#ifdef _WIN32
#  include <windows.h>
#  define SLEEP_MS(ms) Sleep(ms)
#else
#  include <unistd.h>
#  define SLEEP_MS(ms) usleep((ms) * 1000)
#endif

int main(void)
{
    if (!hidtc_mouse_simulator_initialize())
        return 1;

    HidtcAbsolutePosRange range = hidtc_mouse_simulator_get_absolute_move_range();

    /* Move to the center of the screen */
    int32_t cx = (range.min_x + range.max_x) / 2;
    int32_t cy = (range.min_y + range.max_y) / 2;
    hidtc_mouse_simulator_move_to(cx, cy);
    SLEEP_MS(200);

    /* Left click */
    hidtc_mouse_simulator_click_button(HIDTC_MSBTN_LEFT, 50);
    SLEEP_MS(200);

    /* Double click */
    hidtc_mouse_simulator_double_click_button(HIDTC_MSBTN_LEFT, 50, 100);
    SLEEP_MS(200);

    /* Scroll up */
    hidtc_mouse_simulator_wheel(120);
    SLEEP_MS(100);

    /* Relative move */
    hidtc_mouse_simulator_move_by(50, 50);
    SLEEP_MS(100);

    /* Drag from (cx,cy) to (cx+200, cy+100) */
    hidtc_mouse_simulator_drag_combo_from(cx, cy, cx + 200, cy + 100,
                                           HIDTC_MSBTN_LEFT, 10, 0);

    hidtc_mouse_simulator_destroy();
    return 0;
}
```

---

### Example 6 — Unified HID Listener with User Data

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int keyPressCount;
    int mouseClickCount;
} Stats;

int on_hid_event(const HidtcHidEvent* event, void* userData)
{
    Stats* stats = (Stats*)userData;

    if (event->type == HIDTC_HID_ET_KEYBOARD &&
        event->keyboardEvent.type == HIDTC_KBD_ET_PRESS)
    {
        stats->keyPressCount++;
        printf("Total key presses: %d\n", stats->keyPressCount);
    }
    else if (event->type == HIDTC_HID_ET_MOUSE &&
             event->mouseEvent.type == HIDTC_MS_ET_PRESS)
    {
        stats->mouseClickCount++;
        printf("Total mouse clicks: %d\n", stats->mouseClickCount);
    }
    return 1;
}

int main(void)
{
    Stats stats = {0, 0};

    hidtc_hid_hooker_set_event_handler(on_hid_event, &stats);
    hidtc_hid_hooker_run();
    printf("Monitoring input. Press Ctrl+C to exit.\n");

    /* Block until stopped */
    while (hidtc_hid_hooker_is_running())
    {
#ifdef _WIN32
        Sleep(100);
#else
        usleep(100000);
#endif
    }

    printf("Final — keys: %d, clicks: %d\n",
           stats.keyPressCount, stats.mouseClickCount);
    return 0;
}
```

---

### Example 7 — Block a Specific Key (Platform Support Required)

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

int on_key_event(const HidtcKeyboardEvent* event, void* userData)
{
    /* Block the Escape key */
    if (event->type == HIDTC_KBD_ET_PRESS)
    {
        HidtcKeyboardKey key = hidtc_keyboard_key_from_native_key(event->nativeKey);
        if (key == HIDTC_KBDKEY_ESCAPE)
        {
            printf("Escape key blocked.\n");
            return 0; /* block propagation */
        }
    }
    return 1; /* propagate everything else */
}

int main(void)
{
    if (!hidtc_hid_hooker_is_support_block_event_propagation())
    {
        printf("This platform does not support blocking event propagation.\n");
        return 1;
    }

    hidtc_keyboard_hooker_set_event_handler(on_key_event, NULL);
    hidtc_keyboard_hooker_run();
    printf("Running — Escape key is blocked. Press Ctrl+C to quit.\n");

#ifdef _WIN32
    Sleep(INFINITE);
#else
    pause();
#endif
    return 0;
}
```
