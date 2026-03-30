#include <string.h>

#include "anthology_internal.h"

const screensave_theme_descriptor g_anthology_themes[] = {
    {
        "anthology_neutral",
        "Anthology Neutral",
        "Neutral anthology control theme that keeps cross-saver orchestration visually product-agnostic.",
        { 148, 156, 172, 255 },
        { 232, 236, 244, 255 }
    },
    {
        "anthology_amber",
        "Anthology Amber",
        "Warm amber anthology palette tuned for compatibility-safe after-hours rotations.",
        { 188, 154, 104, 255 },
        { 240, 224, 188, 255 }
    },
    {
        "anthology_midnight",
        "Anthology Midnight",
        "Cool midnight anthology palette for scenic and heavyweight grand-tour sessions.",
        { 112, 132, 188, 255 },
        { 226, 236, 252, 255 }
    }
};

const screensave_theme_descriptor *anthology_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_anthology_themes) / sizeof(g_anthology_themes[0]));
    }

    return g_anthology_themes;
}

const screensave_theme_descriptor *anthology_find_theme_descriptor(const char *theme_key)
{
    unsigned int index;

    if (theme_key == NULL || theme_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < ANTHOLOGY_THEME_COUNT; ++index) {
        if (
            g_anthology_themes[index].theme_key != NULL &&
            strcmp(g_anthology_themes[index].theme_key, theme_key) == 0
        ) {
            return &g_anthology_themes[index];
        }
    }

    return NULL;
}
