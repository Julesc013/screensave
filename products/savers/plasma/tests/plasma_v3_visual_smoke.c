#include "plasma_v3_runtime.h"
#include "proof/plasma_v3_proof.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PLASMA_V3_SMOKE_MAX_WIDTH 320U
#define PLASMA_V3_SMOKE_MAX_HEIGHT 180U
#define PLASMA_V3_SMOKE_MAX_PIXELS (PLASMA_V3_SMOKE_MAX_WIDTH * PLASMA_V3_SMOKE_MAX_HEIGHT)
#define PLASMA_V3_SMOKE_MAX_BYTES (PLASMA_V3_SMOKE_MAX_PIXELS * PLASMA_V3_PRESENT_PIXEL_BYTES)

static ss_u32 g_field_buffer[PLASMA_V3_SMOKE_MAX_PIXELS];
static ss_u8 g_material_buffer[PLASMA_V3_SMOKE_MAX_BYTES];
static ss_u8 g_treatment_buffer[PLASMA_V3_SMOKE_MAX_BYTES];
static ss_u8 g_present_buffer[PLASMA_V3_SMOKE_MAX_BYTES];
static ss_u8 g_compare_buffer[PLASMA_V3_SMOKE_MAX_BYTES];

static int plasma_v3_smoke_text_equals(const char *left, const char *right)
{
    if (left == 0 || right == 0) {
        return 0;
    }
    while (*left != '\0' && *right != '\0') {
        if (*left != *right) {
            return 0;
        }
        ++left;
        ++right;
    }
    return *left == *right;
}

static void plasma_v3_smoke_copy_bytes(ss_u8 *dst, const ss_u8 *src, ss_u32 byte_count)
{
    ss_u32 index;

    for (index = 0U; index < byte_count; ++index) {
        dst[index] = src[index];
    }
}

static int plasma_v3_smoke_make_runtime(
    const plasma_v3_spec *spec,
    ss_u32 renderer,
    ss_u32 width,
    ss_u32 height,
    plasma_v3_runtime *runtime
)
{
    plasma_v3_plan_request request;
    plasma_v3_plan plan;
    plasma_v3_runtime_buffers buffers;
    ss_u32 cell_count;
    ss_u32 byte_count;

    if (width > PLASMA_V3_SMOKE_MAX_WIDTH || height > PLASMA_V3_SMOKE_MAX_HEIGHT) {
        return 0;
    }

    request.struct_size = (ss_u32)sizeof(request);
    request.requested_spec = spec;
    request.drawable_size.width = width;
    request.drawable_size.height = height;
    request.requested_renderer = renderer;
    request.capability_flags = PLASMA_V3_CAP_SOFTWARE_REFERENCE | PLASMA_V3_CAP_GDI | PLASMA_V3_CAP_GL11;
    request.base_seed = 7U;
    if (plasma_v3_plan_compile(&request, &plan) != SS_V2_STATUS_OK) {
        return 0;
    }
    if (plasma_v3_plan_required_field_cells(&plan, &cell_count) != SS_V2_STATUS_OK) {
        return 0;
    }
    if (plasma_v3_plan_required_pixel_bytes(&plan, &byte_count) != SS_V2_STATUS_OK) {
        return 0;
    }
    buffers.struct_size = (ss_u32)sizeof(buffers);
    buffers.field_buffer = g_field_buffer;
    buffers.material_buffer = g_material_buffer;
    buffers.treatment_buffer = g_treatment_buffer;
    buffers.present_buffer = g_present_buffer;
    buffers.field_cell_count = PLASMA_V3_SMOKE_MAX_PIXELS;
    buffers.material_byte_count = PLASMA_V3_SMOKE_MAX_BYTES;
    buffers.treatment_byte_count = PLASMA_V3_SMOKE_MAX_BYTES;
    buffers.present_byte_count = PLASMA_V3_SMOKE_MAX_BYTES;
    return plasma_v3_runtime_bind(&plan, &buffers, runtime) == SS_V2_STATUS_OK &&
        cell_count == width * height &&
        byte_count == width * height * PLASMA_V3_PRESENT_PIXEL_BYTES;
}

static int plasma_v3_smoke_render_signature(
    const plasma_v3_spec *spec,
    ss_u32 renderer,
    ss_u32 width,
    ss_u32 height,
    ss_u32 target_frame,
    plasma_v3_signature *signature
)
{
    plasma_v3_runtime runtime;
    ss_u32 frame;

    if (!plasma_v3_smoke_make_runtime(spec, renderer, width, height, &runtime)) {
        return 0;
    }
    for (frame = 0U; frame <= target_frame; ++frame) {
        if (plasma_v3_runtime_render_frame(&runtime) != SS_V2_STATUS_OK) {
            return 0;
        }
        if (frame != target_frame) {
            if (plasma_v3_runtime_advance(&runtime, runtime.resolved_plan.frame_delta_millis) != SS_V2_STATUS_OK) {
                return 0;
            }
        }
    }
    return plasma_v3_proof_signature(runtime.present_buffer, runtime.present_byte_count, signature) == SS_V2_STATUS_OK;
}

static int plasma_v3_smoke_run(void)
{
    plasma_v3_spec spec;
    plasma_v3_spec variant;
    plasma_v3_signature baseline;
    plasma_v3_signature changed;

    plasma_v3_spec_set_defaults(&spec);
    if (plasma_v3_spec_is_valid(&spec) != SS_V2_TRUE) {
        return 1;
    }
    if (!plasma_v3_smoke_text_equals(plasma_v3_spec_schema_id(), PLASMA_V3_SCHEMA_ID)) {
        return 2;
    }
    if (!plasma_v3_smoke_text_equals(plasma_v3_control_token(PLASMA_V3_CONTROL_WARP), "warp")) {
        return 3;
    }
    if (plasma_v3_spec_set_material_key(&spec, "cool_signal") != SS_V2_STATUS_OK) {
        return 4;
    }
    if (spec.material_kind != PLASMA_V3_MATERIAL_COOL_SIGNAL) {
        return 5;
    }
    if (!plasma_v3_smoke_render_signature(&spec, PLASMA_V3_RENDERER_GDI, 96U, 54U, 32U, &baseline)) {
        return 6;
    }
    if (baseline.lit_pixels == 0U || baseline.hash == 0U) {
        return 7;
    }
    variant = spec;
    if (plasma_v3_spec_apply_control(&variant, PLASMA_V3_CONTROL_WARP, 920U) != SS_V2_STATUS_OK) {
        return 8;
    }
    if (!plasma_v3_smoke_render_signature(&variant, PLASMA_V3_RENDERER_GDI, 96U, 54U, 32U, &changed)) {
        return 9;
    }
    if (baseline.hash == changed.hash) {
        return 10;
    }
    variant = spec;
    if (plasma_v3_spec_apply_control(&variant, PLASMA_V3_CONTROL_PALETTE_SHIFT, 940U) != SS_V2_STATUS_OK) {
        return 11;
    }
    if (!plasma_v3_smoke_render_signature(&variant, PLASMA_V3_RENDERER_GL11, 96U, 54U, 32U, &changed)) {
        return 12;
    }
    if (baseline.hash == changed.hash) {
        return 13;
    }
    return 0;
}

static int plasma_v3_smoke_join_path(char *buffer, unsigned int buffer_size, const char *dir, const char *file)
{
    unsigned int index;
    unsigned int out;

    if (buffer == 0 || buffer_size == 0U || dir == 0 || file == 0) {
        return 0;
    }
    out = 0U;
    index = 0U;
    while (dir[index] != '\0' && out + 1U < buffer_size) {
        buffer[out] = dir[index];
        ++out;
        ++index;
    }
    if (out == 0U || out + 2U >= buffer_size) {
        return 0;
    }
    if (buffer[out - 1U] != '/' && buffer[out - 1U] != '\\') {
        buffer[out] = '/';
        ++out;
    }
    index = 0U;
    while (file[index] != '\0' && out + 1U < buffer_size) {
        buffer[out] = file[index];
        ++out;
        ++index;
    }
    buffer[out] = '\0';
    return file[index] == '\0';
}

static int plasma_v3_smoke_write_ppm(
    const char *path,
    const ss_u8 *pixels,
    ss_u32 width,
    ss_u32 height
)
{
    FILE *file;
    ss_u32 index;
    ss_u32 pixel_count;
    ss_u32 offset;

    file = fopen(path, "wb");
    if (file == 0) {
        return 0;
    }
    fprintf(file, "P6\n%u %u\n255\n", (unsigned int)width, (unsigned int)height);
    pixel_count = width * height;
    for (index = 0U; index < pixel_count; ++index) {
        offset = index * PLASMA_V3_PRESENT_PIXEL_BYTES;
        fputc((int)pixels[offset + 0U], file);
        fputc((int)pixels[offset + 1U], file);
        fputc((int)pixels[offset + 2U], file);
    }
    fclose(file);
    return 1;
}

static int plasma_v3_smoke_capture(
    const char *out_dir,
    const char *label,
    const char *control_token,
    ss_u32 control_value,
    const char *renderer_token,
    ss_u32 width,
    ss_u32 height
)
{
    plasma_v3_spec spec;
    plasma_v3_runtime runtime;
    plasma_v3_signature signature;
    ss_u32 control_id;
    ss_u32 renderer;
    ss_u32 frame;
    ss_u32 desired_index;
    ss_u32 desired_frames[4];
    char file_name[128];
    char path[512];
    FILE *summary;

    desired_frames[0] = 0U;
    desired_frames[1] = 8U;
    desired_frames[2] = 32U;
    desired_frames[3] = 64U;
    plasma_v3_spec_set_defaults(&spec);
    if (control_token != 0 && !plasma_v3_smoke_text_equals(control_token, "none")) {
        if (plasma_v3_control_from_token(control_token, &control_id) != SS_V2_STATUS_OK) {
            return 20;
        }
        if (plasma_v3_spec_apply_control(&spec, control_id, control_value) != SS_V2_STATUS_OK) {
            return 21;
        }
    }
    if (plasma_v3_renderer_from_token(renderer_token, &renderer) != SS_V2_STATUS_OK) {
        return 22;
    }
    if (!plasma_v3_smoke_make_runtime(&spec, renderer, width, height, &runtime)) {
        return 23;
    }
    if (!plasma_v3_smoke_join_path(path, (unsigned int)sizeof(path), out_dir, "summary.txt")) {
        return 24;
    }
    summary = fopen(path, "w");
    if (summary == 0) {
        return 25;
    }
    fprintf(summary, "label=%s\n", label);
    fprintf(summary, "control=%s\n", control_token);
    fprintf(summary, "control_value=%u\n", (unsigned int)control_value);
    fprintf(summary, "requested_renderer=%s\n", renderer_token);
    fprintf(summary, "active_renderer=%s\n", plasma_v3_renderer_token(runtime.resolved_plan.active_renderer));
    fprintf(summary, "width=%u\n", (unsigned int)width);
    fprintf(summary, "height=%u\n", (unsigned int)height);
    desired_index = 0U;
    for (frame = 0U; frame <= desired_frames[3]; ++frame) {
        if (plasma_v3_runtime_render_frame(&runtime) != SS_V2_STATUS_OK) {
            fclose(summary);
            return 26;
        }
        if (frame == desired_frames[desired_index]) {
            if (plasma_v3_proof_signature(runtime.present_buffer, runtime.present_byte_count, &signature) != SS_V2_STATUS_OK) {
                fclose(summary);
                return 27;
            }
            sprintf(file_name, "%s-frame-%04u.ppm", label, (unsigned int)frame);
            if (!plasma_v3_smoke_join_path(path, (unsigned int)sizeof(path), out_dir, file_name)) {
                fclose(summary);
                return 28;
            }
            if (!plasma_v3_smoke_write_ppm(path, runtime.present_buffer, width, height)) {
                fclose(summary);
                return 29;
            }
            fprintf(
                summary,
                "frame=%04u hash=%08x lit=%u path=%s\n",
                (unsigned int)frame,
                (unsigned int)signature.hash,
                (unsigned int)signature.lit_pixels,
                file_name
            );
            ++desired_index;
        }
        if (frame != desired_frames[3]) {
            if (plasma_v3_runtime_advance(&runtime, runtime.resolved_plan.frame_delta_millis) != SS_V2_STATUS_OK) {
                fclose(summary);
                return 30;
            }
        }
    }
    plasma_v3_smoke_copy_bytes(g_compare_buffer, runtime.present_buffer, runtime.present_byte_count);
    fclose(summary);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc > 1 && plasma_v3_smoke_text_equals(argv[1], "capture")) {
        if (argc != 9) {
            return 19;
        }
        return plasma_v3_smoke_capture(
            argv[2],
            argv[3],
            argv[4],
            (ss_u32)atoi(argv[5]),
            argv[6],
            (ss_u32)atoi(argv[7]),
            (ss_u32)atoi(argv[8])
        );
    }
    return plasma_v3_smoke_run();
}
