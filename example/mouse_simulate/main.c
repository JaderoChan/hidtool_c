/*
 * mouse_simulate — hidtool_c example
 *
 * Demonstrates mouse input simulation:
 *   1. Move to screen centre.
 *   2. Left-click.
 *   3. Scroll down.
 *   4. Drag from one point to another.
 *   5. Double-click.
 *
 * Press ENTER in this console to begin (gives you time to position windows).
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

int main(void)
{
    if (!hidt_is_hid_type_supported(HIDT_HIDTYPE_MOUSE))
    {
        fprintf(stderr, "Mouse module is not supported in this build.\n");
        return 1;
    }

    if (!hidt_mouse_simulator_initialize())
    {
        fprintf(stderr, "Failed to initialize mouse simulator.\n");
        return 1;
    }

    printf("Press ENTER to start the mouse simulation sequence...\n");
    getchar();

    // 1. Move to absolute position (800, 600)
    printf("1. Moving to (800, 600)...\n");
    hidt_mouse_simulator_move_to(800, 600);
    SLEEP_MS(500);

    // 2. Left-click at current position
    printf("2. Left-click...\n");
    hidt_mouse_simulator_click_button(HIDT_MSBTN_LEFT, 0);
    SLEEP_MS(500);

    // 3. Scroll down by 3 ticks (each tick = 120 units)
    printf("3. Scroll down 3 ticks...\n");
    hidt_mouse_simulator_wheel(-3 * 120); // negative = toward user
    SLEEP_MS(500);

    // 4. Relative move (+100, +50)
    printf("4. Relative move +100,+50...\n");
    hidt_mouse_simulator_move_by(100, 50);
    SLEEP_MS(500);

    // 5. Drag from (400, 400) to (600, 600) holding left button
    printf("5. Drag (400,400) -> (600,600)...\n");
    hidt_mouse_simulator_drag_combo_from(400, 400, 600, 600, HIDT_MSBTN_LEFT, 0, 0);
    SLEEP_MS(500);

    // 6. Double-click
    printf("6. Double-click at (800,600)...\n");
    hidt_mouse_simulator_move_to(800, 600);
    hidt_mouse_simulator_double_click_button(HIDT_MSBTN_LEFT, 0, 0);
    SLEEP_MS(200);

    hidt_mouse_simulator_destroy();
    printf("Done.\n");
    return 0;
}
