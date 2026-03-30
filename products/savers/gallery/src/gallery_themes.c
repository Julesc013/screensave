#include <string.h>

#include "gallery_internal.h"

const screensave_theme_descriptor g_gallery_themes[] = {
    {
        "compatibility_gallery",
        "Compatibility Gallery",
        "Low-contrast charcoal, warm white, and slate tones for baseline-safe viewing.",
        { 126, 132, 142, 255 },
        { 232, 232, 224, 255 }
    },
    {
        "gl11_classic",
        "GL11 Classic",
        "Blue-cyan classic acceleration tones with cool highlights and steady contrast.",
        { 92, 150, 214, 255 },
        { 206, 244, 255, 255 }
    },
    {
        "advanced_showcase",
        "Advanced Showcase",
        "High-contrast showcase tones with bright edges and deep indigo shadows.",
        { 148, 104, 232, 255 },
        { 244, 236, 255, 255 }
    },
    {
        "neon_abstract",
        "Neon Abstract",
        "Electric magenta, cyan, and violet tones for brisk abstract motion.",
        { 232, 88, 208, 255 },
        { 112, 244, 255, 255 }
    },
    {
        "technical_exhibit",
        "Technical Exhibit",
        "Green-white instrument tones with measured exhibit contrast.",
        { 92, 196, 144, 255 },
        { 236, 248, 240, 255 }
    },
    {
        "quiet_museum_renderer_tour",
        "Quiet Museum Renderer Tour",
        "Muted museum neutrals for restrained motion and low-distraction viewing.",
        { 174, 162, 142, 255 },
        { 238, 232, 220, 255 }
    },
    {
        "amber_compatibility",
        "Amber Compatibility",
        "Warm amber-and-charcoal compatibility palette for calmer baseline renderer tours.",
        { 188, 148, 92, 255 },
        { 236, 220, 176, 255 }
    },
    {
        "gl21_prism_hall",
        "GL21 Prism Hall",
        "Cool prism-hall palette with brighter cyan-violet edges and cleaner technical layering.",
        { 124, 148, 238, 255 },
        { 244, 236, 255, 255 }
    }
};

const screensave_theme_descriptor *gallery_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_gallery_themes) / sizeof(g_gallery_themes[0]));
    }

    return g_gallery_themes;
}

const screensave_theme_descriptor *gallery_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;
    unsigned int index;

    themes = gallery_get_themes(&theme_count);
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
