#include "plasma_v2_core.h"

#include <stdlib.h>
#include <string.h>

typedef struct plasma_v2_material_colors_tag {
    ss_u8 floor_r;
    ss_u8 floor_g;
    ss_u8 floor_b;
    ss_u8 crest_r;
    ss_u8 crest_g;
    ss_u8 crest_b;
} plasma_v2_material_colors;

struct plasma_v2_core_session_tag {
    plasma_spec_v2 spec;
    ss_u32 width;
    ss_u32 height;
    ss_u32 base_seed;
    ss_u32 stream_seed;
    ss_u32 elapsed_ms;
    ss_u32 frame_index;
    ss_u32 phase;
    ss_u32 previous_count;
    ss_u8 *previous_values;
};

static ss_u32 plasma_v2_mul_overflows(ss_u32 left, ss_u32 right)
{
    if (left == 0U || right == 0U) {
        return SS_V2_FALSE;
    }
    return left > (0xffffffffU / right) ? SS_V2_TRUE : SS_V2_FALSE;
}

static ss_u32 plasma_v2_prepare_previous(plasma_v2_core_session *session, ss_u32 width, ss_u32 height)
{
    ss_u32 count;
    ss_u8 *next_values;

    if (session == 0 || width == 0U || height == 0U) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    if (plasma_v2_mul_overflows(width, height) == SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_SIZE;
    }

    count = width * height;
    next_values = (ss_u8 *)malloc((size_t)count);
    if (next_values == 0) {
        return SS_V2_STATUS_FAIL;
    }
    memset(next_values, 0, (size_t)count);

    if (session->previous_values != 0) {
        free(session->previous_values);
    }
    session->previous_values = next_values;
    session->previous_count = count;
    session->width = width;
    session->height = height;
    return SS_V2_STATUS_OK;
}

static ss_u32 plasma_v2_tri8(ss_u32 value)
{
    value = value & 255U;
    if (value < 128U) {
        return value * 2U;
    }
    return (255U - value) * 2U;
}

static ss_u32 plasma_v2_classic_value(const plasma_v2_core_session *session, ss_u32 x, ss_u32 y)
{
    ss_u32 width;
    ss_u32 height;
    ss_u32 diagonal;
    ss_u32 a;
    ss_u32 b;
    ss_u32 c;
    ss_u32 phase;
    ss_u32 warp;

    width = session->width == 0U ? 1U : session->width;
    height = session->height == 0U ? 1U : session->height;
    diagonal = width + height;
    if (diagonal == 0U) {
        diagonal = 1U;
    }

    phase = (session->phase + (session->base_seed & 255U)) & 255U;
    warp = (plasma_v2_tri8(((y * 5U) + phase) & 255U) * session->spec.warp_amount) / 100U;
    a = plasma_v2_tri8((((x * (session->spec.scale + 64U)) / width) + phase + warp) & 255U);
    b = plasma_v2_tri8((((y * (session->spec.complexity + 96U)) / height) + (phase * 2U)) & 255U);
    c = plasma_v2_tri8(((((x + y) * 160U) / diagonal) + (phase * 3U) + session->stream_seed) & 255U);

    return (a + b + c) / 3U;
}

static ss_u32 plasma_v2_radial_value(const plasma_v2_core_session *session, ss_u32 x, ss_u32 y)
{
    ss_u32 width;
    ss_u32 height;
    ss_i32 dx;
    ss_i32 dy;
    ss_u32 radial;
    ss_u32 fold;
    ss_u32 warped;
    ss_u32 phase;
    ss_u32 warp;

    width = session->width == 0U ? 1U : session->width;
    height = session->height == 0U ? 1U : session->height;
    dx = (ss_i32)((x * 256U) / width) - (ss_i32)128;
    dy = (ss_i32)((y * 256U) / height) - (ss_i32)128;
    radial = (ss_u32)(((dx * dx) + (dy * dy)) >> 1) & 255U;
    phase = (session->phase + ((session->base_seed ^ session->stream_seed) & 255U)) & 255U;
    fold = plasma_v2_tri8(((x * 3U) + (y * 5U) + phase) & 255U);
    warped = plasma_v2_tri8((radial + phase + ((fold * session->spec.warp_amount) / 100U)) & 255U);
    warp = session->spec.warp_amount;

    return ((warped * (100U - warp)) + (fold * warp)) / 100U;
}

static ss_u32 plasma_v2_field_value(plasma_v2_core_session *session, ss_u32 x, ss_u32 y)
{
    ss_u32 value;
    ss_u32 offset;
    ss_u32 previous;
    ss_u32 feedback;

    if (session->spec.field_family == PLASMA_V2_FIELD_RADIAL_WARPED) {
        value = plasma_v2_radial_value(session, x, y);
    } else {
        value = plasma_v2_classic_value(session, x, y);
    }

    if (session->spec.output_style == PLASMA_V2_OUTPUT_BANDED) {
        value = ((value / 32U) * 32U) + 16U;
    } else if (session->spec.output_style == PLASMA_V2_OUTPUT_CONTOUR) {
        value = (value & 31U) < 5U ? 236U : 34U;
    }

    offset = (y * session->width) + x;
    if (offset < session->previous_count) {
        previous = session->previous_values[offset];
        feedback = session->spec.feedback_amount;
        value = ((value * (100U - feedback)) + (previous * feedback)) / 100U;
        session->previous_values[offset] = (ss_u8)(value & 255U);
    }

    return value & 255U;
}

static void plasma_v2_material(ss_u32 material_id, plasma_v2_material_colors *colors)
{
    if (colors == 0) {
        return;
    }

    if (material_id == PLASMA_V2_MATERIAL_AURORA_COOL) {
        colors->floor_r = 3U;
        colors->floor_g = 24U;
        colors->floor_b = 42U;
        colors->crest_r = 104U;
        colors->crest_g = 248U;
        colors->crest_b = 214U;
        return;
    }
    if (material_id == PLASMA_V2_MATERIAL_OCEANIC_BLUE) {
        colors->floor_r = 4U;
        colors->floor_g = 16U;
        colors->floor_b = 54U;
        colors->crest_r = 104U;
        colors->crest_g = 198U;
        colors->crest_b = 252U;
        return;
    }
    if (material_id == PLASMA_V2_MATERIAL_MUSEUM_PHOSPHOR) {
        colors->floor_r = 4U;
        colors->floor_g = 28U;
        colors->floor_b = 8U;
        colors->crest_r = 172U;
        colors->crest_g = 255U;
        colors->crest_b = 150U;
        return;
    }
    if (material_id == PLASMA_V2_MATERIAL_QUIET_DARKROOM) {
        colors->floor_r = 12U;
        colors->floor_g = 8U;
        colors->floor_b = 7U;
        colors->crest_r = 132U;
        colors->crest_g = 70U;
        colors->crest_b = 48U;
        return;
    }

    colors->floor_r = 18U;
    colors->floor_g = 4U;
    colors->floor_b = 1U;
    colors->crest_r = 255U;
    colors->crest_g = 174U;
    colors->crest_b = 48U;
}

static ss_i32 plasma_v2_clamp_i32(ss_i32 value)
{
    if (value < 0) {
        return 0;
    }
    if (value > 255) {
        return 255;
    }
    return value;
}

static ss_u8 plasma_v2_tone_component(ss_u8 floor_value, ss_u8 crest_value, ss_u32 value, const plasma_spec_v2 *spec)
{
    ss_i32 component;

    component = (ss_i32)floor_value + ((((ss_i32)crest_value - (ss_i32)floor_value) * (ss_i32)value) / 255);
    component = (component * (ss_i32)spec->brightness) / 100;
    component = 128 + (((component - 128) * (ss_i32)spec->contrast) / 100);
    return (ss_u8)plasma_v2_clamp_i32(component);
}

static void plasma_v2_apply_treatment(
    const plasma_v2_core_session *session,
    ss_u32 x,
    ss_u32 y,
    ss_u8 *red,
    ss_u8 *green,
    ss_u8 *blue
)
{
    ss_i32 adjustment;

    if (session->spec.treatment_flags == PLASMA_V2_TREATMENT_RESTRAINED_DITHER) {
        adjustment = (((x ^ y ^ session->frame_index) & 1U) != 0U) ? 6 : -4;
        *red = (ss_u8)plasma_v2_clamp_i32((ss_i32)(*red) + adjustment);
        *green = (ss_u8)plasma_v2_clamp_i32((ss_i32)(*green) + adjustment);
        *blue = (ss_u8)plasma_v2_clamp_i32((ss_i32)(*blue) + adjustment);
    } else if (session->spec.treatment_flags == PLASMA_V2_TREATMENT_RESTRAINED_CRT) {
        if ((y & 1U) != 0U) {
            *red = (ss_u8)(((ss_u32)(*red) * 82U) / 100U);
            *green = (ss_u8)(((ss_u32)(*green) * 82U) / 100U);
            *blue = (ss_u8)(((ss_u32)(*blue) * 82U) / 100U);
        }
    }
}

ss_u32 plasma_v2_core_create(const plasma_v2_core_desc *desc, plasma_v2_core_session **session_out)
{
    plasma_v2_core_session *session;
    ss_u32 status;

    if (session_out == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    *session_out = 0;
    if (desc == 0 || desc->spec == 0 || plasma_spec_v2_is_valid(desc->spec) != SS_V2_TRUE) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    session = (plasma_v2_core_session *)calloc(1U, sizeof(*session));
    if (session == 0) {
        return SS_V2_STATUS_FAIL;
    }

    session->spec = *desc->spec;
    session->base_seed = desc->base_seed;
    session->stream_seed = desc->stream_seed;
    session->phase = (desc->base_seed ^ (desc->stream_seed * 33U)) & 255U;

    status = plasma_v2_prepare_previous(session, desc->width, desc->height);
    if (status != SS_V2_STATUS_OK) {
        free(session);
        return status;
    }

    *session_out = session;
    return SS_V2_STATUS_OK;
}

void plasma_v2_core_destroy(plasma_v2_core_session *session)
{
    if (session == 0) {
        return;
    }
    if (session->previous_values != 0) {
        free(session->previous_values);
    }
    free(session);
}

ss_u32 plasma_v2_core_resize(plasma_v2_core_session *session, ss_u32 width, ss_u32 height)
{
    if (session == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    return plasma_v2_prepare_previous(session, width, height);
}

ss_u32 plasma_v2_core_advance(plasma_v2_core_session *session, ss_u32 delta_ms)
{
    ss_u32 step;

    if (session == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }

    session->elapsed_ms += delta_ms;
    session->frame_index += 1U;
    step = ((delta_ms * (session->spec.motion_speed + 1U)) / 64U) + 1U;
    session->phase = (session->phase + step) & 255U;
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v2_core_render(plasma_v2_core_session *session, ss_v2_draw_target *target)
{
    ss_v2_surface_desc *surface;
    plasma_v2_material_colors colors;
    ss_u32 status;
    ss_u32 x;
    ss_u32 y;
    ss_u32 value;
    ss_u32 offset;
    ss_u8 red;
    ss_u8 green;
    ss_u8 blue;

    if (session == 0 || target == 0) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    status = ss_v2_draw_target_is_valid(target);
    if (status != SS_V2_STATUS_OK) {
        return status;
    }
    surface = target->surface;
    if (surface == 0) {
        return SS_V2_STATUS_UNSUPPORTED;
    }
    if (surface->width != session->width || surface->height != session->height) {
        return SS_V2_STATUS_BAD_SIZE;
    }

    plasma_v2_material(session->spec.material_id, &colors);
    for (y = 0U; y < surface->height; ++y) {
        for (x = 0U; x < surface->width; ++x) {
            value = plasma_v2_field_value(session, x, y);
            red = plasma_v2_tone_component(colors.floor_r, colors.crest_r, value, &session->spec);
            green = plasma_v2_tone_component(colors.floor_g, colors.crest_g, value, &session->spec);
            blue = plasma_v2_tone_component(colors.floor_b, colors.crest_b, value, &session->spec);
            plasma_v2_apply_treatment(session, x, y, &red, &green, &blue);

            offset = (y * surface->stride_bytes) + (x * 4U);
            surface->pixels[offset + 0U] = red;
            surface->pixels[offset + 1U] = green;
            surface->pixels[offset + 2U] = blue;
            surface->pixels[offset + 3U] = 255U;
        }
    }

    return SS_V2_STATUS_OK;
}
