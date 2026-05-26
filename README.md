# hidtool_c

[[**简体中文**](doc/README_ZH.md) | [**English**](doc/README_EN.md)]

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)](https://github.com/JaderoChan/hidtool_c/releases)
[![Language: C](https://img.shields.io/badge/language-C99-lightgrey.svg)](https://en.wikipedia.org/wiki/C99)
[![C++](https://img.shields.io/badge/build%20requires-C%2B%2B11-blue.svg)](https://en.cppreference.com/w/cpp/11)
[![CMake](https://img.shields.io/badge/CMake-%3E%3D3.26-064F8C.svg)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-informational.svg)](https://github.com/JaderoChan/hidtool_c)
[![Upstream](https://img.shields.io/badge/upstream-hidtool-orange.svg)](https://github.com/JaderoChan/hidtool)

Pure C (C99) bindings for the [hidtool](https://github.com/JaderoChan/hidtool) C++ library — global keyboard / mouse event listening and input simulation, accessible from plain C.

---

## What is this?

**hidtool_c** wraps the [hidtool](https://github.com/JaderoChan/hidtool) C++ library in a thin, zero-overhead C99 layer. The binding is compiled as C++ but exports a pure C API, so your application only needs a C compiler.

| Feature | hidtool (C++) | hidtool_c (C binding) |
|---------|--------------|----------------------|
| Language | C++11 | C99 |
| Keyboard hooker | ✅ | ✅ |
| Keyboard simulator | ✅ | ✅ |
| Mouse hooker | ✅ | ✅ |
| Mouse simulator | ✅ | ✅ |
| Unified HID hooker | ✅ | ✅ |
| Windows / macOS / Linux | ✅ | ✅ |

---

## Quick Start

```bash
git clone --recurse-submodules https://github.com/JaderoChan/hidtool_c.git
cd hidtool_c
cmake -B build -DCMAKE_BUILD_TYPE=Release -DHIDTOOL_C_BUILD_EXAMPLES=ON
cmake --build build
```

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

static int on_key(const HidtKeyboardEvent* e, void* ud)
{
    (void)ud;
    if (e->type == HIDT_KBD_ET_PRESS)
        printf("key pressed: native=0x%X\n", (unsigned)e->u.native_key);
    return 1;
}

int main(void)
{
    hidt_keyboard_hooker_set_event_handler(on_key, NULL);
    hidt_keyboard_hooker_run();
    getchar(); // press ENTER to quit
    hidt_keyboard_hooker_stop();
    return 0;
}
```

---

## Documentation

- [English documentation](doc/README_EN.md)
- [中文文档](doc/README_ZH.md)

---

## Platform Support

| Platform | Status | Notes |
|----------|--------|-------|
| Windows  | ✅ | — |
| macOS    | ✅ | Requires **Accessibility** permissions |
| Linux    | ✅ | Requires **root** privileges |

---

## License

This project is licensed under the [MIT License](LICENSE).
The bundled [hidtool](https://github.com/JaderoChan/hidtool) submodule is also MIT-licensed.
