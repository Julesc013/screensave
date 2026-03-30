#include <string.h>

#include "mechanize_internal.h"

const screensave_theme_descriptor g_mechanize_themes[] = {
    {
        "brass_workshop",
        "Brass Workshop",
        "Warm brass and cream tones for workshop assemblies.",
        { 182, 128, 64, 255 },
        { 252, 220, 152, 255 }
    },
    {
        "steel_machine_room",
        "Steel Machine Room",
        "Cool steel tones with pale mechanical highlights.",
        { 124, 140, 154, 255 },
        { 220, 232, 238, 255 }
    },
    {
        "black_enamel_instrument",
        "Black Enamel Instrument",
        "Dark instrument face tones with ivory pointers.",
        { 116, 126, 132, 255 },
        { 240, 236, 212, 255 }
    },
    {
        "industrial_green",
        "Industrial Green",
        "Factory green housings with brass-like accents.",
        { 84, 134, 88, 255 },
        { 224, 212, 138, 255 }
    },
    {
        "quiet_museum",
        "Quiet Museum",
        "Soft museum display palette for patient scenes.",
        { 122, 116, 102, 255 },
        { 214, 204, 182, 255 }
    },
    {
        "copper_foundry",
        "Copper Foundry",
        "Copper and smoked enamel tones for restrained counter assemblies.",
        { 176, 112, 74, 255 },
        { 246, 210, 166, 255 }
    },
    {
        "ivory_gallery",
        "Ivory Gallery",
        "Ivory and slate exhibit palette for calmer museum scenes.",
        { 176, 168, 146, 255 },
        { 242, 236, 214, 255 }
    }
};

const screensave_theme_descriptor *mechanize_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_mechanize_themes) / sizeof(g_mechanize_themes[0]));
    }

    return g_mechanize_themes;
}

const screensave_theme_descriptor *mechanize_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;
    unsigned int index;

    themes = mechanize_get_themes(&theme_count);
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
