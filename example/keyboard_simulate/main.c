/*
 * keyboard_simulate — hidtool_c example
 *
 * Demonstrates keyboard input simulation:
 *   1. Type "Hello, World!" (with Shift for uppercase / symbols).
 *   2. Send Ctrl+A (select all) as a batched event sequence.
 *
 * Focus a text editor before running, then press ENTER in this console.
 */

#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

#ifdef _WIN32
    #include <windows.h>
    #define SLEEP_MS(ms) Sleep(ms)
#else
    #include <unistd.h>
    #define SLEEP_MS(ms) usleep((unsigned int) ((ms) * 1000))
#endif

// Helper: click a key with a small inter-key delay for realistic typing.
static void type_key(HidtKeyboardKey key)
{
    hidt_keyboard_simulator_click_key(key, 0);
    SLEEP_MS(30);
}

// Helper: type a shifted character (e.g. uppercase letter or symbol).
static void type_shifted(HidtKeyboardKey key)
{
    hidt_keyboard_simulator_press_key(HIDT_KBDKEY_SHIFT_LEFT);
    SLEEP_MS(10);
    hidt_keyboard_simulator_click_key(key, 0);
    SLEEP_MS(10);
    hidt_keyboard_simulator_release_key(HIDT_KBDKEY_SHIFT_LEFT);
    SLEEP_MS(30);
}

int main(void)
{
    if (!hidt_is_hid_type_supported(HIDT_HIDTYPE_KEYBOARD))
    {
        fprintf(stderr, "Keyboard module is not supported in this build.\n");
        return 1;
    }

    if (!hidt_keyboard_simulator_initialize())
    {
        fprintf(stderr, "Failed to initialize keyboard simulator.\n");
        return 1;
    }

    printf("Switch to a text editor, then press ENTER here to begin...\n");
    getchar();

    // Part 1: type "Hello, World!"
    printf("Typing \"Hello, World!\"...\n");

    // H E L L O
    type_shifted(HIDT_KBDKEY_H);
    type_key(HIDT_KBDKEY_E);
    type_key(HIDT_KBDKEY_L);
    type_key(HIDT_KBDKEY_L);
    type_key(HIDT_KBDKEY_O);
    /// ,  (comma + space)
    type_key(HIDT_KBDKEY_COMMA);
    type_key(HIDT_KBDKEY_SPACE);
    // W O R L D
    type_shifted(HIDT_KBDKEY_W);
    type_key(HIDT_KBDKEY_O);
    type_key(HIDT_KBDKEY_R);
    type_key(HIDT_KBDKEY_L);
    type_key(HIDT_KBDKEY_D);
    // ! (Shift+1)
    type_shifted(HIDT_KBDKEY_1);

    SLEEP_MS(300);

    // Part 2: send Ctrl+A as a batched event sequence
    printf("Sending Ctrl+A (select all) as a batch...\n");

    HidtKeyboardEvent events[4];

    events[0].type = HIDT_KBD_ET_PRESS;
    events[0].u.native_key = hidt_keyboard_key_to_native_key(HIDT_KBDKEY_CTRL_LEFT);
    events[0].timestamp = 0;

    events[1].type = HIDT_KBD_ET_PRESS;
    events[1].u.native_key = hidt_keyboard_key_to_native_key(HIDT_KBDKEY_A);
    events[1].timestamp = 0;

    events[2].type = HIDT_KBD_ET_RELEASE;
    events[2].u.native_key = hidt_keyboard_key_to_native_key(HIDT_KBDKEY_A);
    events[2].timestamp = 0;

    events[3].type = HIDT_KBD_ET_RELEASE;
    events[3].u.native_key = hidt_keyboard_key_to_native_key(HIDT_KBDKEY_CTRL_LEFT);
    events[3].timestamp = 0;

    size_t sent = hidt_keyboard_simulator_send_events(events, 4);
    printf("Sent %zu / 4 events.\n", sent);

    hidt_keyboard_simulator_destroy();
    printf("Done.\n");
    return 0;
}
