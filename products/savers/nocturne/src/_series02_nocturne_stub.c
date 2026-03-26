/*
 * Series 02 build-only scaffolding.
 * This target proves the product build path without implementing real saver logic.
 */

#include <windows.h>

int screensave_series02_core_stub(void);
int screensave_series02_host_stub(void);

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show_code)
{
    (void)instance;
    (void)previous;
    (void)command_line;
    (void)show_code;

    return screensave_series02_core_stub() + screensave_series02_host_stub();
}
