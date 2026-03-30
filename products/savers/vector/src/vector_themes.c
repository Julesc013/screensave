#include "vector_internal.h"

const screensave_theme_descriptor g_vector_themes[] = {
    {
        "quiet_midnight_grid",
        "Quiet Midnight Grid",
        "Cool midnight lines with restrained blue-gold accents.",
        { 108, 142, 196, 255 },
        { 64, 96, 154, 255 }
    },
    {
        "terminal_green_wireframe",
        "Terminal Green Wireframe",
        "Phosphor-green lines against a dark terminal backdrop.",
        { 96, 220, 144, 255 },
        { 32, 118, 76, 255 }
    },
    {
        "sgi_neon",
        "SGI Neon",
        "Bright cool highlights with a classic workstation glow.",
        { 226, 238, 248, 255 },
        { 92, 180, 255, 255 }
    },
    {
        "white_drafting_board",
        "White Drafting Board",
        "High-contrast drafting tones with graphite lines and blue accents.",
        { 54, 66, 82, 255 },
        { 136, 156, 188, 255 }
    },
    {
        "museum_vector_exhibit",
        "Museum Vector Exhibit",
        "Subdued neutral tones for a quiet gallery-style presentation.",
        { 178, 170, 154, 255 },
        { 92, 102, 120, 255 }
    },
    {
        "amber_wire_tunnel",
        "Amber Wire Tunnel",
        "Warm amber vector-terminal tones with restrained drafting-board contrast.",
        { 210, 168, 112, 255 },
        { 96, 78, 48, 255 }
    },
    {
        "terrain_museum_glide",
        "Terrain Museum Glide",
        "Quiet museum terrain palette with slate lines and pale exhibit highlights.",
        { 164, 170, 182, 255 },
        { 104, 118, 144, 255 }
    }
};

const screensave_theme_descriptor *vector_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_vector_themes) / sizeof(g_vector_themes[0]));
    }

    return g_vector_themes;
}

const screensave_theme_descriptor *vector_find_theme_descriptor(const char *theme_key)
{
    unsigned int theme_count;
    const screensave_theme_descriptor *themes;

    themes = vector_get_themes(&theme_count);
    return screensave_find_theme(themes, theme_count, theme_key);
}
