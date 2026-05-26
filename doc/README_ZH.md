# HID Tool C — 中文文档

[**简体中文** | [**English**](README_EN.md)]

[hidtool](https://github.com/JaderoChan/hidtool) C++ 库的 C 语言绑定。
提供纯 C API，用于跨平台键盘 / 鼠标输入模拟与全局事件监听。

---

## 目录

- [特性](#特性)
- [平台支持](#平台支持)
- [要求](#要求)
- [构建](#构建)
- [集成](#集成)
- [API 参考](#api-参考)
  - [类型](#类型)
  - [HID 类型工具函数](#hid-类型工具函数)
  - [键盘按键工具函数](#键盘按键工具函数)
  - [键盘 Hooker（监听器）](#键盘-hooker监听器)
  - [键盘 Simulator（模拟器）](#键盘-simulator模拟器)
  - [鼠标 Hooker（监听器）](#鼠标-hooker监听器)
  - [鼠标 Simulator（模拟器）](#鼠标-simulator模拟器)
  - [HID Hooker（统一监听器）](#hid-hooker统一监听器)
  - [HID Simulator（统一模拟器）](#hid-simulator统一模拟器)
- [使用示例](#使用示例)

---

## 特性

- **键盘模块**：全局键盘事件监听、键盘输入模拟
- **鼠标模块**：全局鼠标事件监听、鼠标输入模拟（移动、点击、滚轮、拖拽）
- 纯 C 头文件（`hidtool_c.h`）——项目中无需任何 C++ 代码
- 跨平台：Windows、macOS、Linux
- 支持静态库 / 动态库构建
- 通过原子操作实现线程安全的回调注册

---

## 平台支持

| 平台    | 状态 | 备注 |
|---------|------|------|
| Windows | ✅   | —    |
| macOS   | ✅   | 需要辅助功能权限 |
| Linux   | ✅   | 需要管理员权限  |

> **macOS 注意**：由于 macOS API 设计限制，模拟函数无法确认执行是否成功。
> 即使函数返回非零值，操作也可能没有效果，通常需要为应用授予**辅助功能**相关权限。

---

## 要求

- CMake >= 3.26
- C11 编译器（事件结构体使用了匿名 union）
- C++11 编译器（用于构建绑定层）
- **macOS**：CoreFoundation、Carbon、CoreGraphics（CMake 自动查找）
- **Linux**：pthreads

---

## 构建

```bash
git clone --recurse-submodules <本仓库地址>
cd hidtool_c
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### CMake 选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `HIDTOOL_BUILD_WITH_KEYBOARD`  | `ON`  | 构建键盘模块 |
| `HIDTOOL_BUILD_WITH_MOUSE`     | `ON`  | 构建鼠标模块 |
| `HIDTOOL_C_BUILD_SHARED`       | `OFF` | 将 `hidtool_c` 构建为动态库 |
| `HIDTOOL_C_BUILD_EXAMPLES`     | `OFF` | 构建 `example/` 下的示例程序 |

```bash
# 构建动态库
cmake -B build -DCMAKE_BUILD_TYPE=Release -DHIDTOOL_C_BUILD_SHARED=ON

# 构建并包含示例程序
cmake -B build -DCMAKE_BUILD_TYPE=Release -DHIDTOOL_C_BUILD_EXAMPLES=ON

# 仅构建键盘模块
cmake -B build -DHIDTOOL_BUILD_WITH_MOUSE=OFF
```

---

## 集成

### CMake — add_subdirectory

```cmake
add_subdirectory(hidtool_c)
target_link_libraries(your_target PRIVATE hidtool_c)
```

在 C 源文件中引入头文件：

```c
#include <hidtool_c/hidtool_c.h>
```

---

## API 参考

### 类型

#### 位置类型

```c
typedef struct { int32_t x;  int32_t y;  } HidtcAbsolutePos;     /* 绝对坐标 */
typedef struct { int32_t dx; int32_t dy; } HidtcRelativePos;      /* 相对位移 */
typedef struct { int32_t min_x; int32_t max_x;
                 int32_t min_y; int32_t max_y; } HidtcAbsolutePosRange; /* 坐标范围 */
```

#### `HidtcMouseButton` — 鼠标按键

```c
typedef enum {
    HIDTC_MSBTN_NONE    = 0,  /* 无 */
    HIDTC_MSBTN_LEFT    = 1,  /* 左键 */
    HIDTC_MSBTN_RIGHT   = 2,  /* 右键 */
    HIDTC_MSBTN_MIDDLE  = 3,  /* 中键 */
    HIDTC_MSBTN_BACK    = 4,  /* 后退键 */
    HIDTC_MSBTN_FORWARD = 5   /* 前进键 */
} HidtcMouseButton;
```

#### `HidtcKeyboardEvent` — 键盘事件

```c
typedef struct {
    HidtcKeyboardEventType type;   /* NONE / PRESS / RELEASE / SLEEP */
    union {
        int32_t nativeKey;         /* PRESS / RELEASE：平台原生按键值 */
        size_t  sleepMs;           /* SLEEP：休眠毫秒数 */
    };
    uint64_t timestamp;            /* 事件时间戳，纳秒 */
} HidtcKeyboardEvent;
```

#### `HidtcMouseEvent` — 鼠标事件

```c
typedef struct {
    HidtcMouseEventType type;
    union {
        HidtcAbsolutePos absPos;   /* ABS_MOVE：目标绝对坐标 */
        HidtcRelativePos relPos;   /* REL_MOVE：相对位移 */
        int32_t          wheelDelta; /* WHEEL：滚动量，单位 120，正值远离用户 */
        struct {
            HidtcAbsolutePos pos;
            HidtcMouseButton button;
        } drag;                    /* DRAG：拖拽目标坐标与按键 */
        HidtcMouseButton button;   /* PRESS / RELEASE：鼠标按键 */
        size_t           sleepMs;  /* SLEEP：休眠毫秒数 */
    };
    uint64_t timestamp;            /* 事件时间戳，纳秒 */
} HidtcMouseEvent;
```

#### `HidtcHidEvent` — 统一 HID 事件

```c
typedef struct {
    HidtcHidEventType type;        /* NONE / KEYBOARD / MOUSE / SLEEP */
    union {
        HidtcKeyboardEvent keyboardEvent;
        HidtcMouseEvent    mouseEvent;
        size_t             sleepMs;
    };
} HidtcHidEvent;
```

#### 回调函数类型

```c
/* 返回非零值表示传播事件；返回零表示阻断事件传播 */
typedef int (*HidtcKeyboardEventHandler)(const HidtcKeyboardEvent* event, void* userData);
typedef int (*HidtcMouseEventHandler)   (const HidtcMouseEvent*    event, void* userData);
typedef int (*HidtcHidEventHandler)     (const HidtcHidEvent*      event, void* userData);
```

> 并非所有平台都支持阻断事件传播，可通过
> `hidtc_hid_hooker_is_support_block_event_propagation()` 提前检查。

---

### HID 类型工具函数

```c
/* 检查当前环境是否支持指定的 HID 类型子模块，非零表示支持 */
int hidtc_is_hid_type_supported(HidtcHidType hidType);
```

---

### 键盘按键工具函数

```c
/* HidtcKeyboardKey -> 平台原生按键值，无映射时返回 -1 */
int32_t hidtc_keyboard_key_to_native_key(HidtcKeyboardKey key);

/* 平台原生按键值 -> HidtcKeyboardKey，无映射时返回 HIDTC_KBDKEY_NONE */
HidtcKeyboardKey hidtc_keyboard_key_from_native_key(int32_t nativeKey);
```

常用按键常量（`HidtcKeyboardKey`）：

| 常量 | 说明 |
|------|------|
| `HIDTC_KBDKEY_0` – `HIDTC_KBDKEY_9` | 数字行 |
| `HIDTC_KBDKEY_A` – `HIDTC_KBDKEY_Z` | 字母键 |
| `HIDTC_KBDKEY_F1` – `HIDTC_KBDKEY_F24` | 功能键 |
| `HIDTC_KBDKEY_NUMPAD_0` – `HIDTC_KBDKEY_NUMPAD_9` | 小键盘数字 |
| `HIDTC_KBDKEY_ENTER` / `HIDTC_KBDKEY_RETURN` | 回车 |
| `HIDTC_KBDKEY_ESCAPE` / `HIDTC_KBDKEY_ESC` | Escape |
| `HIDTC_KBDKEY_SPACE` | 空格 |
| `HIDTC_KBDKEY_BACKSPACE` | 退格 |
| `HIDTC_KBDKEY_TAB` | Tab |
| `HIDTC_KBDKEY_LEFT/RIGHT/UP/DOWN` | 方向键 |
| `HIDTC_KBDKEY_CTRL` / `_LEFT` / `_RIGHT` | Ctrl |
| `HIDTC_KBDKEY_SHIFT` / `_LEFT` / `_RIGHT` | Shift |
| `HIDTC_KBDKEY_ALT` / `_LEFT` / `_RIGHT` | Alt / Option |
| `HIDTC_KBDKEY_META` / `_LEFT` / `_RIGHT` | Win / Command |
| `HIDTC_KBDKEY_CAPS_LOCK` | Caps Lock |
| `HIDTC_KBDKEY_DELETE` | Delete |
| `HIDTC_KBDKEY_INSERT` | Insert |

---

### 键盘 Hooker（监听器）

```c
/* 检查某个原生按键是否正被按下 */
int  hidtc_keyboard_hooker_is_key_pressed(int32_t nativeKey);

/* 启动 / 停止全局键盘监听 */
int  hidtc_keyboard_hooker_run(void);
void hidtc_keyboard_hooker_stop(void);
int  hidtc_keyboard_hooker_is_running(void);

/* 注册回调，传入 NULL 可注销回调 */
int hidtc_keyboard_hooker_set_event_handler(HidtcKeyboardEventHandler handler,
                                             void* userData);
/* 仅更新用户数据指针，不更改回调 */
int hidtc_keyboard_hooker_set_user_data(void* userData);
```

> **注意**：不能在事件回调函数内部调用 `hidtc_keyboard_hooker_*` 系列函数。

---

### 键盘 Simulator（模拟器）

```c
int    hidtc_keyboard_simulator_initialize(void);
void   hidtc_keyboard_simulator_destroy(void);
int    hidtc_keyboard_simulator_is_initialized(void);

/* 发送单个事件 */
int    hidtc_keyboard_simulator_send_event(const HidtcKeyboardEvent* event);
/* 批量发送事件，返回实际成功发送的数量 */
size_t hidtc_keyboard_simulator_send_events(const HidtcKeyboardEvent* events, size_t count);

/* 便捷函数 */
int hidtc_keyboard_simulator_press_key_native(int32_t nativeKey);  /* 按下（原生键值）*/
int hidtc_keyboard_simulator_press_key(HidtcKeyboardKey key);      /* 按下 */

int hidtc_keyboard_simulator_release_key_native(int32_t nativeKey); /* 抬起（原生键值）*/
int hidtc_keyboard_simulator_release_key(HidtcKeyboardKey key);     /* 抬起 */

/* 点击 = 按下 + 可选延迟 + 抬起 */
int hidtc_keyboard_simulator_click_key_native(int32_t nativeKey, size_t intervalMs);
int hidtc_keyboard_simulator_click_key(HidtcKeyboardKey key,     size_t intervalMs);
```

---

### 鼠标 Hooker（监听器）

```c
int              hidtc_mouse_hooker_is_button_pressed(HidtcMouseButton button);
HidtcAbsolutePos hidtc_mouse_hooker_get_cursor_pos(void); /* Linux 上始终返回 {0,0} */

int  hidtc_mouse_hooker_run(void);
void hidtc_mouse_hooker_stop(void);
int  hidtc_mouse_hooker_is_running(void);

int hidtc_mouse_hooker_set_event_handler(HidtcMouseEventHandler handler, void* userData);
int hidtc_mouse_hooker_set_user_data(void* userData);
```

---

### 鼠标 Simulator（模拟器）

```c
/* 获取当前显示器绝对移动的有效坐标范围 */
HidtcAbsolutePosRange hidtc_mouse_simulator_get_absolute_move_range(void);

int    hidtc_mouse_simulator_initialize(void);
void   hidtc_mouse_simulator_destroy(void);
int    hidtc_mouse_simulator_is_initialized(void);

int    hidtc_mouse_simulator_send_event(const HidtcMouseEvent* event);
size_t hidtc_mouse_simulator_send_events(const HidtcMouseEvent* events, size_t count);

/* 移动 */
int hidtc_mouse_simulator_move_to(int32_t x, int32_t y);         /* 绝对移动 */
int hidtc_mouse_simulator_move_by(int32_t dx, int32_t dy);        /* 相对移动 */

/* 滚轮 — 单位 120，正值远离用户，负值朝向用户 */
int hidtc_mouse_simulator_wheel(int32_t wheelDelta);

/* 在当前光标位置执行按键操作 */
int hidtc_mouse_simulator_press_button(HidtcMouseButton button);
int hidtc_mouse_simulator_release_button(HidtcMouseButton button);
int hidtc_mouse_simulator_click_button(HidtcMouseButton button, size_t intervalMs);
int hidtc_mouse_simulator_double_click_button(HidtcMouseButton button,
                                               size_t interval1Ms, size_t interval2Ms);

/* 先移动到指定绝对坐标，再执行按键操作 */
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

/* 拖拽 */
int hidtc_mouse_simulator_drag_to(int32_t x, int32_t y, HidtcMouseButton button);
/* 从当前位置拖拽到目标（按下 + 拖 + 释放） */
int hidtc_mouse_simulator_drag_combo(int32_t endX, int32_t endY,
                                      HidtcMouseButton button, size_t intervalMs);
/* 从指定起点拖拽到终点（移动 + 按下 + 拖 + 释放） */
int hidtc_mouse_simulator_drag_combo_from(int32_t startX, int32_t startY,
                                           int32_t endX,   int32_t endY,
                                           HidtcMouseButton button,
                                           size_t interval1Ms, size_t interval2Ms);
```

---

### HID Hooker（统一监听器）

将键盘和鼠标监听合并为单一回调接口。

```c
int hidtc_hid_hooker_is_support_block_event_propagation(void);
int hidtc_hid_hooker_is_key_pressed(int32_t nativeKey);          /* 仅键盘 */
int hidtc_hid_hooker_is_button_pressed(HidtcMouseButton button);  /* 仅鼠标 */
HidtcAbsolutePos hidtc_hid_hooker_get_cursor_pos(void);           /* 仅鼠标 */

int  hidtc_hid_hooker_run(void);
void hidtc_hid_hooker_stop(void);
int  hidtc_hid_hooker_is_running(void);

int hidtc_hid_hooker_set_event_handler(HidtcHidEventHandler handler, void* userData);
int hidtc_hid_hooker_set_user_data(void* userData);
```

---

### HID Simulator（统一模拟器）

通过统一接口发送键盘、鼠标和休眠事件。

```c
int    hidtc_hid_simulator_initialize(void);
void   hidtc_hid_simulator_destroy(void);
int    hidtc_hid_simulator_is_initialized(void);

int    hidtc_hid_simulator_send_event(const HidtcHidEvent* event);
size_t hidtc_hid_simulator_send_events(const HidtcHidEvent* events, size_t count);
```

---

## 使用示例

### 示例 1 — 监听全部键盘事件

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
        printf("[按下]   原生键值=0x%X  枚举值=0x%X\n", event->nativeKey, (unsigned)key);
    }
    else if (event->type == HIDTC_KBD_ET_RELEASE)
    {
        printf("[抬起] 原生键值=0x%X\n", event->nativeKey);
    }
    return 1; /* 传播事件 */
}

int main(void)
{
    hidtc_keyboard_hooker_set_event_handler(on_key_event, NULL);
    hidtc_keyboard_hooker_run();
    printf("正在监听键盘事件，按任意键（Ctrl+C 退出）...\n");

    while (hidtc_keyboard_hooker_is_running())
        SLEEP_MS(100);

    return 0;
}
```

---

### 示例 2 — 模拟键盘输入（输入 "Hello"）

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

int main(void)
{
    if (!hidtc_keyboard_simulator_initialize())
    {
        fprintf(stderr, "键盘模拟器初始化失败。\n");
        return 1;
    }

    /* 输入 "Hello"：Shift+H, e, l, l, o */
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

### 示例 3 — 模拟快捷键 Ctrl+C（复制）

```c
#include <hidtool_c/hidtool_c.h>

int main(void)
{
    hidtc_keyboard_simulator_initialize();

    /* 构造并批量发送 4 个事件 */
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

### 示例 4 — 监听鼠标事件

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

int on_mouse_event(const HidtcMouseEvent* event, void* userData)
{
    (void)userData;
    switch (event->type)
    {
    case HIDTC_MS_ET_ABS_MOVE:
        printf("[移动]   x=%d  y=%d\n", event->absPos.x, event->absPos.y);
        break;
    case HIDTC_MS_ET_WHEEL:
        printf("[滚轮]   delta=%d\n", event->wheelDelta);
        break;
    case HIDTC_MS_ET_PRESS:
        printf("[按下]   按键=%d\n", (int)event->button);
        break;
    case HIDTC_MS_ET_RELEASE:
        printf("[抬起]   按键=%d\n", (int)event->button);
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
    printf("正在监听鼠标事件，持续 10 秒...\n");

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

### 示例 5 — 模拟鼠标：移动、点击、滚动

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

    /* 获取屏幕坐标范围 */
    HidtcAbsolutePosRange range = hidtc_mouse_simulator_get_absolute_move_range();
    int32_t cx = (range.min_x + range.max_x) / 2;
    int32_t cy = (range.min_y + range.max_y) / 2;

    /* 移动到屏幕中央 */
    hidtc_mouse_simulator_move_to(cx, cy);
    SLEEP_MS(200);

    /* 单击左键 */
    hidtc_mouse_simulator_click_button(HIDTC_MSBTN_LEFT, 50);
    SLEEP_MS(200);

    /* 双击左键 */
    hidtc_mouse_simulator_double_click_button(HIDTC_MSBTN_LEFT, 50, 100);
    SLEEP_MS(200);

    /* 向上滚动 */
    hidtc_mouse_simulator_wheel(120);
    SLEEP_MS(100);

    /* 相对移动 */
    hidtc_mouse_simulator_move_by(50, 50);
    SLEEP_MS(100);

    /* 从 (cx,cy) 拖拽到 (cx+200, cy+100) */
    hidtc_mouse_simulator_drag_combo_from(cx, cy, cx + 200, cy + 100,
                                           HIDTC_MSBTN_LEFT, 10, 0);

    hidtc_mouse_simulator_destroy();
    return 0;
}
```

---

### 示例 6 — 统一 HID 监听器与用户数据

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

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
        printf("键盘按下总次数：%d\n", stats->keyPressCount);
    }
    else if (event->type == HIDTC_HID_ET_MOUSE &&
             event->mouseEvent.type == HIDTC_MS_ET_PRESS)
    {
        stats->mouseClickCount++;
        printf("鼠标点击总次数：%d\n", stats->mouseClickCount);
    }
    return 1;
}

int main(void)
{
    Stats stats = {0, 0};

    hidtc_hid_hooker_set_event_handler(on_hid_event, &stats);
    hidtc_hid_hooker_run();
    printf("正在监控输入，按 Ctrl+C 退出。\n");

    while (hidtc_hid_hooker_is_running())
    {
#ifdef _WIN32
        Sleep(100);
#else
        usleep(100000);
#endif
    }

    printf("统计结果 — 按键：%d 次，点击：%d 次\n",
           stats.keyPressCount, stats.mouseClickCount);
    return 0;
}
```

---

### 示例 7 — 屏蔽特定按键（需平台支持）

```c
#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

int on_key_event(const HidtcKeyboardEvent* event, void* userData)
{
    if (event->type == HIDTC_KBD_ET_PRESS)
    {
        HidtcKeyboardKey key = hidtc_keyboard_key_from_native_key(event->nativeKey);
        if (key == HIDTC_KBDKEY_ESCAPE)
        {
            printf("Escape 键已被屏蔽。\n");
            return 0; /* 阻断传播 */
        }
    }
    return 1; /* 其余按键正常传播 */
}

int main(void)
{
    if (!hidtc_hid_hooker_is_support_block_event_propagation())
    {
        printf("当前平台不支持阻断事件传播。\n");
        return 1;
    }

    hidtc_keyboard_hooker_set_event_handler(on_key_event, NULL);
    hidtc_keyboard_hooker_run();
    printf("运行中，Escape 键已屏蔽，按 Ctrl+C 退出。\n");

#ifdef _WIN32
    Sleep(INFINITE);
#else
    pause();
#endif
    return 0;
}
```

---

### 示例 8 — 通过统一模拟器发送混合事件序列

```c
#include <hidtool_c/hidtool_c.h>

int main(void)
{
    if (!hidtc_hid_simulator_initialize())
        return 1;

    HidtcHidEvent events[5];

    /* 事件 0：按下 Ctrl */
    events[0].type                        = HIDTC_HID_ET_KEYBOARD;
    events[0].keyboardEvent.type          = HIDTC_KBD_ET_PRESS;
    events[0].keyboardEvent.nativeKey     =
        hidtc_keyboard_key_to_native_key(HIDTC_KBDKEY_CTRL);
    events[0].keyboardEvent.timestamp     = 0;

    /* 事件 1：按下 A */
    events[1].type                        = HIDTC_HID_ET_KEYBOARD;
    events[1].keyboardEvent.type          = HIDTC_KBD_ET_PRESS;
    events[1].keyboardEvent.nativeKey     =
        hidtc_keyboard_key_to_native_key(HIDTC_KBDKEY_A);
    events[1].keyboardEvent.timestamp     = 0;

    /* 事件 2：抬起 A */
    events[2].type                        = HIDTC_HID_ET_KEYBOARD;
    events[2].keyboardEvent.type          = HIDTC_KBD_ET_RELEASE;
    events[2].keyboardEvent.nativeKey     =
        hidtc_keyboard_key_to_native_key(HIDTC_KBDKEY_A);
    events[2].keyboardEvent.timestamp     = 0;

    /* 事件 3：抬起 Ctrl */
    events[3].type                        = HIDTC_HID_ET_KEYBOARD;
    events[3].keyboardEvent.type          = HIDTC_KBD_ET_RELEASE;
    events[3].keyboardEvent.nativeKey     =
        hidtc_keyboard_key_to_native_key(HIDTC_KBDKEY_CTRL);
    events[3].keyboardEvent.timestamp     = 0;

    /* 事件 4：休眠 100ms */
    events[4].type    = HIDTC_HID_ET_SLEEP;
    events[4].sleepMs = 100;

    hidtc_hid_simulator_send_events(events, 5);
    hidtc_hid_simulator_destroy();
    return 0;
}
```
