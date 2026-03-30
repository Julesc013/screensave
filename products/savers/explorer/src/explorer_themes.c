#include "explorer_internal.h"

const screensave_theme_descriptor g_explorer_themes[] = {
    {
        "cold_lab",
        "Cold Lab",
        "Blue-gray corridor lighting with clean, restrained forward travel.",
        { 54, 72, 90, 255 },
        { 180, 226, 244, 255 }
    },
    {
        "industrial_tunnel",
        "Industrial Tunnel",
        "Steel-toned passage lighting with amber service cues and signs.",
        { 46, 58, 54, 255 },
        { 210, 180, 96, 255 }
    },
    {
        "neon_maze",
        "Neon Maze",
        "Dark industrial travel with stronger neon highlights and brighter signage.",
        { 36, 22, 62, 255 },
        { 232, 74, 192, 255 }
    },
    {
        "dusty_ruin",
        "Dusty Ruin",
        "Warm rock and canyon tones with a quiet scenic horizon.",
        { 84, 62, 38, 255 },
        { 224, 164, 92, 255 }
    },
    {
        "quiet_night_run",
        "Quiet Night Run",
        "Dark blue night travel with soft forward movement and low contrast cues.",
        { 16, 26, 44, 255 },
        { 140, 170, 220, 255 }
    },
    {
        "eerie_corridor_watch",
        "Eerie Corridor Watch",
        "Near-black corridor palette with pale lab markers and subdued instrument glints.",
        { 28, 36, 54, 255 },
        { 174, 204, 228, 255 }
    },
    {
        "amber_service_drift",
        "Amber Service Drift",
        "Warm industrial service-light palette with darker steel shadows and calmer signage.",
        { 84, 70, 48, 255 },
        { 222, 176, 92, 255 }
    }
};

const screensave_theme_descriptor *explorer_get_themes(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_explorer_themes) / sizeof(g_explorer_themes[0]));
    }

    return g_explorer_themes;
}

const screensave_theme_descriptor *explorer_find_theme_descriptor(const char *theme_key)
{
    unsigned int count;
    const screensave_theme_descriptor *themes;

    themes = explorer_get_themes(&count);
    return screensave_find_theme(themes, count, theme_key);
}
