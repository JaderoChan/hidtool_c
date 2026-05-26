/*
 * hid_monitor — hidtool_c example
 *
 * Demonstrates the unified HID event hooker (keyboard + mouse together).
 * Passes a custom context struct as user_data to the callback.
 * Press Escape to stop.
 *
 * NOTE: hidt_hid_hooker_stop() MUST NOT be called from inside the event
 * callback, because the callback runs on the hooker's own thread and calling
 * stop() from that thread would cause it to join itself (EDEADLK / deadlock).
 * Instead, set an atomic flag in the callback and let the main thread stop.
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

// user context passed as user_data
typedef struct AppContext
{
    volatile int event_count;
    volatile int should_stop; // set to 1 by callback to request main-thread stop
    int          verbose;     // print all events when non-zero
} AppContext;

static const char* mouse_button_name(HidtMouseButton btn)
{
    switch (btn)
    {
        case HIDT_MSBTN_LEFT:       return "LEFT";
        case HIDT_MSBTN_RIGHT:      return "RIGHT";
        case HIDT_MSBTN_MIDDLE:     return "MIDDLE";
        case HIDT_MSBTN_BACK:       return "BACK";
        case HIDT_MSBTN_FORWARD:    return "FORWARD";
        default:                    return "NONE";
    }
}

// event handler
static int on_hid_event(const HidtHidEvent* event, void* user_data)
{
    AppContext* ctx = (AppContext*) user_data;
    ctx->event_count++;

    if (event->type == HIDT_HID_ET_KEYBOARD)
    {
        const HidtKeyboardEvent* ke = &event->u.keyboard_event;
        HidtKeyboardKey k = hidt_keyboard_key_from_native_key(ke->u.native_key);

        if (ke->type == HIDT_KBD_ET_PRESS)
        {
            // Signal main thread to stop — do NOT call stop() here directly
            if (k == HIDT_KBDKEY_ESCAPE)
            {
                printf("\n[ESC detected] requesting stop...\n");
                ctx->should_stop = 1;
                return 1;
            }

            if (ctx->verbose)
                printf(
                    "[KBD PRESS]   key=0x%04X  native=0x%08X\n",
                    (unsigned) k, (unsigned) ke->u.native_key);
        }
        else if (ke->type == HIDT_KBD_ET_RELEASE && ctx->verbose)
        {
            printf(
                "[KBD RELEASE] key=0x%04X  native=0x%08X\n",
                (unsigned) k, (unsigned) ke->u.native_key);
        }
    }
    else if (event->type == HIDT_HID_ET_MOUSE)
    {
        if (!ctx->verbose)
            return 1;

        const HidtMouseEvent* me = &event->u.mouse_event;
        switch (me->type)
        {
            case HIDT_MS_ET_ABS_MOVE:
                printf("[MOUSE MOVE]    x=%-6d y=%d\n", me->u.abs_pos.x, me->u.abs_pos.y);
                break;
            case HIDT_MS_ET_REL_MOVE:
                printf("[MOUSE REL]     dx=%-6d dy=%d\n", me->u.rel_pos.dx, me->u.rel_pos.dy);
                break;
            case HIDT_MS_ET_WHEEL:
                printf("[MOUSE WHEEL]   delta=%d\n", me->u.wheel_delta);
                break;
            case HIDT_MS_ET_PRESS:
                printf("[MOUSE PRESS]   button=%s\n", mouse_button_name(me->u.button));
                break;
            case HIDT_MS_ET_RELEASE:
                printf("[MOUSE RELEASE] button=%s\n", mouse_button_name(me->u.button));
                break;
            case HIDT_MS_ET_DRAG:
                printf("[MOUSE DRAG]    x=%-6d y=%-6d button=%s\n",
                    me->u.drag.pos.x, me->u.drag.pos.y, mouse_button_name(me->u.drag.button));
                break;
            default:
                break;
        }
    }

    return 1; // propagate event
}

int main(void)
{
    int kbd_ok = hidt_is_hid_type_supported(HIDT_HIDTYPE_KEYBOARD);
    int ms_ok  = hidt_is_hid_type_supported(HIDT_HIDTYPE_MOUSE);

    if (!kbd_ok && !ms_ok)
    {
        fprintf(stderr, "Neither keyboard nor mouse module is available.\n");
        return 1;
    }

    printf(
        "Supported: keyboard=%s  mouse=%s\n",
        kbd_ok ? "yes" : "no",
        ms_ok  ? "yes" : "no");

    AppContext ctx;
    ctx.event_count = 0;
    ctx.should_stop = 0;
    ctx.verbose = 1;

    hidt_hid_hooker_set_event_handler(on_hid_event, &ctx);

    if (!hidt_hid_hooker_run())
    {
        fprintf(stderr, "Failed to start HID hooker.\n");
        return 1;
    }

    printf("Unified HID monitor started (verbose).\n");
    printf("Press the Escape key to stop.\n");

    // Main thread polls the stop flag set by the callback.
    // Stopping from outside the callback thread avoids the EDEADLK error.
    while (!ctx.should_stop)
        SLEEP_MS(50);

    hidt_hid_hooker_stop();

    printf(
        "Stopped. Total events received: %d\n",
        ctx.event_count);
    return 0;
}
