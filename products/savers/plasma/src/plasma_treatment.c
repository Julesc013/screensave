#include "plasma_internal.h"

static screensave_color plasma_treatment_background_color(void)
{
    screensave_color color;

    color.red = 0;
    color.green = 0;
    color.blue = 0;
    color.alpha = 255;
    return color;
}

static unsigned char plasma_treatment_clamp_channel(unsigned int value)
{
    if (value > 255U) {
        return 255U;
    }

    return (unsigned char)value;
}

static unsigned int plasma_treatment_abs_int(int value)
{
    return (unsigned int)(value < 0 ? -value : value);
}

static unsigned char plasma_treatment_sample_scalar(
    const plasma_output_frame *output,
    int x,
    int y
)
{
    int width;
    int height;

    if (
        output == NULL ||
        output->scalar_values == NULL ||
        output->size.width <= 0 ||
        output->size.height <= 0
    ) {
        return 0U;
    }

    width = output->size.width;
    height = output->size.height;
    if (x < 0) {
        x = 0;
    } else if (x >= width) {
        x = width - 1;
    }
    if (y < 0) {
        y = 0;
    } else if (y >= height) {
        y = height - 1;
    }

    return output->scalar_values[(y * width) + x];
}

static unsigned int plasma_treatment_max3(
    unsigned int first,
    unsigned int second,
    unsigned int third
)
{
    unsigned int value;

    value = first;
    if (second > value) {
        value = second;
    }
    if (third > value) {
        value = third;
    }

    return value;
}

static screensave_color plasma_treatment_palette_color(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    unsigned int value
)
{
    screensave_color base_color;
    screensave_color white_color;
    screensave_color highlight_color;
    screensave_color primary_color;
    screensave_color accent_color;
    unsigned int palette_index;
    unsigned int amount;

    base_color = plasma_treatment_background_color();
    white_color.red = 255;
    white_color.green = 255;
    white_color.blue = 255;
    white_color.alpha = 255;
    plasma_transition_resolve_theme_colors(plan, state, &primary_color, &accent_color);
    highlight_color = screensave_color_lerp(accent_color, white_color, 112U);

    palette_index = (value + (unsigned int)(state->palette_phase & 255UL)) & 255U;
    if (plan->effect_mode == PLASMA_EFFECT_FIRE) {
        palette_index = (value + (unsigned int)((state->palette_phase / 2UL) & 255UL)) & 255U;
        if (palette_index < 64U) {
            amount = (palette_index * 255U) / 64U;
            return screensave_color_lerp(base_color, primary_color, amount);
        }
        if (palette_index < 176U) {
            amount = ((palette_index - 64U) * 255U) / 112U;
            return screensave_color_lerp(primary_color, accent_color, amount);
        }

        amount = ((palette_index - 176U) * 255U) / 79U;
        return screensave_color_lerp(accent_color, highlight_color, amount);
    }

    if (plan->effect_mode == PLASMA_EFFECT_INTERFERENCE) {
        if (palette_index < 112U) {
            amount = (palette_index * 255U) / 112U;
            return screensave_color_lerp(base_color, accent_color, amount);
        }
        if (palette_index < 208U) {
            amount = ((palette_index - 112U) * 255U) / 96U;
            return screensave_color_lerp(accent_color, primary_color, amount);
        }

        amount = ((palette_index - 208U) * 255U) / 47U;
        return screensave_color_lerp(primary_color, highlight_color, amount);
    }

    if (palette_index < 96U) {
        amount = (palette_index * 255U) / 96U;
        return screensave_color_lerp(base_color, primary_color, amount);
    }
    if (palette_index < 192U) {
        amount = ((palette_index - 96U) * 255U) / 96U;
        return screensave_color_lerp(primary_color, accent_color, amount);
    }

    amount = ((palette_index - 192U) * 255U) / 63U;
    return screensave_color_lerp(accent_color, highlight_color, amount);
}

static unsigned int plasma_treatment_band_count(const struct plasma_plan_tag *plan)
{
    unsigned int band_count;

    if (plan == NULL) {
        return 6U;
    }

    band_count = 5U;
    if (plan->output_mode == PLASMA_OUTPUT_MODE_CONTOUR_ONLY) {
        band_count = 10U;
    } else if (plan->output_mode == PLASMA_OUTPUT_MODE_CONTOUR_BANDS) {
        band_count = 7U;
    }

    if (plan->effect_mode == PLASMA_EFFECT_INTERFERENCE && band_count < 10U) {
        band_count += 1U;
    }
    if (plan->resolution_mode == PLASMA_RESOLUTION_COARSE && band_count > 4U) {
        band_count -= 1U;
    } else if (plan->resolution_mode == PLASMA_RESOLUTION_FINE && band_count < 10U) {
        band_count += 1U;
    }

    if (band_count < 4U) {
        band_count = 4U;
    } else if (band_count > 10U) {
        band_count = 10U;
    }

    return band_count;
}

static unsigned int plasma_treatment_band_index(
    unsigned int value,
    unsigned int band_count
)
{
    if (band_count == 0U) {
        return 0U;
    }

    return (value * band_count) / 256U;
}

static unsigned int plasma_treatment_band_value(
    unsigned int band_index,
    unsigned int band_count
)
{
    if (band_count <= 1U) {
        return 0U;
    }
    if (band_index >= band_count) {
        band_index = band_count - 1U;
    }

    return (band_index * 255U) / (band_count - 1U);
}

static int plasma_treatment_is_contour_edge(
    const plasma_output_frame *output,
    int x,
    int y,
    unsigned int band_count
)
{
    unsigned int center_index;

    center_index = plasma_treatment_band_index(
        (unsigned int)plasma_treatment_sample_scalar(output, x, y),
        band_count
    );

    return
        center_index != plasma_treatment_band_index(
            (unsigned int)plasma_treatment_sample_scalar(output, x - 1, y),
            band_count
        ) ||
        center_index != plasma_treatment_band_index(
            (unsigned int)plasma_treatment_sample_scalar(output, x + 1, y),
            band_count
        ) ||
        center_index != plasma_treatment_band_index(
            (unsigned int)plasma_treatment_sample_scalar(output, x, y - 1),
            band_count
        ) ||
        center_index != plasma_treatment_band_index(
            (unsigned int)plasma_treatment_sample_scalar(output, x, y + 1),
            band_count
        );
}

typedef struct plasma_glyph_pattern_tag {
    unsigned char rows[6];
} plasma_glyph_pattern;

static const plasma_glyph_pattern g_plasma_ascii_patterns[] = {
    { { 0x0U, 0x0U, 0x0U, 0x0U, 0x0U, 0x0U } },
    { { 0x0U, 0x0U, 0x0U, 0x0U, 0x6U, 0x6U } },
    { { 0x0U, 0x6U, 0x6U, 0x0U, 0x6U, 0x6U } },
    { { 0x0U, 0x0U, 0xFU, 0xFU, 0x0U, 0x0U } },
    { { 0x0U, 0xFU, 0x0U, 0xFU, 0x0U, 0x0U } },
    { { 0x2U, 0x2U, 0xFU, 0xFU, 0x2U, 0x2U } },
    { { 0x9U, 0x6U, 0xFU, 0xFU, 0x6U, 0x9U } },
    { { 0xAU, 0xFU, 0xAU, 0xFU, 0xAU, 0x0U } },
    { { 0x9U, 0x1U, 0x2U, 0x4U, 0x8U, 0x9U } },
    { { 0x6U, 0x9U, 0xFU, 0xDU, 0x8U, 0x7U } }
};

static const plasma_glyph_pattern g_plasma_matrix_patterns[] = {
    { { 0x6U, 0x9U, 0x9U, 0x9U, 0x9U, 0x6U } },
    { { 0x2U, 0x6U, 0x2U, 0x2U, 0x2U, 0x7U } },
    { { 0x6U, 0x9U, 0x1U, 0x2U, 0x4U, 0xFU } },
    { { 0xEU, 0x1U, 0x6U, 0x1U, 0x9U, 0x6U } },
    { { 0x2U, 0x6U, 0xAU, 0xFU, 0x2U, 0x2U } },
    { { 0xFU, 0x8U, 0xEU, 0x1U, 0x9U, 0x6U } },
    { { 0x7U, 0x8U, 0xEU, 0x9U, 0x9U, 0x6U } },
    { { 0xFU, 0x1U, 0x2U, 0x4U, 0x4U, 0x4U } }
};

static unsigned int plasma_treatment_glyph_cell_width(plasma_output_mode mode)
{
    if (mode == PLASMA_OUTPUT_MODE_MATRIX_GLYPH) {
        return 4U;
    }

    return 4U;
}

static unsigned int plasma_treatment_glyph_cell_height(plasma_output_mode mode)
{
    if (mode == PLASMA_OUTPUT_MODE_MATRIX_GLYPH) {
        return 6U;
    }

    return 6U;
}

static unsigned int plasma_treatment_sample_cell_scalar(
    const plasma_output_frame *output,
    unsigned int cell_x,
    unsigned int cell_y,
    unsigned int cell_width,
    unsigned int cell_height
)
{
    int sample_x;
    int sample_y;

    sample_x = (int)(cell_x * cell_width) + (int)(cell_width / 2U);
    sample_y = (int)(cell_y * cell_height) + (int)(cell_height / 2U);
    return (unsigned int)plasma_treatment_sample_scalar(output, sample_x, sample_y);
}

static int plasma_treatment_cell_has_contour(
    const plasma_output_frame *output,
    unsigned int cell_x,
    unsigned int cell_y,
    unsigned int cell_width,
    unsigned int cell_height,
    unsigned int band_count
)
{
    int sample_x;
    int sample_y;

    sample_x = (int)(cell_x * cell_width) + (int)(cell_width / 2U);
    sample_y = (int)(cell_y * cell_height) + (int)(cell_height / 2U);
    return plasma_treatment_is_contour_edge(output, sample_x, sample_y, band_count);
}

static screensave_color plasma_treatment_scale_color(
    screensave_color color,
    unsigned int amount
)
{
    if (amount > 255U) {
        amount = 255U;
    }

    return screensave_color_lerp(plasma_treatment_background_color(), color, amount);
}

static int plasma_treatment_pattern_bit(
    const plasma_glyph_pattern *pattern,
    unsigned int column,
    unsigned int row
)
{
    unsigned int bit_index;

    if (pattern == NULL || column >= 4U || row >= 6U) {
        return 0;
    }

    bit_index = 3U - column;
    return (pattern->rows[row] & (unsigned char)(1U << bit_index)) != 0U;
}

static const plasma_glyph_pattern *plasma_treatment_select_ascii_pattern(
    unsigned int value,
    int contour_edge
)
{
    unsigned int count;
    unsigned int index;

    count = (unsigned int)(sizeof(g_plasma_ascii_patterns) / sizeof(g_plasma_ascii_patterns[0]));
    if (value < 10U) {
        return &g_plasma_ascii_patterns[0];
    }

    index = (value * (count - 1U)) / 255U;
    if (contour_edge && index + 1U < count) {
        ++index;
    }

    return &g_plasma_ascii_patterns[index];
}

static const plasma_glyph_pattern *plasma_treatment_select_matrix_pattern(
    unsigned int value,
    unsigned int cell_x,
    unsigned int cell_y,
    const struct plasma_execution_state_tag *state
)
{
    unsigned int count;
    unsigned int index;
    unsigned long phase_step;

    count = (unsigned int)(sizeof(g_plasma_matrix_patterns) / sizeof(g_plasma_matrix_patterns[0]));
    phase_step = state != NULL ? (state->phase_millis / 43UL) : 0UL;
    index = (
        value +
        (cell_x * 17U) +
        (cell_y * 11U) +
        (unsigned int)(phase_step & 255UL)
    ) % count;
    return &g_plasma_matrix_patterns[index];
}

static unsigned int plasma_treatment_matrix_trail_strength(
    const struct plasma_execution_state_tag *state,
    unsigned int cell_x,
    unsigned int cell_y,
    unsigned int cell_rows
)
{
    unsigned int cycle;
    unsigned int head_row;
    unsigned int distance;
    unsigned long phase_step;

    if (state == NULL || cell_rows == 0U) {
        return 0U;
    }

    cycle = cell_rows + 9U;
    phase_step = state->phase_millis / 37UL;
    head_row = (unsigned int)(
        (phase_step + (unsigned long)(cell_x * 3U) + (state->source_phase_a & 7UL)) %
        (unsigned long)cycle
    );
    if (head_row >= cell_rows || cell_y > head_row) {
        return 0U;
    }

    distance = head_row - cell_y;
    if (distance == 0U) {
        return 255U;
    }
    if (distance > 6U) {
        return 0U;
    }

    return 224U - (distance * 28U);
}

static screensave_color plasma_treatment_ascii_color(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    unsigned int value,
    int contour_edge
)
{
    screensave_color color;
    screensave_color white_color;

    color = plasma_treatment_palette_color(plan, state, value);
    if (contour_edge) {
        white_color.red = 255;
        white_color.green = 255;
        white_color.blue = 255;
        white_color.alpha = 255;
        color = screensave_color_lerp(color, white_color, 84U);
    }

    return plasma_treatment_scale_color(color, 72U + ((value * 160U) / 255U));
}

static screensave_color plasma_treatment_matrix_color(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    unsigned int value,
    unsigned int trail_strength
)
{
    screensave_color color;
    screensave_color white_color;

    color = plasma_treatment_palette_color(plan, state, value);
    color = screensave_color_lerp(plan->theme->primary_color, color, 160U);

    white_color.red = 255;
    white_color.green = 255;
    white_color.blue = 255;
    white_color.alpha = 255;
    if (trail_strength >= 240U) {
        color = screensave_color_lerp(color, white_color, 112U);
    } else {
        color = screensave_color_lerp(color, plan->theme->accent_color, 56U);
    }

    return plasma_treatment_scale_color(color, trail_strength);
}

static int plasma_theme_map_output(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    const plasma_output_frame *output,
    screensave_visual_buffer *visual_buffer
)
{
    screensave_color background_color;
    screensave_color white_color;
    unsigned int band_count;
    int x;
    int y;

    if (
        plan == NULL ||
        state == NULL ||
        output == NULL ||
        visual_buffer == NULL ||
        plan->theme == NULL ||
        output->scalar_values == NULL ||
        visual_buffer->pixels == NULL
    ) {
        return 0;
    }

    if (
        visual_buffer->size.width != output->size.width ||
        visual_buffer->size.height != output->size.height
    ) {
        return 0;
    }

    background_color = plasma_treatment_background_color();
    white_color.red = 255;
    white_color.green = 255;
    white_color.blue = 255;
    white_color.alpha = 255;
    band_count = plasma_treatment_band_count(plan);

    for (y = 0; y < output->size.height; ++y) {
        unsigned char *row;

        row = visual_buffer->pixels + ((size_t)y * (size_t)visual_buffer->stride_bytes);
        for (x = 0; x < output->size.width; ++x) {
            unsigned int value;
            unsigned int band_index;
            unsigned int band_value;
            int contour_edge;
            screensave_color color;

            value = (unsigned int)plasma_treatment_sample_scalar(output, x, y);
            band_index = plasma_treatment_band_index(value, band_count);
            band_value = plasma_treatment_band_value(band_index, band_count);
            contour_edge = plasma_treatment_is_contour_edge(output, x, y, band_count);

            if (
                output->family == PLASMA_OUTPUT_FAMILY_RASTER &&
                output->mode == PLASMA_OUTPUT_MODE_NATIVE_RASTER
            ) {
                color = plasma_treatment_palette_color(plan, state, value);
            } else if (
                output->family == PLASMA_OUTPUT_FAMILY_BANDED &&
                output->mode == PLASMA_OUTPUT_MODE_POSTERIZED_BANDS
            ) {
                color = plasma_treatment_palette_color(plan, state, band_value);
                if ((band_index & 1U) != 0U) {
                    color = screensave_color_lerp(color, plan->theme->accent_color, 112U);
                } else {
                    color = screensave_color_lerp(color, background_color, 28U);
                }
            } else if (
                output->family == PLASMA_OUTPUT_FAMILY_CONTOUR &&
                output->mode == PLASMA_OUTPUT_MODE_CONTOUR_ONLY
            ) {
                if (contour_edge) {
                    color = screensave_color_lerp(plan->theme->accent_color, white_color, 160U);
                } else {
                    color = background_color;
                }
            } else if (
                output->family == PLASMA_OUTPUT_FAMILY_CONTOUR &&
                output->mode == PLASMA_OUTPUT_MODE_CONTOUR_BANDS
            ) {
                color = plasma_treatment_palette_color(plan, state, band_value);
                if (contour_edge) {
                    color = screensave_color_lerp(plan->theme->accent_color, white_color, 136U);
                } else if ((band_index & 1U) != 0U) {
                    color = screensave_color_lerp(color, plan->theme->accent_color, 72U);
                } else {
                    color = screensave_color_lerp(color, background_color, 18U);
                }
            } else if (
                output->family == PLASMA_OUTPUT_FAMILY_GLYPH &&
                (
                    output->mode == PLASMA_OUTPUT_MODE_ASCII_GLYPH ||
                    output->mode == PLASMA_OUTPUT_MODE_MATRIX_GLYPH
                )
            ) {
                unsigned int cell_width;
                unsigned int cell_height;
                unsigned int cell_x;
                unsigned int cell_y;
                unsigned int cell_value;
                unsigned int local_x;
                unsigned int local_y;
                const plasma_glyph_pattern *pattern;

                cell_width = plasma_treatment_glyph_cell_width(output->mode);
                cell_height = plasma_treatment_glyph_cell_height(output->mode);
                cell_x = (unsigned int)x / cell_width;
                cell_y = (unsigned int)y / cell_height;
                local_x = (unsigned int)x % cell_width;
                local_y = (unsigned int)y % cell_height;
                cell_value = plasma_treatment_sample_cell_scalar(
                    output,
                    cell_x,
                    cell_y,
                    cell_width,
                    cell_height
                );

                if (output->mode == PLASMA_OUTPUT_MODE_ASCII_GLYPH) {
                    int cell_contour;

                    cell_contour = plasma_treatment_cell_has_contour(
                        output,
                        cell_x,
                        cell_y,
                        cell_width,
                        cell_height,
                        band_count
                    );
                    pattern = plasma_treatment_select_ascii_pattern(cell_value, cell_contour);
                    if (plasma_treatment_pattern_bit(pattern, local_x, local_y)) {
                        color = plasma_treatment_ascii_color(plan, state, cell_value, cell_contour);
                    } else {
                        color = background_color;
                    }
                } else {
                    unsigned int cell_rows;
                    unsigned int trail_strength;

                    cell_rows = (unsigned int)(
                        (output->size.height + (int)cell_height - 1) / (int)cell_height
                    );
                    trail_strength = plasma_treatment_matrix_trail_strength(
                        state,
                        cell_x,
                        cell_y,
                        cell_rows
                    );
                    pattern = plasma_treatment_select_matrix_pattern(cell_value, cell_x, cell_y, state);
                    if (
                        trail_strength > 0U &&
                        cell_value >= 24U &&
                        plasma_treatment_pattern_bit(pattern, local_x, local_y)
                    ) {
                        color = plasma_treatment_matrix_color(plan, state, cell_value, trail_strength);
                    } else {
                        color = background_color;
                    }
                }
            } else {
                return 0;
            }

            row[(x * 4) + 0] = color.blue;
            row[(x * 4) + 1] = color.green;
            row[(x * 4) + 2] = color.red;
            row[(x * 4) + 3] = 255U;
        }
    }

    return 1;
}

int plasma_sampling_treatment_is_supported(plasma_sampling_treatment treatment)
{
    return treatment == PLASMA_SAMPLING_TREATMENT_NONE;
}

int plasma_filter_treatment_is_supported(plasma_filter_treatment treatment)
{
    return
        treatment == PLASMA_FILTER_TREATMENT_NONE ||
        treatment == PLASMA_FILTER_TREATMENT_BLUR ||
        treatment == PLASMA_FILTER_TREATMENT_GLOW_EDGE ||
        treatment == PLASMA_FILTER_TREATMENT_HALFTONE_STIPPLE ||
        treatment == PLASMA_FILTER_TREATMENT_EMBOSS_EDGE;
}

int plasma_filter_treatment_requires_advanced(plasma_filter_treatment treatment)
{
    return treatment == PLASMA_FILTER_TREATMENT_BLUR;
}

int plasma_emulation_treatment_is_supported(plasma_emulation_treatment treatment)
{
    return
        treatment == PLASMA_EMULATION_TREATMENT_NONE ||
        treatment == PLASMA_EMULATION_TREATMENT_PHOSPHOR ||
        treatment == PLASMA_EMULATION_TREATMENT_CRT;
}

int plasma_accent_treatment_is_supported(plasma_accent_treatment treatment)
{
    return
        treatment == PLASMA_ACCENT_TREATMENT_NONE ||
        treatment == PLASMA_ACCENT_TREATMENT_OVERLAY_PASS ||
        treatment == PLASMA_ACCENT_TREATMENT_ACCENT_PASS;
}

int plasma_accent_treatment_requires_advanced(plasma_accent_treatment treatment)
{
    return treatment == PLASMA_ACCENT_TREATMENT_OVERLAY_PASS;
}

const char *plasma_sampling_treatment_token(plasma_sampling_treatment treatment)
{
    switch (treatment) {
    case PLASMA_SAMPLING_TREATMENT_NONE:
        return "none";

    default:
        return "unsupported";
    }
}

const char *plasma_filter_treatment_token(plasma_filter_treatment treatment)
{
    switch (treatment) {
    case PLASMA_FILTER_TREATMENT_NONE:
        return "none";

    case PLASMA_FILTER_TREATMENT_BLUR:
        return "blur";

    case PLASMA_FILTER_TREATMENT_GLOW_EDGE:
        return "glow_edge";

    case PLASMA_FILTER_TREATMENT_HALFTONE_STIPPLE:
        return "halftone_stipple";

    case PLASMA_FILTER_TREATMENT_EMBOSS_EDGE:
        return "emboss_edge";

    default:
        return "unsupported";
    }
}

const char *plasma_emulation_treatment_token(plasma_emulation_treatment treatment)
{
    switch (treatment) {
    case PLASMA_EMULATION_TREATMENT_NONE:
        return "none";

    case PLASMA_EMULATION_TREATMENT_PHOSPHOR:
        return "phosphor";

    case PLASMA_EMULATION_TREATMENT_CRT:
        return "crt";

    default:
        return "unsupported";
    }
}

const char *plasma_accent_treatment_token(plasma_accent_treatment treatment)
{
    switch (treatment) {
    case PLASMA_ACCENT_TREATMENT_NONE:
        return "none";

    case PLASMA_ACCENT_TREATMENT_OVERLAY_PASS:
        return "overlay_pass";

    case PLASMA_ACCENT_TREATMENT_ACCENT_PASS:
        return "accent_pass";

    default:
        return "unsupported";
    }
}

static int plasma_apply_sampling_treatment(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    screensave_visual_buffer *visual_buffer
)
{
    (void)state;
    (void)visual_buffer;

    if (plan == NULL) {
        return 0;
    }

    return plan->sampling_treatment == PLASMA_SAMPLING_TREATMENT_NONE;
}

static int plasma_apply_glow_edge_filter(
    const struct plasma_plan_tag *plan,
    const plasma_output_frame *output,
    screensave_visual_buffer *visual_buffer
)
{
    int x;
    int y;

    if (
        plan == NULL ||
        output == NULL ||
        visual_buffer == NULL ||
        plan->theme == NULL ||
        visual_buffer->pixels == NULL
    ) {
        return 0;
    }

    for (y = 0; y < visual_buffer->size.height; ++y) {
        unsigned char *row;

        row = visual_buffer->pixels + ((size_t)y * (size_t)visual_buffer->stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            int x_gradient;
            int y_gradient;
            unsigned int intensity;
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;

            x_gradient =
                (int)plasma_treatment_sample_scalar(output, x + 1, y) -
                (int)plasma_treatment_sample_scalar(output, x - 1, y);
            y_gradient =
                (int)plasma_treatment_sample_scalar(output, x, y + 1) -
                (int)plasma_treatment_sample_scalar(output, x, y - 1);
            intensity = plasma_treatment_abs_int(x_gradient) + plasma_treatment_abs_int(y_gradient);
            if (intensity > 255U) {
                intensity = 255U;
            }

            blue_value =
                (unsigned int)row[(x * 4) + 0] +
                ((((unsigned int)plan->theme->accent_color.blue * intensity) / 255U) * 3U) / 4U;
            green_value =
                (unsigned int)row[(x * 4) + 1] +
                ((((unsigned int)plan->theme->accent_color.green * intensity) / 255U) * 3U) / 4U;
            red_value =
                (unsigned int)row[(x * 4) + 2] +
                ((((unsigned int)plan->theme->accent_color.red * intensity) / 255U) * 3U) / 4U;

            row[(x * 4) + 0] = plasma_treatment_clamp_channel(blue_value);
            row[(x * 4) + 1] = plasma_treatment_clamp_channel(green_value);
            row[(x * 4) + 2] = plasma_treatment_clamp_channel(red_value);
            row[(x * 4) + 3] = 255U;
        }
    }

    return 1;
}

static int plasma_apply_halftone_stipple_filter(
    const plasma_output_frame *output,
    screensave_visual_buffer *visual_buffer
)
{
    static const unsigned int g_thresholds[4] = { 48U, 168U, 112U, 224U };
    int x;
    int y;

    if (
        output == NULL ||
        visual_buffer == NULL ||
        visual_buffer->pixels == NULL
    ) {
        return 0;
    }

    for (y = 0; y < visual_buffer->size.height; ++y) {
        unsigned char *row;

        row = visual_buffer->pixels + ((size_t)y * (size_t)visual_buffer->stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            unsigned int scalar_value;
            unsigned int threshold;
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;

            scalar_value = (unsigned int)plasma_treatment_sample_scalar(output, x, y);
            threshold = g_thresholds[((unsigned int)(y & 1) * 2U) + (unsigned int)(x & 1)];
            if (scalar_value < threshold) {
                blue_value = ((unsigned int)row[(x * 4) + 0] * 28U) / 255U;
                green_value = ((unsigned int)row[(x * 4) + 1] * 28U) / 255U;
                red_value = ((unsigned int)row[(x * 4) + 2] * 28U) / 255U;
                row[(x * 4) + 0] = (unsigned char)blue_value;
                row[(x * 4) + 1] = (unsigned char)green_value;
                row[(x * 4) + 2] = (unsigned char)red_value;
                row[(x * 4) + 3] = 255U;
            }
        }
    }

    return 1;
}

static int plasma_apply_emboss_edge_filter(
    const plasma_output_frame *output,
    screensave_visual_buffer *visual_buffer
)
{
    int x;
    int y;

    if (
        output == NULL ||
        visual_buffer == NULL ||
        visual_buffer->pixels == NULL
    ) {
        return 0;
    }

    for (y = 0; y < visual_buffer->size.height; ++y) {
        unsigned char *row;

        row = visual_buffer->pixels + ((size_t)y * (size_t)visual_buffer->stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            int x_gradient;
            int y_gradient;
            int shade;
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;

            x_gradient =
                (int)plasma_treatment_sample_scalar(output, x + 1, y) -
                (int)plasma_treatment_sample_scalar(output, x - 1, y);
            y_gradient =
                (int)plasma_treatment_sample_scalar(output, x, y + 1) -
                (int)plasma_treatment_sample_scalar(output, x, y - 1);
            shade = 144 + ((x_gradient + y_gradient) / 2);
            if (shade < 16) {
                shade = 16;
            } else if (shade > 248) {
                shade = 248;
            }

            blue_value = ((unsigned int)row[(x * 4) + 0] * (unsigned int)shade) / 128U;
            green_value = ((unsigned int)row[(x * 4) + 1] * (unsigned int)shade) / 128U;
            red_value = ((unsigned int)row[(x * 4) + 2] * (unsigned int)shade) / 128U;
            row[(x * 4) + 0] = plasma_treatment_clamp_channel(blue_value);
            row[(x * 4) + 1] = plasma_treatment_clamp_channel(green_value);
            row[(x * 4) + 2] = plasma_treatment_clamp_channel(red_value);
            row[(x * 4) + 3] = 255U;
        }
    }

    return 1;
}

static int plasma_apply_filter_treatment(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    const plasma_output_frame *output,
    screensave_visual_buffer *visual_buffer
)
{
    if (plan == NULL) {
        return 0;
    }

    switch (plan->filter_treatment) {
    case PLASMA_FILTER_TREATMENT_NONE:
        return 1;

    case PLASMA_FILTER_TREATMENT_BLUR:
        if (!plasma_advanced_apply_blur_filter(plan, state, visual_buffer)) {
            return 0;
        }
        return plasma_modern_apply_filter_refinement(plan, state, visual_buffer);

    case PLASMA_FILTER_TREATMENT_GLOW_EDGE:
        return plasma_apply_glow_edge_filter(plan, output, visual_buffer);

    case PLASMA_FILTER_TREATMENT_HALFTONE_STIPPLE:
        return plasma_apply_halftone_stipple_filter(output, visual_buffer);

    case PLASMA_FILTER_TREATMENT_EMBOSS_EDGE:
        return plasma_apply_emboss_edge_filter(output, visual_buffer);

    case PLASMA_FILTER_TREATMENT_KALEIDOSCOPE_MIRROR:
    case PLASMA_FILTER_TREATMENT_RESTRAINED_GLITCH:
    default:
        return 0;
    }
}

static int plasma_apply_phosphor_emulation(
    const struct plasma_plan_tag *plan,
    screensave_visual_buffer *visual_buffer
)
{
    int x;
    int y;

    if (plan == NULL || visual_buffer == NULL || visual_buffer->pixels == NULL || plan->theme == NULL) {
        return 0;
    }

    for (y = 0; y < visual_buffer->size.height; ++y) {
        unsigned char *row;

        row = visual_buffer->pixels + ((size_t)y * (size_t)visual_buffer->stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;
            unsigned int luma_value;
            unsigned int scanline_scale;

            blue_value = (unsigned int)row[(x * 4) + 0];
            green_value = (unsigned int)row[(x * 4) + 1];
            red_value = (unsigned int)row[(x * 4) + 2];
            luma_value = plasma_treatment_max3(blue_value, green_value, red_value);
            scanline_scale = (y & 1) == 0 ? 255U : 168U;

            row[(x * 4) + 0] = plasma_treatment_clamp_channel(
                (((unsigned int)plan->theme->primary_color.blue * luma_value) / 255U) * scanline_scale / 255U
            );
            row[(x * 4) + 1] = plasma_treatment_clamp_channel(
                (((unsigned int)plan->theme->primary_color.green * luma_value) / 255U) * scanline_scale / 255U
            );
            row[(x * 4) + 2] = plasma_treatment_clamp_channel(
                (((unsigned int)plan->theme->primary_color.red * luma_value) / 255U) * scanline_scale / 255U
            );
            row[(x * 4) + 3] = 255U;
        }
    }

    return 1;
}

static int plasma_apply_crt_emulation(
    screensave_visual_buffer *visual_buffer
)
{
    int center_x;
    int center_y;
    int x;
    int y;

    if (visual_buffer == NULL || visual_buffer->pixels == NULL) {
        return 0;
    }

    center_x = visual_buffer->size.width / 2;
    center_y = visual_buffer->size.height / 2;
    if (center_x <= 0) {
        center_x = 1;
    }
    if (center_y <= 0) {
        center_y = 1;
    }

    for (y = 0; y < visual_buffer->size.height; ++y) {
        unsigned char *row;

        row = visual_buffer->pixels + ((size_t)y * (size_t)visual_buffer->stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;
            unsigned int mask_scale;
            unsigned int scanline_scale;
            unsigned int vignette_scale;

            blue_value = (unsigned int)row[(x * 4) + 0];
            green_value = (unsigned int)row[(x * 4) + 1];
            red_value = (unsigned int)row[(x * 4) + 2];

            mask_scale = 192U;
            if ((x % 3) == 0) {
                red_value = (red_value * 255U) / 220U;
            } else if ((x % 3) == 1) {
                green_value = (green_value * 255U) / 220U;
            } else {
                blue_value = (blue_value * 255U) / 220U;
            }

            scanline_scale = (y & 1) == 0 ? 252U : 160U;
            vignette_scale = 255U -
                (plasma_treatment_abs_int(x - center_x) * 56U) / (unsigned int)center_x -
                (plasma_treatment_abs_int(y - center_y) * 56U) / (unsigned int)center_y;
            if (vignette_scale < 96U) {
                vignette_scale = 96U;
            }

            row[(x * 4) + 0] = plasma_treatment_clamp_channel(
                (((blue_value * mask_scale) / 255U) * scanline_scale / 255U) * vignette_scale / 255U
            );
            row[(x * 4) + 1] = plasma_treatment_clamp_channel(
                (((green_value * mask_scale) / 255U) * scanline_scale / 255U) * vignette_scale / 255U
            );
            row[(x * 4) + 2] = plasma_treatment_clamp_channel(
                (((red_value * mask_scale) / 255U) * scanline_scale / 255U) * vignette_scale / 255U
            );
            row[(x * 4) + 3] = 255U;
        }
    }

    return 1;
}

static int plasma_apply_emulation_treatment(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    screensave_visual_buffer *visual_buffer
)
{
    (void)state;

    if (plan == NULL) {
        return 0;
    }

    switch (plan->emulation_treatment) {
    case PLASMA_EMULATION_TREATMENT_NONE:
        return 1;

    case PLASMA_EMULATION_TREATMENT_PHOSPHOR:
        return plasma_apply_phosphor_emulation(plan, visual_buffer);

    case PLASMA_EMULATION_TREATMENT_CRT:
        return plasma_apply_crt_emulation(visual_buffer);

    default:
        return 0;
    }
}

static int plasma_apply_accent_pass(
    const struct plasma_plan_tag *plan,
    screensave_visual_buffer *visual_buffer
)
{
    int x;
    int y;

    if (plan == NULL || visual_buffer == NULL || visual_buffer->pixels == NULL || plan->theme == NULL) {
        return 0;
    }

    for (y = 0; y < visual_buffer->size.height; ++y) {
        unsigned char *row;

        row = visual_buffer->pixels + ((size_t)y * (size_t)visual_buffer->stride_bytes);
        for (x = 0; x < visual_buffer->size.width; ++x) {
            unsigned int blue_value;
            unsigned int green_value;
            unsigned int red_value;
            unsigned int intensity;

            blue_value = (unsigned int)row[(x * 4) + 0];
            green_value = (unsigned int)row[(x * 4) + 1];
            red_value = (unsigned int)row[(x * 4) + 2];
            intensity = plasma_treatment_max3(blue_value, green_value, red_value);
            if (intensity < 48U) {
                continue;
            }

            row[(x * 4) + 0] = plasma_treatment_clamp_channel(
                blue_value + ((((unsigned int)plan->theme->accent_color.blue * intensity) / 255U) / 3U)
            );
            row[(x * 4) + 1] = plasma_treatment_clamp_channel(
                green_value + ((((unsigned int)plan->theme->accent_color.green * intensity) / 255U) / 3U)
            );
            row[(x * 4) + 2] = plasma_treatment_clamp_channel(
                red_value + ((((unsigned int)plan->theme->accent_color.red * intensity) / 255U) / 3U)
            );
            row[(x * 4) + 3] = 255U;
        }
    }

    return 1;
}

static int plasma_apply_accent_treatment(
    const struct plasma_plan_tag *plan,
    struct plasma_execution_state_tag *state,
    screensave_visual_buffer *visual_buffer
)
{
    if (plan == NULL) {
        return 0;
    }

    switch (plan->accent_treatment) {
    case PLASMA_ACCENT_TREATMENT_NONE:
        return 1;

    case PLASMA_ACCENT_TREATMENT_OVERLAY_PASS:
        return plasma_advanced_apply_overlay_accent(plan, state, visual_buffer);

    case PLASMA_ACCENT_TREATMENT_ACCENT_PASS:
        return plasma_apply_accent_pass(plan, visual_buffer);

    default:
        return 0;
    }
}

int plasma_treatment_validate_plan(const struct plasma_plan_tag *plan)
{
    if (plan == NULL) {
        return 0;
    }

    if (!plasma_sampling_treatment_is_supported(plan->sampling_treatment)) {
        return 0;
    }

    if (!plasma_filter_treatment_is_supported(plan->filter_treatment)) {
        return 0;
    }
    if (
        plasma_filter_treatment_requires_advanced(plan->filter_treatment) &&
        !plan->advanced_enabled
    ) {
        return 0;
    }
    if (!plasma_emulation_treatment_is_supported(plan->emulation_treatment)) {
        return 0;
    }
    if (!plasma_accent_treatment_is_supported(plan->accent_treatment)) {
        return 0;
    }
    if (
        plasma_accent_treatment_requires_advanced(plan->accent_treatment) &&
        !plan->advanced_enabled
    ) {
        return 0;
    }

    return 1;
}

int plasma_treatment_apply(
    const struct plasma_plan_tag *plan,
    const struct plasma_execution_state_tag *state,
    const plasma_output_frame *output,
    screensave_visual_buffer *visual_buffer,
    plasma_treated_frame *frame_out
)
{
    if (
        frame_out == NULL ||
        output == NULL ||
        visual_buffer == NULL ||
        state == NULL ||
        !plasma_treatment_validate_plan(plan)
    ) {
        return 0;
    }

    if (!plasma_theme_map_output(plan, state, output, visual_buffer)) {
        return 0;
    }
    if (!plasma_apply_sampling_treatment(plan, state, visual_buffer)) {
        return 0;
    }
    if (!plasma_apply_filter_treatment(plan, (struct plasma_execution_state_tag *)state, output, visual_buffer)) {
        return 0;
    }
    if (!plasma_apply_emulation_treatment(plan, state, visual_buffer)) {
        return 0;
    }
    if (!plasma_apply_accent_treatment(plan, (struct plasma_execution_state_tag *)state, visual_buffer)) {
        return 0;
    }
    if (!plasma_premium_apply_post_refinement(plan, (struct plasma_execution_state_tag *)state, visual_buffer)) {
        return 0;
    }

    frame_out->family = output->family;
    frame_out->mode = output->mode;
    frame_out->size = output->size;
    frame_out->visual_buffer = visual_buffer;
    return 1;
}
