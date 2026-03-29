#include <string.h>

#include "gl_gallery_internal.h"

const screensave_preset_descriptor g_gl_gallery_presets[] = {
    {
        "compatibility_gallery",
        "Compatibility Gallery",
        "Baseline-safe gallery tour with restrained frames, orbits, and panels.",
        "compatibility_gallery",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x00001501UL
    },
    {
        "gl11_classic",
        "GL11 Classic",
        "Classic accelerated gallery look with balanced glow and panel motion.",
        "gl11_classic",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00001502UL
    },
    {
        "advanced_showcase",
        "Advanced Showcase",
        "Richest available gallery presentation with denser tier-aware layering.",
        "advanced_showcase",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00001503UL
    },
    {
        "neon_abstract",
        "Neon Abstract",
        "Pulsing abstract gallery study with brisk motion and saturated highlights.",
        "neon_abstract",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00001504UL
    },
    {
        "technical_exhibit",
        "Technical Exhibit",
        "Measured exhibit geometry with stronger line detail and calm pacing.",
        "technical_exhibit",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00001505UL
    },
    {
        "quiet_museum_renderer_tour",
        "Quiet Museum Renderer Tour",
        "Low-intensity gallery tour with subdued motion and conservative detail.",
        "quiet_museum_renderer_tour",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    }
};

static const gl_gallery_preset_values g_gl_gallery_preset_values[] = {
    { GL_GALLERY_SCENE_COMPATIBILITY, GL_GALLERY_MOTION_CALM },
    { GL_GALLERY_SCENE_COMPATIBILITY, GL_GALLERY_MOTION_STANDARD },
    { GL_GALLERY_SCENE_SHOWCASE, GL_GALLERY_MOTION_BRISK },
    { GL_GALLERY_SCENE_NEON, GL_GALLERY_MOTION_BRISK },
    { GL_GALLERY_SCENE_SHOWCASE, GL_GALLERY_MOTION_STANDARD },
    { GL_GALLERY_SCENE_COMPATIBILITY, GL_GALLERY_MOTION_CALM }
};

const screensave_preset_descriptor *gl_gallery_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(sizeof(g_gl_gallery_presets) / sizeof(g_gl_gallery_presets[0]));
    }

    return g_gl_gallery_presets;
}

const gl_gallery_preset_values *gl_gallery_find_preset_values(const char *preset_key)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    unsigned int index;

    presets = gl_gallery_get_presets(&preset_count);
    if (preset_key == NULL || preset_key[0] == '\0') {
        return NULL;
    }

    for (index = 0U; index < preset_count; ++index) {
        if (presets[index].preset_key != NULL && strcmp(presets[index].preset_key, preset_key) == 0) {
            return &g_gl_gallery_preset_values[index];
        }
    }

    return NULL;
}

const char *gl_gallery_scene_mode_name(int scene_mode)
{
    switch (scene_mode) {
    case GL_GALLERY_SCENE_NEON:
        return "neon";
    case GL_GALLERY_SCENE_SHOWCASE:
        return "showcase";
    case GL_GALLERY_SCENE_COMPATIBILITY:
    default:
        return "compatibility";
    }
}

const char *gl_gallery_motion_mode_name(int motion_mode)
{
    switch (motion_mode) {
    case GL_GALLERY_MOTION_CALM:
        return "calm";
    case GL_GALLERY_MOTION_BRISK:
        return "brisk";
    case GL_GALLERY_MOTION_STANDARD:
    default:
        return "standard";
    }
}

void gl_gallery_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    gl_gallery_config *product_config
)
{
    unsigned int preset_count;
    const screensave_preset_descriptor *presets;
    const screensave_preset_descriptor *preset_descriptor;
    const gl_gallery_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    presets = gl_gallery_get_presets(&preset_count);
    preset_descriptor = screensave_find_preset(presets, preset_count, preset_key);
    preset_values = gl_gallery_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->scene_mode = preset_values->scene_mode;
    product_config->motion_mode = preset_values->motion_mode;
}
