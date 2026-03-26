#include "benchlab_internal.h"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous_instance, LPSTR command_line, int show_code)
{
    (void)previous_instance;
    return benchlab_app_run(instance, command_line, show_code);
}
