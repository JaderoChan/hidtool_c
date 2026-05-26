/*
 * keyboard_monitor — hidtool_c example
 *
 * Demonstrates global keyboard event listening.
 * Prints every key press/release to stdout.
 * Press ENTER in the console to stop.
 */

#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

static int on_keyboard_event(const HidtKeyboardEvent* event, void* user_data)
{
    (void) user_data;

    if (event->type == HIDT_KBD_ET_PRESS)
    {
        HidtKeyboardKey key = hidt_keyboard_key_from_native_key(event->u.native_key);
        printf(
            "[PRESS]   native=0x%08X  key=0x%04X\n",
            (unsigned) event->u.native_key, (unsigned) key);
    }
    else if (event->type == HIDT_KBD_ET_RELEASE)
    {
        HidtKeyboardKey key = hidt_keyboard_key_from_native_key(event->u.native_key);
        printf(
            "[RELEASE] native=0x%08X  key=0x%04X\n",
            (unsigned) event->u.native_key, (unsigned) key);
    }

    return 1; // propagate event
}

int main(void)
{
    if (!hidt_is_hid_type_supported(HIDT_HIDTYPE_KEYBOARD))
    {
        fprintf(stderr, "Keyboard module is not supported in this build.\n");
        return 1;
    }

    hidt_keyboard_hooker_set_event_handler(on_keyboard_event, NULL);

    if (!hidt_keyboard_hooker_run())
    {
        fprintf(stderr, "Failed to start keyboard hooker.\n");
        return 1;
    }

    printf("Keyboard monitor started. Press ENTER here to stop...\n");
    getchar();

    hidt_keyboard_hooker_stop();
    printf("Stopped.\n");
    return 0;
}
