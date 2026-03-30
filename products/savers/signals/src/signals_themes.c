#include <string.h>

#include "signals_internal.h"

const screensave_theme_descriptor g_signals_themes[] = {
    {
        "green_terminal",
        "Green Terminal",
        "Green phosphor console with pale terminal highlights.",
        { 36, 160, 72, 255 },
        { 180, 255, 196, 255 }
    },
    {
        "amber_scope",
        "Amber Scope",
        "Warm amber instrument panel with gold highlight strokes.",
        { 212, 132, 44, 255 },
        { 255, 222, 140, 255 }
    },
    {
        "blue_lab",
        "Blue Lab",
        "Cool laboratory blue with cyan indicator accents.",
        { 58, 126, 196, 255 },
        { 168, 230, 255, 255 }
    },
    {
        "white_technical_board",
        "White Technical Board",
        "Light drafting-board tones with blue-grey panel accents.",
        { 88, 116, 146, 255 },
        { 232, 244, 255, 255 }
    },
    {
        "quiet_night_console",
        "Quiet Night Console",
        "Low-intensity slate console for subdued rooms.",
        { 90, 118, 150, 255 },
        { 174, 204, 230, 255 }
    },
    {
        "telemetry_wall",
        "Telemetry Wall",
        "Cool telemetry blue with bright cyan board accents.",
        { 76, 144, 214, 255 },
        { 192, 244, 255, 255 }
    },
    {
        "night_watch_console",
        "Night Watch Console",
        "Dark navy watch-console palette with restrained pale indicators.",
        { 92, 124, 170, 255 },
        { 196, 220, 244, 255 }
    }
};

const screensave_theme_descriptor *signals_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_signals_themes) / sizeof(g_signals_themes[0]));
    }

    return g_signals_themes;
}

const screensave_theme_descriptor *signals_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;
    unsigned int index;

    themes = signals_get_themes(&theme_count);
    if (theme_key == NULL || theme_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < theme_count; ++index) {
        if (themes[index].theme_key != NULL && strcmp(themes[index].theme_key, theme_key) == 0) {
            return &themes[index];
        }
    }

    return NULL;
}
