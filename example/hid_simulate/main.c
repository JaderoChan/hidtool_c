/*
 * hid_simulate — hidtool_c example
 *
 * Demonstrates the unified HID simulator sending a mixed event sequence:
 *   keyboard presses, a SLEEP event (pause inside the sequence), and
 *   mouse movement — all in a single batched call.
 *
 * Focus a text editor and press ENTER to start.
 */

#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

int main(void)
{
    int kbd_ok = hidt_is_hid_type_supported(HIDT_HIDTYPE_KEYBOARD);
    int ms_ok  = hidt_is_hid_type_supported(HIDT_HIDTYPE_MOUSE);

    if (!kbd_ok && !ms_ok)
    {
        fprintf(stderr, "Neither keyboard nor mouse module is available.\n");
        return 1;
    }

    if (!hidt_hid_simulator_initialize())
    {
        fprintf(stderr, "Failed to initialize HID simulator.\n");
        return 1;
    }

    printf("Switch to a text editor, then press ENTER here to begin...\n");
    getchar();

    /* Build a mixed sequence:
     *
     * 1. Press 'A'
     * 2. Release 'A'
     * 3. Sleep 200 ms
     * 4. Move mouse to (640, 480)
     * 5. Left-click at (640, 480)
     */
    HidtHidEvent events[5] = {0};

    // keyboard press 'A'
    events[0].type                          = HIDT_HID_ET_KEYBOARD;
    events[0].u.keyboard_event.type         = HIDT_KBD_ET_PRESS;
    events[0].u.keyboard_event.u.native_key =
        hidt_keyboard_key_to_native_key(HIDT_KBDKEY_A);

    // keyboard release 'A'
    events[1].type                          = HIDT_HID_ET_KEYBOARD;
    events[1].u.keyboard_event.type         = HIDT_KBD_ET_RELEASE;
    events[1].u.keyboard_event.u.native_key =
        hidt_keyboard_key_to_native_key(HIDT_KBDKEY_A);

    // sleep 200 ms
    events[2].type                          = HIDT_HID_ET_SLEEP;
    events[2].u.sleep_ms                    = 200;

    // mouse abs-move to (640, 480)
    events[3].type                          = HIDT_HID_ET_MOUSE;
    events[3].u.mouse_event.type            = HIDT_MS_ET_ABS_MOVE;
    events[3].u.mouse_event.u.abs_pos.x     = 640;
    events[3].u.mouse_event.u.abs_pos.y     = 480;

    // mouse left-click at (640, 480)
    events[4].type                          = HIDT_HID_ET_MOUSE;
    events[4].u.mouse_event.type            = HIDT_MS_ET_PRESS;
    events[4].u.mouse_event.u.button        = HIDT_MSBTN_LEFT;

    size_t sent = hidt_hid_simulator_send_events(events, 5);
    printf("Sent %zu / 5 events.\n", sent);

    hidt_hid_simulator_destroy();
    printf("Done.\n");
    return 0;
}
