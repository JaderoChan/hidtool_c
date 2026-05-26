# hidtool_c — C Bindings for hidtool

[[**简体中文**](README_ZH.md) | **English**]

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/JaderoChan/hidtool_c)
[![Language: C](https://img.shields.io/badge/language-C99-lightgrey.svg)](https://en.wikipedia.org/wiki/C99)
[![Upstream](https://img.shields.io/badge/upstream-hidtool-orange.svg)](https://github.com/JaderoChan/hidtool)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-informational.svg)](https://github.com/JaderoChan/hidtool_c)

Pure C bindings for the [hidtool](https://github.com/JaderoChan/hidtool) C++ library, exposing a complete C99 API for global keyboard / mouse event listening and input simulation.

---

## Table of Contents

- [Features](#features)
- [Platform Support](#platform-support)
- [Requirements](#requirements)
- [Build](#build)
- [Integration](#integration)
- [API Overview](#api-overview)
  - [Types and Enumerations](#types-and-enumerations)
  - [General HID Utilities](#general-hid-utilities)
  - [Keyboard — Key Utilities](#keyboard--key-utilities)
  - [Keyboard — Hooker](#keyboard--hooker)
  - [Keyboard — Simulator](#keyboard--simulator)
  - [Mouse — Hooker](#mouse--hooker)
  - [Mouse — Simulator](#mouse--simulator)
  - [Unified HID Hooker](#unified-hid-hooker)
- [Usage Examples](#usage-examples)
  - [Keyboard Monitor](#keyboard-monitor)
  - [Keyboard Simulator](#keyboard-simulator)
  - [Mouse Monitor](#mouse-monitor)
  - [Mouse Simulator](#mouse-simulator)
- [Notes and Caveats](#notes-and-caveats)

---

## Features

- **Pure C99 API** — include a single header and link the library; no C++ compiler required in your project.
- **Keyboard Module** — global keyboard event listening and keyboard input simulation.
- **Mouse Module** — global mouse event listening, mouse movement (absolute / relative), clicking, scrolling, and dragging.
- **Unified HID Hooker** — receive both keyboard and mouse events through a single callback.
- Cross-platform: Windows, macOS, Linux.
- Static / shared library builds.
- Thread-safe singleton hookers and simulators.

---

## Platform Support

| Platform | Status | Notes |
|----------|--------|-------|
| Windows  | ✅     | — |
| macOS    | ✅     | Requires **Accessibility** permissions for event listening and simulation |
| Linux    | ✅     | Requires **administrator / root** privileges (access to `/dev/input` and `/dev/uinput`) |

> **macOS**: Due to the macOS API design, simulation functions cannot confirm whether an action was applied. Grant the **Accessibility** permission to your application.
>
> **Linux**: On Linux, `hidt_mouse_hooker_get_cursor_pos()` always returns `{0, 0}` because absolute cursor position is not available through the raw input API.

---

## Requirements

- CMake ≥ 3.26
- A **C99** compiler for your project
- A **C++11** compiler to build hidtool\_c itself (the binding layer is compiled as C++)
- **macOS**: CoreFoundation, Carbon, CoreGraphics (found automatically by CMake)
- **Linux**: pthreads

---

## Build

```bash
git clone --recurse-submodules https://github.com/JaderoChan/hidtool_c.git
cd hidtool_c
cmake -B build [options]
cmake --build build
```

> If you already cloned without `--recurse-submodules`, run `git submodule update --init --recursive`.

### CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `HIDTOOL_C_BUILD_SHARED`   | `OFF` | Build hidtool\_c as a shared library |
| `HIDTOOL_C_BUILD_EXAMPLES` | `OFF` | Build example programs |
| `HIDTOOL_BUILD_WITH_KEYBOARD` | `ON`  | Include the keyboard module (propagated from hidtool) |
| `HIDTOOL_BUILD_WITH_MOUSE`    | `ON`  | Include the mouse module (propagated from hidtool) |

```bash
# Default static library, Release build
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Shared library
cmake -B build -DCMAKE_BUILD_TYPE=Release -DHIDTOOL_C_BUILD_SHARED=ON

# Keyboard module only
cmake -B build -DHIDTOOL_BUILD_WITH_MOUSE=OFF

# With examples
cmake -B build -DHIDTOOL_C_BUILD_EXAMPLES=ON
```

---

## Integration

### CMake — add\_subdirectory

```cmake
add_subdirectory(hidtool_c)
target_link_libraries(your_target PRIVATE hidtool_c::hidtool_c)
```

### CMake — find\_package (after install)

```bash
cmake --install build --prefix /your/install/path
```

```cmake
find_package(hidtool_c REQUIRED)
target_link_libraries(your_target PRIVATE hidtool_c::hidtool_c)
```

### Manual Linking

Include `include/hidtool_c/hidtool_c.h` and link against `libhidtool_c.a` (or `.lib` / `.so` / `.dll`).

---

## API Overview

Include the single public header:

```c
#include <hidtool_c/hidtool_c.h>
```

The preprocessor macros `HIDTOOL_C_HAS_KEYBOARD` and `HIDTOOL_C_HAS_MOUSE` (defined in the generated `hidtool_c_config.h`) guard the keyboard and mouse API sections respectively. Check them at compile time when building optional code paths.

---

### Types and Enumerations

#### `HidtHidType`

```c
typedef enum HidtHidType {
    HIDT_HIDTYPE_KEYBOARD = 0,
    HIDT_HIDTYPE_MOUSE    = 1
} HidtHidType;
```

#### `HidtMouseButton`

```c
typedef enum HidtMouseButton {
    HIDT_MSBTN_NONE    = 0,
    HIDT_MSBTN_LEFT    = 1,
    HIDT_MSBTN_RIGHT   = 2,
    HIDT_MSBTN_MIDDLE  = 3,
    HIDT_MSBTN_BACK    = 4,
    HIDT_MSBTN_FORWARD = 5
} HidtMouseButton;
```

#### Position Types

```c
typedef struct HidtAbsolutePos { int32_t x;  int32_t y;  } HidtAbsolutePos;
typedef struct HidtRelativePos { int32_t dx; int32_t dy; } HidtRelativePos;
typedef struct HidtAbsolutePosRange {
    int32_t min_x; int32_t max_x;
    int32_t min_y; int32_t max_y;
} HidtAbsolutePosRange;
```

#### `HidtKeyboardKey`

A cross-platform enumeration that maps logical key names to portable values. Numeric keys map to their ASCII code (`0x30`–`0x39`), letter keys to their ASCII code (`0x41`–`0x5A`), and all other keys use values starting from `0x8000`. Common values:

| Constant | Value |
|----------|-------|
| `HIDT_KBDKEY_NONE` | `0x0000` |
| `HIDT_KBDKEY_A`–`HIDT_KBDKEY_Z` | `0x0041`–`0x005A` |
| `HIDT_KBDKEY_0`–`HIDT_KBDKEY_9` | `0x0030`–`0x0039` |
| `HIDT_KBDKEY_ENTER` | `0x8002` |
| `HIDT_KBDKEY_ESCAPE` | `0x8039` |
| `HIDT_KBDKEY_SPACE` | `0x8001` |
| `HIDT_KBDKEY_SHIFT` / `_LEFT` / `_RIGHT` | `0x8060`–`0x8062` |
| `HIDT_KBDKEY_CTRL` / `_LEFT` / `_RIGHT` | `0x805A`–`0x805C` |
| `HIDT_KBDKEY_ALT` / `_LEFT` / `_RIGHT` | `0x805D`–`0x805F` |
| `HIDT_KBDKEY_F1`–`HIDT_KBDKEY_F24` | `0x800F`–`0x8026` |

Common aliases: `HIDT_KBDKEY_RETURN`, `HIDT_KBDKEY_ESC`, `HIDT_KBDKEY_FN`, `HIDT_KBDKEY_OPTION`.

#### `HidtKeyboardEventType`

```c
typedef enum HidtKeyboardEventType {
    HIDT_KBD_ET_NONE    = 0,
    HIDT_KBD_ET_PRESS   = 1,
    HIDT_KBD_ET_RELEASE = 2,
    HIDT_KBD_ET_SLEEP   = 3
} HidtKeyboardEventType;
```

#### `HidtKeyboardEvent`

```c
typedef struct HidtKeyboardEvent {
    HidtKeyboardEventType type;
    union {
        int32_t  native_key; // ET_PRESS / ET_RELEASE: platform native key code
        uint64_t sleep_ms;   // ET_SLEEP: sleep duration in milliseconds
    } u;
    uint64_t timestamp;      // nanoseconds since epoch
} HidtKeyboardEvent;
```

#### `HidtMouseEventType`

```c
typedef enum HidtMouseEventType {
    HIDT_MS_ET_NONE     = 0,
    HIDT_MS_ET_ABS_MOVE = 1,  // absolute cursor position
    HIDT_MS_ET_REL_MOVE = 2,  // relative cursor delta
    HIDT_MS_ET_WHEEL    = 3,  // scroll wheel
    HIDT_MS_ET_DRAG     = 4,  // drag
    HIDT_MS_ET_PRESS    = 5,  // button press
    HIDT_MS_ET_RELEASE  = 6,  // button release
    HIDT_MS_ET_SLEEP    = 7   // sleep (simulation only)
} HidtMouseEventType;
```

#### `HidtMouseEvent`

```c
typedef struct HidtMouseEvent {
    HidtMouseEventType type;
    union {
        HidtAbsolutePos abs_pos;      // ET_ABS_MOVE
        HidtRelativePos rel_pos;      // ET_REL_MOVE
        int32_t         wheel_delta;  // ET_WHEEL (unit: 120 per detent)
        struct {
            HidtAbsolutePos pos;
            HidtMouseButton button;
        } drag;                       // ET_DRAG
        HidtMouseButton button;       // ET_PRESS / ET_RELEASE
        uint64_t        sleep_ms;     // ET_SLEEP
    } u;
    uint64_t timestamp;               // nanoseconds since epoch
} HidtMouseEvent;
```

#### `HidtHidEvent`

Unified event that wraps either a keyboard event, a mouse event, or a sleep marker.

```c
typedef enum HidtHidEventType {
    HIDT_HID_ET_NONE     = 0,
    HIDT_HID_ET_KEYBOARD = 1,
    HIDT_HID_ET_MOUSE    = 2,
    HIDT_HID_ET_SLEEP    = 3
} HidtHidEventType;

typedef struct HidtHidEvent {
    HidtHidEventType type;
    union {
        HidtKeyboardEvent keyboard_event; // HID_ET_KEYBOARD
        HidtMouseEvent    mouse_event;    // HID_ET_MOUSE
        uint64_t          sleep_ms;       // HID_ET_SLEEP
    } u;
} HidtHidEvent;
```

#### Callback Types

```c
// Return non-zero to propagate; zero to block (platform support varies).
typedef int (*HidtKeyboardEventHandler)(const HidtKeyboardEvent* event, void* user_data);
typedef int (*HidtMouseEventHandler)  (const HidtMouseEvent*    event, void* user_data);
typedef int (*HidtHidEventHandler)    (const HidtHidEvent*      event, void* user_data);
```

---

### General HID Utilities

```c
// Check if a HID module is available in the current build.
int hidt_is_hid_type_supported(HidtHidType hid_type);
```

---

### Keyboard — Key Utilities

> Available only when `HIDTOOL_C_HAS_KEYBOARD` is defined.

```c
// Convert a HidtKeyboardKey to the platform-native key code (-1 if no mapping).
int32_t       hidt_keyboard_key_to_native_key  (HidtKeyboardKey key);

// Convert a platform-native key code to HidtKeyboardKey (HIDT_KBDKEY_NONE if no mapping).
HidtKeyboardKey hidt_keyboard_key_from_native_key(int32_t native_key);
```

---

### Keyboard — Hooker

> Available only when `HIDTOOL_C_HAS_KEYBOARD` is defined.

The keyboard hooker is a global singleton. It runs its own internal thread once started.

```c
// Query whether a native key is currently held down (can be called before run()).
int  hidt_keyboard_hooker_is_key_pressed(int32_t native_key);

// Start / stop the event listener thread.
int  hidt_keyboard_hooker_run(void);         // returns non-zero on success
void hidt_keyboard_hooker_stop(void);
int  hidt_keyboard_hooker_is_running(void);  // returns non-zero if running

// Register a callback (pass NULL to unregister).
int  hidt_keyboard_hooker_set_event_handler(HidtKeyboardEventHandler handler, void* user_data);

// Update user_data without changing the callback.
int  hidt_keyboard_hooker_set_user_data(void* user_data);
```

> **Thread safety**: Do NOT call `hidt_keyboard_hooker_stop()` from inside the event callback. Signal the main thread (e.g., with an atomic flag) and call stop from there.

---

### Keyboard — Simulator

> Available only when `HIDTOOL_C_HAS_KEYBOARD` is defined.

The keyboard simulator must be initialized before use and destroyed when done.

```c
int  hidt_keyboard_simulator_initialize(void);   // returns non-zero on success
void hidt_keyboard_simulator_destroy(void);
int  hidt_keyboard_simulator_is_initialized(void);

// Send raw event(s).
int    hidt_keyboard_simulator_send_event (const HidtKeyboardEvent* event);
size_t hidt_keyboard_simulator_send_events(const HidtKeyboardEvent* events, size_t count);

// Press / release by native key code.
int hidt_keyboard_simulator_press_key_native  (int32_t native_key);
int hidt_keyboard_simulator_release_key_native(int32_t native_key);

// Press / release by HidtKeyboardKey.
int hidt_keyboard_simulator_press_key  (HidtKeyboardKey key);
int hidt_keyboard_simulator_release_key(HidtKeyboardKey key);

// Click (press + optional delay + release) by native key code.
// interval: milliseconds between press and release.
int hidt_keyboard_simulator_click_key_native(int32_t native_key, uint64_t interval);

// Click by HidtKeyboardKey.
int hidt_keyboard_simulator_click_key(HidtKeyboardKey key, uint64_t interval);
```

---

### Mouse — Hooker

> Available only when `HIDTOOL_C_HAS_MOUSE` is defined.

```c
// Query whether a mouse button is held down.
int            hidt_mouse_hooker_is_button_pressed(HidtMouseButton button);

// Get the current cursor position (always {0,0} on Linux).
HidtAbsolutePos hidt_mouse_hooker_get_cursor_pos(void);

// Start / stop.
int  hidt_mouse_hooker_run(void);
void hidt_mouse_hooker_stop(void);
int  hidt_mouse_hooker_is_running(void);

// Register a callback.
int hidt_mouse_hooker_set_event_handler(HidtMouseEventHandler handler, void* user_data);
int hidt_mouse_hooker_set_user_data(void* user_data);
```

---

### Mouse — Simulator

> Available only when `HIDTOOL_C_HAS_MOUSE` is defined.

```c
int  hidt_mouse_simulator_initialize(void);
void hidt_mouse_simulator_destroy(void);
int  hidt_mouse_simulator_is_initialized(void);

// Send raw event(s).
int    hidt_mouse_simulator_send_event (const HidtMouseEvent* event);
size_t hidt_mouse_simulator_send_events(const HidtMouseEvent* events, size_t count);

// ---- Movement ----
// Move cursor to absolute screen coordinates.
int hidt_mouse_simulator_move_to(int32_t x, int32_t y);

// Move cursor relative to current position.
int hidt_mouse_simulator_move_by(int32_t dx, int32_t dy);

// ---- Scroll Wheel ----
// delta: positive = away from user; negative = toward user. Each detent = 120 units.
int hidt_mouse_simulator_wheel(int32_t delta);

// ---- Buttons ----
int hidt_mouse_simulator_press_button  (HidtMouseButton button);
int hidt_mouse_simulator_release_button(HidtMouseButton button);

// Click (press + optional delay + release).
// interval: milliseconds between press and release.
int hidt_mouse_simulator_click_button(HidtMouseButton button, uint64_t interval);

// Double-click. interval = delay between clicks; press_interval = press/release delay.
int hidt_mouse_simulator_double_click_button(
    HidtMouseButton button, uint64_t interval, uint64_t press_interval);

// ---- Drag ----
// Drag from (from_x, from_y) to (to_x, to_y) holding button.
// interval: ms between move steps; press_interval: ms for press/release.
int hidt_mouse_simulator_drag_combo_from(
    int32_t from_x, int32_t from_y,
    int32_t to_x,   int32_t to_y,
    HidtMouseButton button,
    uint64_t interval, uint64_t press_interval);
```

---

### Unified HID Hooker

Listens to both keyboard and mouse events through a single callback. Internally dispatches to the per-device hookers.

```c
int  hidt_hid_hooker_run(void);
void hidt_hid_hooker_stop(void);
int  hidt_hid_hooker_is_running(void);

int hidt_hid_hooker_set_event_handler(HidtHidEventHandler handler, void* user_data);
int hidt_hid_hooker_set_user_data(void* user_data);
```

---

## Usage Examples

### Keyboard Monitor

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

static int on_keyboard_event(const HidtKeyboardEvent* event, void* user_data)
{
    (void)user_data;
    if (event->type == HIDT_KBD_ET_PRESS) {
        HidtKeyboardKey key = hidt_keyboard_key_from_native_key(event->u.native_key);
        printf("[PRESS]   native=0x%08X  key=0x%04X\n",
               (unsigned)event->u.native_key, (unsigned)key);
    } else if (event->type == HIDT_KBD_ET_RELEASE) {
        HidtKeyboardKey key = hidt_keyboard_key_from_native_key(event->u.native_key);
        printf("[RELEASE] native=0x%08X  key=0x%04X\n",
               (unsigned)event->u.native_key, (unsigned)key);
    }
    return 1; // propagate
}

int main(void)
{
    hidt_keyboard_hooker_set_event_handler(on_keyboard_event, NULL);

    if (!hidt_keyboard_hooker_run()) {
        fprintf(stderr, "Failed to start keyboard hooker.\n");
        return 1;
    }

    printf("Press ENTER to stop...\n");
    getchar();

    hidt_keyboard_hooker_stop();
    return 0;
}
```

---

### Keyboard Simulator

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

int main(void)
{
    if (!hidt_keyboard_simulator_initialize()) {
        fprintf(stderr, "Failed to initialize keyboard simulator.\n");
        return 1;
    }

    printf("Focus a text editor, then press ENTER...\n");
    getchar();

    // Type 'A' (uppercase)
    hidt_keyboard_simulator_press_key(HIDT_KBDKEY_SHIFT_LEFT);
    hidt_keyboard_simulator_click_key(HIDT_KBDKEY_A, 0);
    hidt_keyboard_simulator_release_key(HIDT_KBDKEY_SHIFT_LEFT);

    // Send Ctrl+A
    hidt_keyboard_simulator_press_key(HIDT_KBDKEY_CTRL_LEFT);
    hidt_keyboard_simulator_click_key(HIDT_KBDKEY_A, 0);
    hidt_keyboard_simulator_release_key(HIDT_KBDKEY_CTRL_LEFT);

    hidt_keyboard_simulator_destroy();
    return 0;
}
```

---

### Mouse Monitor

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

static int on_mouse_event(const HidtMouseEvent* event, void* user_data)
{
    (void)user_data;
    switch (event->type) {
        case HIDT_MS_ET_ABS_MOVE:
            printf("[ABS_MOVE] x=%d y=%d\n",
                   event->u.abs_pos.x, event->u.abs_pos.y);
            break;
        case HIDT_MS_ET_PRESS:
            printf("[PRESS]    button=%d\n", (int)event->u.button);
            break;
        case HIDT_MS_ET_WHEEL:
            printf("[WHEEL]    delta=%d\n", event->u.wheel_delta);
            break;
        default:
            break;
    }
    return 1;
}

int main(void)
{
    hidt_mouse_hooker_set_event_handler(on_mouse_event, NULL);
    if (!hidt_mouse_hooker_run()) {
        fprintf(stderr, "Failed to start mouse hooker.\n");
        return 1;
    }

    printf("Press ENTER to stop...\n");
    getchar();

    hidt_mouse_hooker_stop();
    return 0;
}
```

---

### Mouse Simulator

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

int main(void)
{
    if (!hidt_mouse_simulator_initialize()) {
        fprintf(stderr, "Failed to initialize mouse simulator.\n");
        return 1;
    }

    // Move to (800, 600) and left-click
    hidt_mouse_simulator_move_to(800, 600);
    hidt_mouse_simulator_click_button(HIDT_MSBTN_LEFT, 50);

    // Scroll down 3 detents
    hidt_mouse_simulator_wheel(-3 * 120);

    // Drag from (400, 400) to (600, 400)
    hidt_mouse_simulator_drag_combo_from(400, 400, 600, 400, HIDT_MSBTN_LEFT, 0, 0);

    hidt_mouse_simulator_destroy();
    return 0;
}
```

---

## Notes and Caveats

- **Blocking events**: Returning `0` from a callback blocks the event from reaching other applications. This feature is not supported on all platforms — check the upstream [hidtool documentation](https://github.com/JaderoChan/hidtool) for details.
- **Stopping from a callback**: Never call `hidt_*_hooker_stop()` from inside the event callback. The callback runs on the hooker's internal thread; calling stop from there will deadlock (`EDEADLK`). Use an atomic flag and call stop from the main thread.
- **macOS simulation**: Simulation functions always return success even if the action has no effect. Grant Accessibility permissions to the hosting process.
- **Linux cursor position**: `hidt_mouse_hooker_get_cursor_pos()` always returns `{0, 0}` on Linux.
- **Wheel units**: One scroll detent = 120 units. Positive delta scrolls away from the user; negative scrolls toward the user.
