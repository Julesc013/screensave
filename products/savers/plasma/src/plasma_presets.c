#include <string.h>

#include "plasma_internal.h"

const char *plasma_canonical_content_key(const char *key)
{
    if (key != NULL && lstrcmpiA(key, "ember_lava") == 0) {
        return "plasma_lava";
    }

    return key;
}

const char *plasma_classic_canonical_key(const char *key)
{
    return plasma_canonical_content_key(key);
}

const screensave_preset_descriptor g_plasma_presets[] = {
    {
        "plasma_lava",
        "Plasma Lava",
        "Warm fire baseline with restrained diffusion, longer-held structure, and cleaner default contrast.",
        "plasma_lava",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        0,
        0UL
    },
    {
        "aurora_plasma",
        "Aurora Plasma",
        "Cool plasma study with explicit edge lift and a clearer high-detail crest profile.",
        "aurora_cool",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A11UL
    },
    {
        "ocean_interference",
        "Ocean Interference",
        "Layered interference field with crisper blue crest structure and less soft drift.",
        "oceanic_blue",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A12UL
    },
    {
        "museum_phosphor",
        "Museum Phosphor",
        "Measured green phosphor raster with clearer scanline structure and steadier museum-floor pacing.",
        "museum_phosphor",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        1,
        0x00000A13UL
    },
    {
        "quiet_darkroom",
        "Quiet Darkroom",
        "Subdued fire field with dry CRT falloff and calmer dark-room pacing for long sessions.",
        "quiet_darkroom",
        SCREENSAVE_DETAIL_LEVEL_LOW,
        0,
        0UL
    },
    {
        "midnight_interference",
        "Midnight Interference",
        "Dark interference field with sharper embossed relief and bounded accent lift.",
        "midnight_interference",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A14UL
    },
    {
        "amber_terminal",
        "Amber Terminal",
        "Amber phosphor plasma with bounded edge glow and warmer terminal highlight lift.",
        "amber_terminal",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000A15UL
    },
    {
        "lava_isolines",
        "Lava Isolines",
        "Warm fire-derived contour-band study that turns the classic lava field into explicit isolines.",
        "plasma_lava",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A20UL
    },
    {
        "aurora_bands",
        "Aurora Bands",
        "Cool aurora band study with softened glow so the banding reads as structure instead of a palette swap.",
        "aurora_cool",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A21UL
    },
    {
        "wire_glow",
        "Wire Glow",
        "Fast interference wire contours with bounded glow-edge emphasis.",
        "oceanic_blue",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A22UL
    },
    {
        "phosphor_topography",
        "Phosphor Topography",
        "Sharper monochrome contour-band study with explicit phosphor display emulation.",
        "museum_phosphor",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000A23UL
    },
    {
        "stipple_bands",
        "Stipple Bands",
        "Warm fine-band poster study filtered through a restrained halftone-stipple treatment.",
        "amber_terminal",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000A24UL
    },
    {
        "emboss_current",
        "Emboss Current",
        "Cool contour-band study with embossed edge shaping and restrained current-line lift.",
        "midnight_interference",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A25UL
    },
    {
        "crt_signal_bands",
        "CRT Signal Bands",
        "Amber interference bands with bounded CRT-style display emulation and accent lift.",
        "amber_terminal",
        SCREENSAVE_DETAIL_LEVEL_STANDARD,
        1,
        0x00000A26UL
    },
    {
        "ascii_reactor",
        "ASCII Reactor",
        "Chemical reactor study rendered through a denser amber ASCII glyph ramp.",
        "amber_terminal",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A30UL
    },
    {
        "matrix_lattice",
        "Matrix Lattice",
        "Structured lattice study rendered through phosphor-tinted columnar matrix flow.",
        "museum_phosphor",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A31UL
    },
    {
        "cellular_bloom",
        "Cellular Bloom",
        "Chemical bloom field with stronger glow lift and bounded non-literal cellular spread.",
        "aurora_cool",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A32UL
    },
    {
        "quasi_crystal_bands",
        "Quasi-Crystal Bands",
        "Measured lattice bands with embossed strata and restrained accent lift.",
        "midnight_interference",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A33UL
    },
    {
        "caustic_waterlight",
        "Caustic Waterlight",
        "Abstract refractive waterlight rendered as brighter caustic contour bands.",
        "oceanic_blue",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A34UL
    },
    {
        "aurora_curtain",
        "Aurora Curtain",
        "Cool curtain field study with bounded premium curtain lift and explicit accent separation.",
        "aurora_cool",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A40UL
    },
    {
        "ribbon_aurora",
        "Ribbon Aurora",
        "Layered aurora study folded into a softened ribbon presentation with deliberate non-scenic flow.",
        "midnight_interference",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A41UL
    },
    {
        "substrate_relief",
        "Substrate Relief",
        "Abstract substrate relief lifted into a bounded surface study with clearer edge accents.",
        "museum_phosphor",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A42UL
    },
    {
        "filament_extrusion",
        "Filament Extrusion",
        "Fast discharge-filament study rendered as lifted contour extrusion with explicit premium gating.",
        "amber_terminal",
        SCREENSAVE_DETAIL_LEVEL_HIGH,
        1,
        0x00000A43UL
    }
};

static const plasma_preset_values g_plasma_preset_values[] = {
    {
        PLASMA_EFFECT_FIRE,
        PLASMA_SPEED_GENTLE,
        PLASMA_RESOLUTION_STANDARD,
        PLASMA_SMOOTHING_SOFT,
        PLASMA_OUTPUT_FAMILY_RASTER,
        PLASMA_OUTPUT_MODE_NATIVE_RASTER,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_NONE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_NONE,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_PLASMA,
        PLASMA_SPEED_STANDARD,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_GLOW,
        PLASMA_OUTPUT_FAMILY_RASTER,
        PLASMA_OUTPUT_MODE_NATIVE_RASTER,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_GLOW_EDGE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_NONE,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_INTERFERENCE,
        PLASMA_SPEED_STANDARD,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_RASTER,
        PLASMA_OUTPUT_MODE_NATIVE_RASTER,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_NONE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_ACCENT_PASS,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_PLASMA,
        PLASMA_SPEED_GENTLE,
        PLASMA_RESOLUTION_STANDARD,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_RASTER,
        PLASMA_OUTPUT_MODE_NATIVE_RASTER,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_NONE,
        PLASMA_EMULATION_TREATMENT_PHOSPHOR,
        PLASMA_ACCENT_TREATMENT_NONE,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_FIRE,
        PLASMA_SPEED_GENTLE,
        PLASMA_RESOLUTION_COARSE,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_RASTER,
        PLASMA_OUTPUT_MODE_NATIVE_RASTER,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_NONE,
        PLASMA_EMULATION_TREATMENT_CRT,
        PLASMA_ACCENT_TREATMENT_NONE,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_INTERFERENCE,
        PLASMA_SPEED_STANDARD,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_CONTOUR,
        PLASMA_OUTPUT_MODE_CONTOUR_BANDS,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_EMBOSS_EDGE,
        PLASMA_EMULATION_TREATMENT_CRT,
        PLASMA_ACCENT_TREATMENT_ACCENT_PASS,
        PLASMA_PRESENTATION_MODE_HEIGHTFIELD
    },
    {
        PLASMA_EFFECT_PLASMA,
        PLASMA_SPEED_STANDARD,
        PLASMA_RESOLUTION_STANDARD,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_RASTER,
        PLASMA_OUTPUT_MODE_NATIVE_RASTER,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_GLOW_EDGE,
        PLASMA_EMULATION_TREATMENT_PHOSPHOR,
        PLASMA_ACCENT_TREATMENT_ACCENT_PASS,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_FIRE,
        PLASMA_SPEED_STANDARD,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_CONTOUR,
        PLASMA_OUTPUT_MODE_CONTOUR_BANDS,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_NONE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_NONE,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_AURORA,
        PLASMA_SPEED_STANDARD,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_SOFT,
        PLASMA_OUTPUT_FAMILY_BANDED,
        PLASMA_OUTPUT_MODE_POSTERIZED_BANDS,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_GLOW_EDGE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_NONE,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_INTERFERENCE,
        PLASMA_SPEED_STANDARD,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_CONTOUR,
        PLASMA_OUTPUT_MODE_CONTOUR_ONLY,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_GLOW_EDGE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_NONE,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_PLASMA,
        PLASMA_SPEED_GENTLE,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_CONTOUR,
        PLASMA_OUTPUT_MODE_CONTOUR_BANDS,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_NONE,
        PLASMA_EMULATION_TREATMENT_PHOSPHOR,
        PLASMA_ACCENT_TREATMENT_NONE,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_FIRE,
        PLASMA_SPEED_STANDARD,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_BANDED,
        PLASMA_OUTPUT_MODE_POSTERIZED_BANDS,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_HALFTONE_STIPPLE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_NONE,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_INTERFERENCE,
        PLASMA_SPEED_LIVELY,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_CONTOUR,
        PLASMA_OUTPUT_MODE_CONTOUR_BANDS,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_EMBOSS_EDGE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_ACCENT_PASS,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_INTERFERENCE,
        PLASMA_SPEED_STANDARD,
        PLASMA_RESOLUTION_STANDARD,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_BANDED,
        PLASMA_OUTPUT_MODE_POSTERIZED_BANDS,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_NONE,
        PLASMA_EMULATION_TREATMENT_CRT,
        PLASMA_ACCENT_TREATMENT_ACCENT_PASS,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_CHEMICAL,
        PLASMA_SPEED_STANDARD,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_GLYPH,
        PLASMA_OUTPUT_MODE_ASCII_GLYPH,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_NONE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_NONE,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_LATTICE,
        PLASMA_SPEED_STANDARD,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_GLYPH,
        PLASMA_OUTPUT_MODE_MATRIX_GLYPH,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_NONE,
        PLASMA_EMULATION_TREATMENT_PHOSPHOR,
        PLASMA_ACCENT_TREATMENT_NONE,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_CHEMICAL,
        PLASMA_SPEED_GENTLE,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_GLOW,
        PLASMA_OUTPUT_FAMILY_RASTER,
        PLASMA_OUTPUT_MODE_NATIVE_RASTER,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_GLOW_EDGE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_NONE,
        PLASMA_PRESENTATION_MODE_HEIGHTFIELD
    },
    {
        PLASMA_EFFECT_LATTICE,
        PLASMA_SPEED_GENTLE,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_BANDED,
        PLASMA_OUTPUT_MODE_POSTERIZED_BANDS,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_EMBOSS_EDGE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_ACCENT_PASS,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_CAUSTIC,
        PLASMA_SPEED_GENTLE,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_GLOW,
        PLASMA_OUTPUT_FAMILY_CONTOUR,
        PLASMA_OUTPUT_MODE_CONTOUR_BANDS,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_GLOW_EDGE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_NONE,
        PLASMA_PRESENTATION_MODE_FLAT
    },
    {
        PLASMA_EFFECT_AURORA,
        PLASMA_SPEED_GENTLE,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_SOFT,
        PLASMA_OUTPUT_FAMILY_RASTER,
        PLASMA_OUTPUT_MODE_NATIVE_RASTER,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_NONE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_ACCENT_PASS,
        PLASMA_PRESENTATION_MODE_CURTAIN
    },
    {
        PLASMA_EFFECT_AURORA,
        PLASMA_SPEED_STANDARD,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_SOFT,
        PLASMA_OUTPUT_FAMILY_BANDED,
        PLASMA_OUTPUT_MODE_POSTERIZED_BANDS,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_NONE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_ACCENT_PASS,
        PLASMA_PRESENTATION_MODE_RIBBON
    },
    {
        PLASMA_EFFECT_SUBSTRATE,
        PLASMA_SPEED_GENTLE,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_SOFT,
        PLASMA_OUTPUT_FAMILY_RASTER,
        PLASMA_OUTPUT_MODE_NATIVE_RASTER,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_EMBOSS_EDGE,
        PLASMA_EMULATION_TREATMENT_PHOSPHOR,
        PLASMA_ACCENT_TREATMENT_ACCENT_PASS,
        PLASMA_PRESENTATION_MODE_BOUNDED_SURFACE
    },
    {
        PLASMA_EFFECT_ARC,
        PLASMA_SPEED_LIVELY,
        PLASMA_RESOLUTION_FINE,
        PLASMA_SMOOTHING_OFF,
        PLASMA_OUTPUT_FAMILY_CONTOUR,
        PLASMA_OUTPUT_MODE_CONTOUR_BANDS,
        PLASMA_SAMPLING_TREATMENT_NONE,
        PLASMA_FILTER_TREATMENT_GLOW_EDGE,
        PLASMA_EMULATION_TREATMENT_NONE,
        PLASMA_ACCENT_TREATMENT_ACCENT_PASS,
        PLASMA_PRESENTATION_MODE_CONTOUR_EXTRUSION
    }
};

const screensave_preset_descriptor *plasma_get_presets(unsigned int *count_out)
{
    if (count_out != NULL) {
        *count_out = plasma_preset_count();
    }

    return g_plasma_presets;
}

unsigned int plasma_preset_count(void)
{
    return (unsigned int)(sizeof(g_plasma_presets) / sizeof(g_plasma_presets[0]));
}

unsigned int plasma_classic_preset_count(void)
{
    return plasma_preset_count();
}

const screensave_preset_descriptor *plasma_find_preset_descriptor(const char *preset_key)
{
    const plasma_content_preset_entry *entry;

    entry = plasma_content_find_preset_entry(preset_key);
    return entry != NULL ? entry->descriptor : NULL;
}

int plasma_is_known_preset_key(const char *preset_key)
{
    return plasma_find_preset_descriptor(preset_key) != NULL;
}

int plasma_classic_is_known_preset_key(const char *preset_key)
{
    return plasma_is_known_preset_key(preset_key);
}

const plasma_preset_values *plasma_find_preset_values(const char *preset_key)
{
    const screensave_preset_descriptor *preset_descriptor;
    unsigned int index;

    preset_descriptor = plasma_find_preset_descriptor(preset_key);
    if (preset_descriptor == NULL) {
        return NULL;
    }

    for (index = 0U; index < plasma_preset_count(); ++index) {
        if (&g_plasma_presets[index] == preset_descriptor) {
            return &g_plasma_preset_values[index];
        }
    }

    return NULL;
}

const char *plasma_effect_mode_name(int effect_mode)
{
    switch (effect_mode) {
    case PLASMA_EFFECT_FIRE:
        return "fire";

    case PLASMA_EFFECT_INTERFERENCE:
        return "interference";

    case PLASMA_EFFECT_CHEMICAL:
        return "chemical_cellular_growth";

    case PLASMA_EFFECT_LATTICE:
        return "lattice_quasi_crystal";

    case PLASMA_EFFECT_CAUSTIC:
        return "caustic_marbling";

    case PLASMA_EFFECT_AURORA:
        return "aurora_curtain_ribbon";

    case PLASMA_EFFECT_SUBSTRATE:
        return "substrate_vein_coral";

    case PLASMA_EFFECT_ARC:
        return "arc_discharge";

    case PLASMA_EFFECT_PLASMA:
    default:
        return "plasma";
    }
}

const char *plasma_speed_mode_name(int speed_mode)
{
    switch (speed_mode) {
    case PLASMA_SPEED_GENTLE:
        return "gentle";

    case PLASMA_SPEED_LIVELY:
        return "lively";

    case PLASMA_SPEED_STANDARD:
    default:
        return "standard";
    }
}

const char *plasma_resolution_mode_name(int resolution_mode)
{
    switch (resolution_mode) {
    case PLASMA_RESOLUTION_COARSE:
        return "coarse";

    case PLASMA_RESOLUTION_FINE:
        return "fine";

    case PLASMA_RESOLUTION_STANDARD:
    default:
        return "standard";
    }
}

const char *plasma_smoothing_mode_name(int smoothing_mode)
{
    switch (smoothing_mode) {
    case PLASMA_SMOOTHING_OFF:
        return "off";

    case PLASMA_SMOOTHING_GLOW:
        return "glow";

    case PLASMA_SMOOTHING_SOFT:
    default:
        return "soft";
    }
}

void plasma_apply_preset_bundle_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    plasma_config *product_config
)
{
    const screensave_preset_descriptor *preset_descriptor;
    const plasma_preset_values *preset_values;

    if (common_config == NULL || product_config == NULL) {
        return;
    }

    preset_descriptor = plasma_find_preset_descriptor(preset_key);
    preset_values = plasma_find_preset_values(preset_key);
    if (preset_descriptor == NULL || preset_values == NULL) {
        return;
    }

    common_config->preset_key = preset_descriptor->preset_key;
    common_config->theme_key = preset_descriptor->theme_key;
    common_config->detail_level = preset_descriptor->detail_level;
    common_config->use_deterministic_seed = preset_descriptor->use_fixed_seed;
    common_config->deterministic_seed = preset_descriptor->fixed_seed;

    product_config->effect_mode = preset_values->effect_mode;
    product_config->speed_mode = preset_values->speed_mode;
    product_config->resolution_mode = preset_values->resolution_mode;
    product_config->smoothing_mode = preset_values->smoothing_mode;
    product_config->output_family = preset_values->output_family;
    product_config->output_mode = preset_values->output_mode;
    product_config->sampling_treatment = preset_values->sampling_treatment;
    product_config->filter_treatment = preset_values->filter_treatment;
    product_config->emulation_treatment = preset_values->emulation_treatment;
    product_config->accent_treatment = preset_values->accent_treatment;
    product_config->presentation_mode = preset_values->presentation_mode;
}

void plasma_apply_preset_to_config(
    const char *preset_key,
    screensave_common_config *common_config,
    plasma_config *product_config
)
{
    plasma_apply_preset_bundle_to_config(preset_key, common_config, product_config);
}
