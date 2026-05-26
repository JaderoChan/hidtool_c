# HID Tool C

[[**简体中文**](doc/README_ZH.md) | **English**]

C language bindings for the [hidtool](https://github.com/JaderoChan/hidtool) C++ library — cross-platform keyboard and mouse input simulation and event listening, wrapped in a pure C API.

## Features

- **Keyboard Module**: Global keyboard event listening, keyboard input simulation
- **Mouse Module**: Global mouse event listening, mouse input simulation (move, click, wheel, drag)
- Pure C API (`hidtool_c.h`) — usable from any C11-compatible project
- Cross-platform: Windows, macOS, Linux
- Supports static / shared library builds
- Thread-safe callback registration

## Quick Start

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

int on_keyboard_event(const HidtcKeyboardEvent* event, void* userData)
{
    if (event->type == HIDTC_KBD_ET_PRESS)
        printf("Key pressed: native key = %d\n", event->nativeKey);
    return 1; /* propagate */
}

int main(void)
{
    hidtc_keyboard_hooker_set_event_handler(on_keyboard_event, NULL);
    hidtc_keyboard_hooker_run();

    /* ... wait for events ... */

    hidtc_keyboard_hooker_stop();
    return 0;
}
```

## Documentation

- [English Documentation](doc/README_EN.md)
- [中文文档](doc/README_ZH.md)

## License

This project follows the same [MIT License](3rdparty/hidtool/LICENSE) as the underlying hidtool library.
