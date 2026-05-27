# hidtool_c — hidtool 的 C 语言绑定 *

[**简体中文** | [**English**](README_EN.md)]

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/version-2.4.1-blue.svg)](https://github.com/JaderoChan/hidtool_c)
[![Language: C](https://img.shields.io/badge/language-C99-lightgrey.svg)](https://en.wikipedia.org/wiki/C99)
[![Upstream](https://img.shields.io/badge/upstream-hidtool-orange.svg)](https://github.com/JaderoChan/hidtool)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-informational.svg)](https://github.com/JaderoChan/hidtool_c)

[hidtool](https://github.com/JaderoChan/hidtool) C++ 库的纯 C 绑定，提供完整的 C99 API，用于全局键盘 / 鼠标事件监听与输入模拟。

---

## 目录

- [hidtool\_c — hidtool 的 C 语言绑定 \*](#hidtool_c--hidtool-的-c-语言绑定-)
  - [目录](#目录)
  - [特性](#特性)
  - [平台支持](#平台支持)
  - [环境要求](#环境要求)
  - [构建](#构建)
    - [CMake 选项](#cmake-选项)
  - [集成](#集成)
    - [CMake — add\_subdirectory](#cmake--add_subdirectory)
    - [CMake — find\_package（安装后使用）](#cmake--find_package安装后使用)
    - [手动链接](#手动链接)
  - [API 概览](#api-概览)
    - [类型与枚举](#类型与枚举)
      - [`HidtHidType`](#hidthidtype)
      - [`HidtMouseButton`](#hidtmousebutton)
      - [坐标类型](#坐标类型)
      - [`HidtKeyboardKey`](#hidtkeyboardkey)
      - [`HidtKeyboardEventType`](#hidtkeyboardeventtype)
      - [`HidtKeyboardEvent`](#hidtkeyboardevent)
      - [`HidtMouseEventType`](#hidtmouseeventtype)
      - [`HidtMouseEvent`](#hidtmouseevent)
      - [`HidtHidEvent`](#hidthidevent)
      - [回调类型](#回调类型)
    - [通用 HID 工具函数](#通用-hid-工具函数)
    - [键盘 — 按键工具函数](#键盘--按键工具函数)
    - [键盘 — 钩子（监听）](#键盘--钩子监听)
    - [键盘 — 模拟器](#键盘--模拟器)
    - [鼠标 — 钩子（监听）](#鼠标--钩子监听)
    - [鼠标 — 模拟器](#鼠标--模拟器)
    - [统一 HID 钩子](#统一-hid-钩子)
  - [使用示例](#使用示例)
    - [键盘监听](#键盘监听)
    - [键盘模拟](#键盘模拟)
    - [鼠标监听](#鼠标监听)
    - [鼠标模拟](#鼠标模拟)
  - [注意事项](#注意事项)

---

## 特性

- **纯 C99 API** — 只需包含单个头文件并链接库，您的项目无需 C++ 编译器。
- **键盘模块** — 全局键盘事件监听与键盘输入模拟。
- **鼠标模块** — 全局鼠标事件监听，支持绝对 / 相对移动、点击、滚轮、拖拽。
- **统一 HID 钩子** — 通过单个回调同时接收键盘和鼠标事件。
- 跨平台：Windows、macOS、Linux。
- 支持静态库 / 动态库构建。
- 线程安全的单例钩子与模拟器。

---

## 平台支持

| 平台 | 状态 | 备注 |
|------|------|------|
| Windows  | ✅ | — |
| macOS    | ✅ | 事件监听与模拟功能需要**辅助功能**权限 |
| Linux    | ✅ | 需要**管理员 / root** 权限（访问 `/dev/input` 和 `/dev/uinput`） |

> **macOS**：由于 macOS API 设计，模拟函数无法确认操作是否生效。请为宿主程序授予**辅助功能**权限。
>
> **Linux**：`hidt_mouse_hooker_get_cursor_pos()` 在 Linux 上始终返回 `{0, 0}`，因为原始输入 API 不提供绝对光标位置。

---

## 环境要求

- CMake ≥ 3.26
- 您的项目需要 **C99** 编译器
- 构建 hidtool\_c 本身需要 **C++11** 编译器（绑定层以 C++ 编译）
- **macOS**：CoreFoundation、Carbon、CoreGraphics（CMake 自动查找）
- **Linux**：pthreads

---

## 构建

```bash
git clone --recurse-submodules https://github.com/JaderoChan/hidtool_c.git
cd hidtool_c
cmake -B build [选项]
cmake --build build
```

> 若已克隆但未拉取子模块，请执行 `git submodule update --init --recursive`。

### CMake 选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `HIDTOOL_C_BUILD_SHARED`      | `OFF` | 构建为动态库 |
| `HIDTOOL_C_BUILD_EXAMPLES`    | `OFF` | 构建示例程序 |
| `HIDTOOL_BUILD_WITH_KEYBOARD` | `ON`  | 包含键盘模块（透传自 hidtool） |
| `HIDTOOL_BUILD_WITH_MOUSE`    | `ON`  | 包含鼠标模块（透传自 hidtool） |

```bash
# 默认静态库，Release 构建
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 动态库
cmake -B build -DCMAKE_BUILD_TYPE=Release -DHIDTOOL_C_BUILD_SHARED=ON

# 仅键盘模块
cmake -B build -DHIDTOOL_BUILD_WITH_MOUSE=OFF

# 带示例
cmake -B build -DHIDTOOL_C_BUILD_EXAMPLES=ON
```

---

## 集成

### CMake — add\_subdirectory

```cmake
add_subdirectory(hidtool_c)
target_link_libraries(your_target PRIVATE hidtool_c::hidtool_c)
```

### CMake — find\_package（安装后使用）

```bash
cmake --install build --prefix /your/install/path
```

```cmake
find_package(hidtool_c REQUIRED)
target_link_libraries(your_target PRIVATE hidtool_c::hidtool_c)
```

### 手动链接

包含 `include/hidtool_c/hidtool_c.h` 并链接 `libhidtool_c.a`（或 `.lib` / `.so` / `.dll`）。

---

## API 概览

包含单一公共头文件：

```c
#include <hidtool_c/hidtool_c.h>
```

预处理器宏 `HIDTOOL_C_HAS_KEYBOARD` 和 `HIDTOOL_C_HAS_MOUSE`（由生成的 `hidtool_c_config.h` 定义）分别用于条件编译键盘和鼠标 API 段。在编写可选代码路径时请检查这些宏。

---

### 类型与枚举

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

#### 坐标类型

```c
typedef struct HidtAbsolutePos { int32_t x;  int32_t y;  } HidtAbsolutePos;
typedef struct HidtRelativePos { int32_t dx; int32_t dy; } HidtRelativePos;
typedef struct HidtAbsolutePosRange {
    int32_t min_x; int32_t max_x;
    int32_t min_y; int32_t max_y;
} HidtAbsolutePosRange;
```

#### `HidtKeyboardKey`

跨平台的键值枚举，将逻辑按键名映射为可移植值。数字键映射到对应 ASCII 码（`0x30`–`0x39`），字母键映射到对应 ASCII 码（`0x41`–`0x5A`），其余按键使用从 `0x8000` 开始的值。常用值：

| 常量 | 值 |
|------|----|
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

常用别名：`HIDT_KBDKEY_RETURN`、`HIDT_KBDKEY_ESC`、`HIDT_KBDKEY_FN`、`HIDT_KBDKEY_OPTION`。

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
        int32_t  native_key; // ET_PRESS / ET_RELEASE：平台原生键值
        uint64_t sleep_ms;   // ET_SLEEP：休眠时长（毫秒）
    } u;
    uint64_t timestamp;      // 事件时间戳（纳秒）
} HidtKeyboardEvent;
```

#### `HidtMouseEventType`

```c
typedef enum HidtMouseEventType {
    HIDT_MS_ET_NONE     = 0,
    HIDT_MS_ET_ABS_MOVE = 1,  // 光标绝对位置
    HIDT_MS_ET_REL_MOVE = 2,  // 光标相对位移
    HIDT_MS_ET_WHEEL    = 3,  // 滚轮
    HIDT_MS_ET_DRAG     = 4,  // 拖拽
    HIDT_MS_ET_PRESS    = 5,  // 按钮按下
    HIDT_MS_ET_RELEASE  = 6,  // 按钮释放
    HIDT_MS_ET_SLEEP    = 7   // 休眠（仅模拟）
} HidtMouseEventType;
```

#### `HidtMouseEvent`

```c
typedef struct HidtMouseEvent {
    HidtMouseEventType type;
    union {
        HidtAbsolutePos abs_pos;      // ET_ABS_MOVE
        HidtRelativePos rel_pos;      // ET_REL_MOVE
        int32_t         wheel_delta;  // ET_WHEEL（单位：每格 120）
        struct {
            HidtAbsolutePos pos;
            HidtMouseButton button;
        } drag;                       // ET_DRAG
        HidtMouseButton button;       // ET_PRESS / ET_RELEASE
        uint64_t        sleep_ms;     // ET_SLEEP
    } u;
    uint64_t timestamp;               // 事件时间戳（纳秒）
} HidtMouseEvent;
```

#### `HidtHidEvent`

统一事件结构，包装键盘事件、鼠标事件或休眠标记。

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

#### 回调类型

```c
// 返回非零值继续传播事件；返回零阻断事件（平台支持情况不一）。
typedef int (*HidtKeyboardEventHandler)(const HidtKeyboardEvent* event, void* user_data);
typedef int (*HidtMouseEventHandler)  (const HidtMouseEvent*    event, void* user_data);
typedef int (*HidtHidEventHandler)    (const HidtHidEvent*      event, void* user_data);
```

---

### 通用 HID 工具函数

```c
// 检查当前构建是否包含指定的 HID 模块。
int hidt_is_hid_type_supported(HidtHidType hid_type);
```

---

### 键盘 — 按键工具函数

> 仅在定义了 `HIDTOOL_C_HAS_KEYBOARD` 时可用。

```c
// 将 HidtKeyboardKey 转换为平台原生键值（无映射时返回 -1）。
int32_t       hidt_keyboard_key_to_native_key  (HidtKeyboardKey key);

// 将平台原生键值转换为 HidtKeyboardKey（无映射时返回 HIDT_KBDKEY_NONE）。
HidtKeyboardKey hidt_keyboard_key_from_native_key(int32_t native_key);
```

---

### 键盘 — 钩子（监听）

> 仅在定义了 `HIDTOOL_C_HAS_KEYBOARD` 时可用。

键盘钩子是全局单例，启动后在自己的内部线程中运行。

```c
// 查询某原生键是否当前被按住（可在 run() 之前调用）。
int  hidt_keyboard_hooker_is_key_pressed(int32_t native_key);

// 启动 / 停止事件监听线程。
int  hidt_keyboard_hooker_run(void);         // 成功返回非零值
void hidt_keyboard_hooker_stop(void);
int  hidt_keyboard_hooker_is_running(void);  // 运行中返回非零值

// 注册回调（传 NULL 取消注册）。
int  hidt_keyboard_hooker_set_event_handler(HidtKeyboardEventHandler handler, void* user_data);

// 仅更新 user_data，不改变回调。
int  hidt_keyboard_hooker_set_user_data(void* user_data);
```

> **线程安全**：切勿在事件回调内部调用 `hidt_keyboard_hooker_stop()`。回调在钩子的内部线程上运行，在该线程中调用 stop() 会导致死锁（`EDEADLK`）。请使用原子标志并在主线程中调用 stop()。

---

### 键盘 — 模拟器

> 仅在定义了 `HIDTOOL_C_HAS_KEYBOARD` 时可用。

键盘模拟器使用前必须初始化，使用完毕后需要销毁。

```c
int  hidt_keyboard_simulator_initialize(void);   // 成功返回非零值
void hidt_keyboard_simulator_destroy(void);
int  hidt_keyboard_simulator_is_initialized(void);

// 发送原始事件。
int    hidt_keyboard_simulator_send_event (const HidtKeyboardEvent* event);
size_t hidt_keyboard_simulator_send_events(const HidtKeyboardEvent* events, size_t count);

// 按原生键值按下 / 释放。
int hidt_keyboard_simulator_press_key_native  (int32_t native_key);
int hidt_keyboard_simulator_release_key_native(int32_t native_key);

// 按 HidtKeyboardKey 按下 / 释放。
int hidt_keyboard_simulator_press_key  (HidtKeyboardKey key);
int hidt_keyboard_simulator_release_key(HidtKeyboardKey key);

// 点击（按下 + 可选延迟 + 释放），按原生键值。
// interval：按下与释放之间的延迟（毫秒）。
int hidt_keyboard_simulator_click_key_native(int32_t native_key, uint64_t interval);

// 按 HidtKeyboardKey 点击。
int hidt_keyboard_simulator_click_key(HidtKeyboardKey key, uint64_t interval);
```

---

### 鼠标 — 钩子（监听）

> 仅在定义了 `HIDTOOL_C_HAS_MOUSE` 时可用。

```c
// 查询某鼠标按键是否当前被按住。
int            hidt_mouse_hooker_is_button_pressed(HidtMouseButton button);

// 获取当前光标位置（Linux 上始终返回 {0,0}）。
HidtAbsolutePos hidt_mouse_hooker_get_cursor_pos(void);

// 启动 / 停止。
int  hidt_mouse_hooker_run(void);
void hidt_mouse_hooker_stop(void);
int  hidt_mouse_hooker_is_running(void);

// 注册回调。
int hidt_mouse_hooker_set_event_handler(HidtMouseEventHandler handler, void* user_data);
int hidt_mouse_hooker_set_user_data(void* user_data);
```

---

### 鼠标 — 模拟器

> 仅在定义了 `HIDTOOL_C_HAS_MOUSE` 时可用。

```c
int  hidt_mouse_simulator_initialize(void);
void hidt_mouse_simulator_destroy(void);
int  hidt_mouse_simulator_is_initialized(void);

// 发送原始事件。
int    hidt_mouse_simulator_send_event (const HidtMouseEvent* event);
size_t hidt_mouse_simulator_send_events(const HidtMouseEvent* events, size_t count);

// ---- 移动 ----
// 移动光标到屏幕绝对坐标。
int hidt_mouse_simulator_move_to(int32_t x, int32_t y);

// 相对当前位置移动光标。
int hidt_mouse_simulator_move_by(int32_t dx, int32_t dy);

// ---- 滚轮 ----
// delta：正值向远离用户方向滚动；负值向用户方向滚动。每格 = 120 单位。
int hidt_mouse_simulator_wheel(int32_t delta);

// ---- 按钮 ----
int hidt_mouse_simulator_press_button  (HidtMouseButton button);
int hidt_mouse_simulator_release_button(HidtMouseButton button);

// 点击（按下 + 可选延迟 + 释放）。
// interval：按下与释放之间的延迟（毫秒）。
int hidt_mouse_simulator_click_button(HidtMouseButton button, uint64_t interval);

// 双击。interval = 两次点击间隔；press_interval = 按下/释放延迟。
int hidt_mouse_simulator_double_click_button(
    HidtMouseButton button, uint64_t interval, uint64_t press_interval);

// ---- 拖拽 ----
// 从 (from_x, from_y) 拖拽到 (to_x, to_y)，按住指定按钮。
// interval：移动步骤间延迟（毫秒）；press_interval：按下/释放延迟（毫秒）。
int hidt_mouse_simulator_drag_combo_from(
    int32_t from_x, int32_t from_y,
    int32_t to_x,   int32_t to_y,
    HidtMouseButton button,
    uint64_t interval, uint64_t press_interval);
```

---

### 统一 HID 钩子

通过单个回调同时监听键盘和鼠标事件，内部分发到各设备钩子。

```c
int  hidt_hid_hooker_run(void);
void hidt_hid_hooker_stop(void);
int  hidt_hid_hooker_is_running(void);

int hidt_hid_hooker_set_event_handler(HidtHidEventHandler handler, void* user_data);
int hidt_hid_hooker_set_user_data(void* user_data);
```

---

## 使用示例

### 键盘监听

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

static int on_keyboard_event(const HidtKeyboardEvent* event, void* user_data)
{
    (void)user_data;
    if (event->type == HIDT_KBD_ET_PRESS) {
        HidtKeyboardKey key = hidt_keyboard_key_from_native_key(event->u.native_key);
        printf("[按下]   原生键=0x%08X  逻辑键=0x%04X\n",
               (unsigned)event->u.native_key, (unsigned)key);
    } else if (event->type == HIDT_KBD_ET_RELEASE) {
        HidtKeyboardKey key = hidt_keyboard_key_from_native_key(event->u.native_key);
        printf("[释放]   原生键=0x%08X  逻辑键=0x%04X\n",
               (unsigned)event->u.native_key, (unsigned)key);
    }
    return 1; // 继续传播
}

int main(void)
{
    hidt_keyboard_hooker_set_event_handler(on_keyboard_event, NULL);

    if (!hidt_keyboard_hooker_run()) {
        fprintf(stderr, "启动键盘钩子失败。\n");
        return 1;
    }

    printf("按 ENTER 停止...\n");
    getchar();

    hidt_keyboard_hooker_stop();
    return 0;
}
```

---

### 键盘模拟

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

int main(void)
{
    if (!hidt_keyboard_simulator_initialize()) {
        fprintf(stderr, "初始化键盘模拟器失败。\n");
        return 1;
    }

    printf("请切换到文本编辑器，然后按 ENTER...\n");
    getchar();

    // 输入大写字母 'A'
    hidt_keyboard_simulator_press_key(HIDT_KBDKEY_SHIFT_LEFT);
    hidt_keyboard_simulator_click_key(HIDT_KBDKEY_A, 0);
    hidt_keyboard_simulator_release_key(HIDT_KBDKEY_SHIFT_LEFT);

    // 发送 Ctrl+A（全选）
    hidt_keyboard_simulator_press_key(HIDT_KBDKEY_CTRL_LEFT);
    hidt_keyboard_simulator_click_key(HIDT_KBDKEY_A, 0);
    hidt_keyboard_simulator_release_key(HIDT_KBDKEY_CTRL_LEFT);

    hidt_keyboard_simulator_destroy();
    return 0;
}
```

---

### 鼠标监听

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

static int on_mouse_event(const HidtMouseEvent* event, void* user_data)
{
    (void)user_data;
    switch (event->type) {
        case HIDT_MS_ET_ABS_MOVE:
            printf("[绝对移动] x=%d y=%d\n",
                   event->u.abs_pos.x, event->u.abs_pos.y);
            break;
        case HIDT_MS_ET_PRESS:
            printf("[按下]    按键=%d\n", (int)event->u.button);
            break;
        case HIDT_MS_ET_WHEEL:
            printf("[滚轮]    delta=%d\n", event->u.wheel_delta);
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
        fprintf(stderr, "启动鼠标钩子失败。\n");
        return 1;
    }

    printf("按 ENTER 停止...\n");
    getchar();

    hidt_mouse_hooker_stop();
    return 0;
}
```

---

### 鼠标模拟

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

int main(void)
{
    if (!hidt_mouse_simulator_initialize()) {
        fprintf(stderr, "初始化鼠标模拟器失败。\n");
        return 1;
    }

    // 移动到 (800, 600) 并左键单击
    hidt_mouse_simulator_move_to(800, 600);
    hidt_mouse_simulator_click_button(HIDT_MSBTN_LEFT, 50);

    // 向下滚动 3 格
    hidt_mouse_simulator_wheel(-3 * 120);

    // 从 (400, 400) 拖拽到 (600, 400)
    hidt_mouse_simulator_drag_combo_from(400, 400, 600, 400, HIDT_MSBTN_LEFT, 0, 0);

    hidt_mouse_simulator_destroy();
    return 0;
}
```

---

## 注意事项

- **阻断事件传播**：从回调中返回 `0` 可阻止事件传递给其他程序。此功能并非所有平台均支持——详情请参阅上游 [hidtool 文档](https://github.com/JaderoChan/hidtool)。
- **回调中不可调用 stop**：切勿在事件回调内部调用 `hidt_*_hooker_stop()`。回调运行在钩子的内部线程中，在此线程调用 stop() 会导致自身 join（`EDEADLK` / 死锁）。请使用原子标志，并在主线程中调用 stop()。
- **macOS 模拟**：模拟函数始终返回成功，即使操作未实际生效。请为宿主进程授予辅助功能权限。
- **Linux 光标位置**：`hidt_mouse_hooker_get_cursor_pos()` 在 Linux 上始终返回 `{0, 0}`。
- **滚轮单位**：一格滚动 = 120 单位。正值表示向远离用户方向滚动；负值表示向用户方向滚动。
