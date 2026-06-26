#include "screensave/sslab.h"
#include "proof_registry.h"

#include <stdio.h>
#include <string.h>

static const char *runner_status_name(sslab_status status)
{
    switch (status) {
    case SSLAB_STATUS_OK:
        return "pass";
    case SSLAB_STATUS_BLOCKED:
        return "blocked";
    case SSLAB_STATUS_UNSUPPORTED_HOST:
        return "unsupported-host";
    case SSLAB_STATUS_INVALID_ARGUMENT:
        return "invalid-argument";
    case SSLAB_STATUS_NOT_FOUND:
        return "not-found";
    case SSLAB_STATUS_OUT_OF_MEMORY:
        return "out-of-memory";
    case SSLAB_STATUS_FAIL:
    default:
        return "fail";
    }
}

static sslab_comparison_class runner_comparison_class(const char *value)
{
    if (value != 0 && strcmp(value, "tolerant") == 0) {
        return SSLAB_COMPARISON_TOLERANT;
    }
    if (value != 0 && strcmp(value, "perceptual") == 0) {
        return SSLAB_COMPARISON_PERCEPTUAL;
    }
    if (value != 0 && strcmp(value, "observational") == 0) {
        return SSLAB_COMPARISON_OBSERVATIONAL;
    }
    return SSLAB_COMPARISON_EXACT;
}

static unsigned long runner_profile_max_frame(const screensave_generated_proof_profile *profile)
{
    unsigned int index;
    unsigned long max_frame;

    max_frame = 0UL;
    for (index = 0U; index < profile->capture_frame_count; ++index) {
        if (profile->capture_frames[index] > max_frame) {
            max_frame = profile->capture_frames[index];
        }
    }
    return max_frame;
}

static void runner_fill_profile_desc(
    const screensave_generated_proof_profile *generated,
    sslab_proof_profile_desc *profile)
{
    unsigned int index;

    memset(profile, 0, sizeof(*profile));
    profile->size = sizeof(*profile);
    profile->abi_version = SSLAB_ABI_VERSION;
    profile->profile_key = generated->key;
    profile->product_key = generated->product;
    profile->preset_key = generated->preset;
    profile->width = generated->width;
    profile->height = generated->height;
    profile->resize_width = generated->resize_width;
    profile->resize_height = generated->resize_height;
    profile->seed = generated->seed;
    profile->delta_ms = generated->delta_ms;
    profile->capture_frame_count = generated->capture_frame_count;
    profile->comparison_class = runner_comparison_class(generated->comparison_class);
    for (index = 0U; index < generated->capture_frame_count && index < SSLAB_MAX_CAPTURE_FRAMES; ++index) {
        profile->capture_frames[index] = generated->capture_frames[index];
    }
}

static void runner_fill_run_desc(
    const screensave_generated_proof_profile *generated,
    unsigned long frame_count,
    sslab_run_desc *run)
{
    memset(run, 0, sizeof(*run));
    run->size = sizeof(*run);
    run->abi_version = SSLAB_ABI_VERSION;
    run->product_key = generated->product;
    run->preset_key = generated->preset;
    run->width = generated->width;
    run->height = generated->height;
    run->seed = generated->seed;
    run->delta_ms = generated->delta_ms;
    run->frame_count = frame_count;
}

static int runner_write_json_string(FILE *file, const char *value)
{
    const char *scan;

    if (file == 0) {
        return 0;
    }
    fputc('"', file);
    if (value != 0) {
        scan = value;
        while (*scan != '\0') {
            if (*scan == '"' || *scan == '\\') {
                fputc('\\', file);
            }
            fputc(*scan, file);
            ++scan;
        }
    }
    fputc('"', file);
    return 1;
}

static int runner_write_proof_json(
    FILE *file,
    const screensave_generated_proof_profile *generated,
    const sslab_capture_receipt *captures,
    const sslab_lifecycle_receipt *lifecycle,
    const sslab_profile_receipt *profile_receipt,
    sslab_status aggregate_status)
{
    unsigned int index;

    fprintf(file, "{\n");
    fprintf(file, "  \"runner_schema\": \"sslab-runner-proof-v0\",\n");
    fprintf(file, "  \"status\": ");
    runner_write_json_string(file, runner_status_name(aggregate_status));
    fprintf(file, ",\n");
    fprintf(file, "  \"profile\": ");
    runner_write_json_string(file, generated->key);
    fprintf(file, ",\n");
    fprintf(file, "  \"product\": ");
    runner_write_json_string(file, generated->product);
    fprintf(file, ",\n");
    fprintf(file, "  \"preset\": ");
    runner_write_json_string(file, generated->preset);
    fprintf(file, ",\n");
    fprintf(file, "  \"captures\": [\n");
    for (index = 0U; index < generated->capture_frame_count; ++index) {
        fprintf(file, "    { \"frame\": %lu, \"status\": ", captures[index].frame_index);
        runner_write_json_string(file, runner_status_name(captures[index].status));
        fprintf(file, ", \"rgba_sha256\": ");
        runner_write_json_string(file, captures[index].rgba_sha256);
        fprintf(file, ", \"raw_rgba_path\": ");
        runner_write_json_string(file, captures[index].raw_rgba_path);
        fprintf(file, ", \"review_ppm_path\": ");
        runner_write_json_string(file, captures[index].review_ppm_path);
        fprintf(file, " }%s\n", (index + 1U) < generated->capture_frame_count ? "," : "");
    }
    fprintf(file, "  ],\n");
    fprintf(file, "  \"lifecycle\": {\n");
    fprintf(file, "    \"status\": ");
    runner_write_json_string(file, runner_status_name(lifecycle->status));
    fprintf(file, ",\n");
    fprintf(file, "    \"create_session\": %lu,\n", lifecycle->create_session);
    fprintf(file, "    \"resize_session\": %lu,\n", lifecycle->resize_session);
    fprintf(file, "    \"step_count\": %lu,\n", lifecycle->step_count);
    fprintf(file, "    \"render_session\": %lu,\n", lifecycle->render_session);
    fprintf(file, "    \"destroy_session\": %lu,\n", lifecycle->destroy_session);
    fprintf(file, "    \"create_destroy_cycles\": %lu,\n", lifecycle->create_destroy_cycles);
    fprintf(file, "    \"checksum\": %lu\n", lifecycle->checksum);
    fprintf(file, "  },\n");
    fprintf(file, "  \"profile_receipt\": {\n");
    fprintf(file, "    \"status\": ");
    runner_write_json_string(file, runner_status_name(profile_receipt->status));
    fprintf(file, ",\n");
    fprintf(file, "    \"measured_frames\": %lu,\n", profile_receipt->measured_frames);
    fprintf(file, "    \"short_soak_frames\": %lu,\n", profile_receipt->short_soak_frames);
    fprintf(file, "    \"frame_time_ms_p50\": %.6f,\n", profile_receipt->frame_time_ms_p50);
    fprintf(file, "    \"frame_time_ms_p95\": %.6f,\n", profile_receipt->frame_time_ms_p95);
    fprintf(file, "    \"frame_time_ms_p99\": %.6f\n", profile_receipt->frame_time_ms_p99);
    fprintf(file, "  }\n");
    fprintf(file, "}\n");
    return 1;
}

static int runner_run_proof(const char *profile_key, const char *output_path, const char *output_root)
{
    const screensave_generated_proof_profile *generated;
    sslab_context_desc context_desc;
    sslab_context *context;
    sslab_proof_profile_desc proof_profile;
    sslab_lifecycle_desc lifecycle_desc;
    sslab_profile_desc profile_desc;
    sslab_capture_receipt captures[SSLAB_MAX_CAPTURE_FRAMES];
    sslab_lifecycle_receipt lifecycle_receipt;
    sslab_profile_receipt profile_receipt;
    sslab_run_desc run;
    sslab_status aggregate_status;
    sslab_status status;
    FILE *file;
    unsigned int index;
    unsigned long max_frame;

    generated = screensave_generated_find_proof_profile(profile_key);
    if (generated == 0) {
        fprintf(stderr, "unknown profile: %s\n", profile_key != 0 ? profile_key : "");
        return 2;
    }
    if (generated->capture_frame_count > SSLAB_MAX_CAPTURE_FRAMES) {
        fprintf(stderr, "profile has too many capture frames: %s\n", profile_key);
        return 2;
    }
    if (sslab_check_host_abi() != SSLAB_STATUS_OK) {
        fprintf(stderr, "unsupported host ABI: sizeof(unsigned long) must be 4\n");
        return 3;
    }

    context = 0;
    context_desc.size = sizeof(context_desc);
    context_desc.abi_version = SSLAB_ABI_VERSION;
    context_desc.output_root = output_root != 0 ? output_root : "out/sslab-runner";
    context_desc.catalog_root = "catalog";
    status = sslab_create_context(&context_desc, &context);
    if (status != SSLAB_STATUS_OK) {
        fprintf(stderr, "failed to create sslab context\n");
        return 4;
    }

    runner_fill_profile_desc(generated, &proof_profile);
    aggregate_status = SSLAB_STATUS_OK;
    memset(captures, 0, sizeof(captures));
    for (index = 0U; index < generated->capture_frame_count; ++index) {
        status = sslab_run_capture(context, &proof_profile, generated->capture_frames[index], &captures[index]);
        if (status != SSLAB_STATUS_OK) {
            aggregate_status = status;
        }
    }

    max_frame = runner_profile_max_frame(generated);
    runner_fill_run_desc(generated, max_frame, &run);
    lifecycle_desc.size = sizeof(lifecycle_desc);
    lifecycle_desc.abi_version = SSLAB_ABI_VERSION;
    lifecycle_desc.run = run;
    lifecycle_desc.resize_width = generated->resize_width;
    lifecycle_desc.resize_height = generated->resize_height;
    lifecycle_desc.create_destroy_cycles = generated->create_destroy_cycles;
    status = sslab_run_lifecycle(context, &lifecycle_desc, &lifecycle_receipt);
    if (status != SSLAB_STATUS_OK) {
        aggregate_status = status;
    }

    profile_desc.size = sizeof(profile_desc);
    profile_desc.abi_version = SSLAB_ABI_VERSION;
    profile_desc.run = run;
    profile_desc.warmup_frames = 2UL;
    profile_desc.measured_frames = 8UL;
    profile_desc.short_soak_frames = generated->create_destroy_cycles > 1UL ? 32UL : 8UL;
    status = sslab_run_profile(context, &profile_desc, &profile_receipt);
    if (status != SSLAB_STATUS_OK) {
        aggregate_status = status;
    }

    file = stdout;
    if (output_path != 0) {
        file = fopen(output_path, "w");
        if (file == 0) {
            sslab_destroy_context(context);
            fprintf(stderr, "failed to open output: %s\n", output_path);
            return 5;
        }
    }
    runner_write_proof_json(file, generated, captures, &lifecycle_receipt, &profile_receipt, aggregate_status);
    if (output_path != 0) {
        fclose(file);
    }
    sslab_destroy_context(context);
    return aggregate_status == SSLAB_STATUS_OK ? 0 : 6;
}

static void runner_usage(void)
{
    fprintf(stderr, "usage: sslab_runner proof --profile <profile-key> [--output <path>] [--output-root <path>]\n");
}

int main(int argc, char **argv)
{
    const char *profile_key;
    const char *output_path;
    const char *output_root;
    int index;

    profile_key = 0;
    output_path = 0;
    output_root = 0;
    if (argc < 4 || strcmp(argv[1], "proof") != 0) {
        runner_usage();
        return 1;
    }
    for (index = 2; index < argc; ++index) {
        if (strcmp(argv[index], "--profile") == 0 && (index + 1) < argc) {
            ++index;
            profile_key = argv[index];
        } else if (strcmp(argv[index], "--output") == 0 && (index + 1) < argc) {
            ++index;
            output_path = argv[index];
        } else if (strcmp(argv[index], "--output-root") == 0 && (index + 1) < argc) {
            ++index;
            output_root = argv[index];
        } else {
            runner_usage();
            return 1;
        }
    }
    if (profile_key == 0) {
        runner_usage();
        return 1;
    }
    return runner_run_proof(profile_key, output_path, output_root);
}
