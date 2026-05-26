/*
 * mouse_monitor — hidtool_c example
 *
 * Demonstrates global mouse event listening.
 * Prints every mouse event (move, click, wheel, drag) to stdout.
 * Press ENTER in the console to stop.
 */

#include <hidtool_c/hidtool_c.h>
#include <stdio.h>

static const char* button_name(HidtMouseButton btn)
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

static int on_mouse_event(const HidtMouseEvent* event, void* user_data)
{
    (void) user_data;

    switch (event->type)
    {
        case HIDT_MS_ET_ABS_MOVE:
            printf(
                "[ABS_MOVE]  x=%-6d  y=%d\n",
                event->u.abs_pos.x, event->u.abs_pos.y);
            break;
        case HIDT_MS_ET_REL_MOVE:
            printf(
                "[REL_MOVE]  dx=%-6d  dy=%d\n",
                event->u.rel_pos.dx, event->u.rel_pos.dy);
            break;
        case HIDT_MS_ET_WHEEL:
            printf("[WHEEL]     delta=%d\n", event->u.wheel_delta);
            break;
        case HIDT_MS_ET_DRAG:
            printf(
                "[DRAG]      x=%-6d  y=%-6d  button=%s\n",
                event->u.drag.pos.x, event->u.drag.pos.y,
                button_name(event->u.drag.button));
            break;
        case HIDT_MS_ET_PRESS:
            printf("[PRESS]     button=%s\n", button_name(event->u.button));
            break;
        case HIDT_MS_ET_RELEASE:
            printf("[RELEASE]   button=%s\n", button_name(event->u.button));
            break;
        default:
            break;
    }

    return 1; // propagate event
}

int main(void)
{
    if (!hidt_is_hid_type_supported(HIDT_HIDTYPE_MOUSE))
    {
        fprintf(stderr, "Mouse module is not supported in this build.\n");
        return 1;
    }

    hidt_mouse_hooker_set_event_handler(on_mouse_event, NULL);

    if (!hidt_mouse_hooker_run())
    {
        fprintf(stderr, "Failed to start mouse hooker.\n");
        return 1;
    }

    printf("Mouse monitor started. Press ENTER here to stop...\n");
    getchar();

    hidt_mouse_hooker_stop();
    printf("Stopped.\n");
    return 0;
}
