#include "runtime.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct sslab_sha256_context_tag {
    unsigned long state[8];
    unsigned long bitlen_hi;
    unsigned long bitlen_lo;
    unsigned char data[64];
    unsigned int datalen;
} sslab_sha256_context;

#define SSLAB_SHA256_ROTR(value, bits) (((value) >> (bits)) | ((value) << (32U - (bits))))
#define SSLAB_SHA256_CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define SSLAB_SHA256_MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define SSLAB_SHA256_EP0(x) (SSLAB_SHA256_ROTR((x), 2U) ^ SSLAB_SHA256_ROTR((x), 13U) ^ SSLAB_SHA256_ROTR((x), 22U))
#define SSLAB_SHA256_EP1(x) (SSLAB_SHA256_ROTR((x), 6U) ^ SSLAB_SHA256_ROTR((x), 11U) ^ SSLAB_SHA256_ROTR((x), 25U))
#define SSLAB_SHA256_SIG0(x) (SSLAB_SHA256_ROTR((x), 7U) ^ SSLAB_SHA256_ROTR((x), 18U) ^ ((x) >> 3U))
#define SSLAB_SHA256_SIG1(x) (SSLAB_SHA256_ROTR((x), 17U) ^ SSLAB_SHA256_ROTR((x), 19U) ^ ((x) >> 10U))

static const unsigned long g_sslab_sha256_k[64] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
    0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
    0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
    0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
    0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
    0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
    0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
    0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
    0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
    0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
    0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
    0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
    0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

static void sslab_sha256_transform(sslab_sha256_context *context, const unsigned char data[64])
{
    unsigned long a;
    unsigned long b;
    unsigned long c;
    unsigned long d;
    unsigned long e;
    unsigned long f;
    unsigned long g;
    unsigned long h;
    unsigned long t1;
    unsigned long t2;
    unsigned long m[64];
    unsigned int i;
    unsigned int j;

    for (i = 0U, j = 0U; i < 16U; ++i, j += 4U) {
        m[i] = (((unsigned long)data[j]) << 24U) |
            (((unsigned long)data[j + 1U]) << 16U) |
            (((unsigned long)data[j + 2U]) << 8U) |
            ((unsigned long)data[j + 3U]);
    }
    for (; i < 64U; ++i) {
        m[i] = SSLAB_SHA256_SIG1(m[i - 2U]) + m[i - 7U] + SSLAB_SHA256_SIG0(m[i - 15U]) + m[i - 16U];
    }

    a = context->state[0];
    b = context->state[1];
    c = context->state[2];
    d = context->state[3];
    e = context->state[4];
    f = context->state[5];
    g = context->state[6];
    h = context->state[7];

    for (i = 0U; i < 64U; ++i) {
        t1 = h + SSLAB_SHA256_EP1(e) + SSLAB_SHA256_CH(e, f, g) + g_sslab_sha256_k[i] + m[i];
        t2 = SSLAB_SHA256_EP0(a) + SSLAB_SHA256_MAJ(a, b, c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    context->state[0] += a;
    context->state[1] += b;
    context->state[2] += c;
    context->state[3] += d;
    context->state[4] += e;
    context->state[5] += f;
    context->state[6] += g;
    context->state[7] += h;
}

static void sslab_sha256_init(sslab_sha256_context *context)
{
    context->datalen = 0U;
    context->bitlen_hi = 0UL;
    context->bitlen_lo = 0UL;
    context->state[0] = 0x6a09e667UL;
    context->state[1] = 0xbb67ae85UL;
    context->state[2] = 0x3c6ef372UL;
    context->state[3] = 0xa54ff53aUL;
    context->state[4] = 0x510e527fUL;
    context->state[5] = 0x9b05688cUL;
    context->state[6] = 0x1f83d9abUL;
    context->state[7] = 0x5be0cd19UL;
}

static void sslab_sha256_add_bits(sslab_sha256_context *context, unsigned long bits)
{
    unsigned long old_lo;

    old_lo = context->bitlen_lo;
    context->bitlen_lo += bits;
    if (context->bitlen_lo < old_lo) {
        ++context->bitlen_hi;
    }
}

static void sslab_sha256_update(sslab_sha256_context *context, const unsigned char *data, unsigned long length)
{
    unsigned long index;

    for (index = 0UL; index < length; ++index) {
        context->data[context->datalen] = data[index];
        ++context->datalen;
        if (context->datalen == 64U) {
            sslab_sha256_transform(context, context->data);
            sslab_sha256_add_bits(context, 512UL);
            context->datalen = 0U;
        }
    }
}

static void sslab_sha256_final(sslab_sha256_context *context, unsigned char hash[32])
{
    unsigned int i;
    unsigned long bitlen_hi;
    unsigned long bitlen_lo;

    sslab_sha256_add_bits(context, ((unsigned long)context->datalen) * 8UL);
    bitlen_hi = context->bitlen_hi;
    bitlen_lo = context->bitlen_lo;

    i = context->datalen;
    if (context->datalen < 56U) {
        context->data[i] = 0x80U;
        ++i;
        while (i < 56U) {
            context->data[i] = 0U;
            ++i;
        }
    } else {
        context->data[i] = 0x80U;
        ++i;
        while (i < 64U) {
            context->data[i] = 0U;
            ++i;
        }
        sslab_sha256_transform(context, context->data);
        memset(context->data, 0, 56U);
    }

    context->data[56] = (unsigned char)((bitlen_hi >> 24U) & 0xffU);
    context->data[57] = (unsigned char)((bitlen_hi >> 16U) & 0xffU);
    context->data[58] = (unsigned char)((bitlen_hi >> 8U) & 0xffU);
    context->data[59] = (unsigned char)(bitlen_hi & 0xffU);
    context->data[60] = (unsigned char)((bitlen_lo >> 24U) & 0xffU);
    context->data[61] = (unsigned char)((bitlen_lo >> 16U) & 0xffU);
    context->data[62] = (unsigned char)((bitlen_lo >> 8U) & 0xffU);
    context->data[63] = (unsigned char)(bitlen_lo & 0xffU);
    sslab_sha256_transform(context, context->data);

    for (i = 0U; i < 4U; ++i) {
        hash[i] = (unsigned char)((context->state[0] >> (24U - (i * 8U))) & 0xffU);
        hash[i + 4U] = (unsigned char)((context->state[1] >> (24U - (i * 8U))) & 0xffU);
        hash[i + 8U] = (unsigned char)((context->state[2] >> (24U - (i * 8U))) & 0xffU);
        hash[i + 12U] = (unsigned char)((context->state[3] >> (24U - (i * 8U))) & 0xffU);
        hash[i + 16U] = (unsigned char)((context->state[4] >> (24U - (i * 8U))) & 0xffU);
        hash[i + 20U] = (unsigned char)((context->state[5] >> (24U - (i * 8U))) & 0xffU);
        hash[i + 24U] = (unsigned char)((context->state[6] >> (24U - (i * 8U))) & 0xffU);
        hash[i + 28U] = (unsigned char)((context->state[7] >> (24U - (i * 8U))) & 0xffU);
    }
}

static void sslab_sha256_hex(const unsigned char *data, unsigned long length, char out[65])
{
    static const char hex[] = "0123456789abcdef";
    sslab_sha256_context context;
    unsigned char hash[32];
    unsigned int index;

    sslab_sha256_init(&context);
    sslab_sha256_update(&context, data, length);
    sslab_sha256_final(&context, hash);
    for (index = 0U; index < 32U; ++index) {
        out[index * 2U] = hex[(hash[index] >> 4U) & 0x0fU];
        out[(index * 2U) + 1U] = hex[hash[index] & 0x0fU];
    }
    out[64] = '\0';
}

static unsigned long sslab_checksum_rgba(const unsigned char *rgba, unsigned long byte_count)
{
    unsigned long checksum;
    unsigned long index;

    checksum = 2166136261UL;
    for (index = 0UL; index < byte_count; ++index) {
        checksum ^= (unsigned long)rgba[index];
        checksum *= 16777619UL;
    }
    return checksum;
}

static sslab_status sslab_alloc_rgba(
    sslab_u32 width,
    sslab_u32 height,
    unsigned char **rgba_out,
    unsigned long *stride_out,
    unsigned long *byte_count_out)
{
    unsigned long stride;
    unsigned long byte_count;
    unsigned char *rgba;

    if (width == 0UL || height == 0UL || rgba_out == 0 || stride_out == 0 || byte_count_out == 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    stride = width * 4UL;
    byte_count = stride * height;
    rgba = (unsigned char *)malloc((size_t)byte_count);
    if (rgba == 0) {
        return SSLAB_STATUS_OUT_OF_MEMORY;
    }
    *rgba_out = rgba;
    *stride_out = stride;
    *byte_count_out = byte_count;
    return SSLAB_STATUS_OK;
}

static sslab_status sslab_run_rendered_frame(
    sslab_context *context,
    const sslab_run_desc *run,
    sslab_u32 frame_index,
    unsigned char *rgba,
    unsigned long stride)
{
    sslab_product *product;
    sslab_session *session;
    sslab_surface_desc surface;
    sslab_status status;
    sslab_u32 step_index;

    product = 0;
    session = 0;
    status = sslab_open_product(context, run->product_key, &product);
    if (status != SSLAB_STATUS_OK) {
        return status;
    }
    status = sslab_create_session(product, run, &session);
    if (status == SSLAB_STATUS_OK) {
        for (step_index = 0UL; step_index < frame_index; ++step_index) {
            status = sslab_step_session(session, run->delta_ms);
            if (status != SSLAB_STATUS_OK) {
                break;
            }
        }
    }
    if (status == SSLAB_STATUS_OK) {
        surface.size = sizeof(surface);
        surface.abi_version = SSLAB_ABI_VERSION;
        surface.width = run->width;
        surface.height = run->height;
        surface.stride = stride;
        surface.rgba = rgba;
        status = sslab_render_session(session, &surface);
    }
    sslab_destroy_session(session);
    sslab_close_product(product);
    return status;
}

static void sslab_sort_doubles(double *values, sslab_u32 count)
{
    sslab_u32 index;
    sslab_u32 scan;
    double value;

    for (index = 1UL; index < count; ++index) {
        value = values[index];
        scan = index;
        while (scan > 0UL && values[scan - 1UL] > value) {
            values[scan] = values[scan - 1UL];
            --scan;
        }
        values[scan] = value;
    }
}

static double sslab_percentile(const double *values, sslab_u32 count, sslab_u32 percentile)
{
    sslab_u32 index;

    if (values == 0 || count == 0UL) {
        return 0.0;
    }
    index = ((count - 1UL) * percentile) / 100UL;
    if (index >= count) {
        index = count - 1UL;
    }
    return values[index];
}

sslab_status sslab_run_capture(
    sslab_context *context,
    const sslab_proof_profile_desc *profile,
    sslab_u32 frame_index,
    sslab_capture_receipt *receipt)
{
    sslab_run_desc run;
    sslab_status status;
    unsigned char *rgba;
    unsigned long stride;
    unsigned long byte_count;

    if (context == 0 || profile == 0 || receipt == 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    memset(receipt, 0, sizeof(*receipt));
    receipt->size = sizeof(*receipt);
    receipt->abi_version = SSLAB_ABI_VERSION;
    receipt->status = SSLAB_STATUS_FAIL;
    status = sslab_validate_desc(profile->size, sizeof(*profile), profile->abi_version);
    if (status != SSLAB_STATUS_OK) {
        receipt->status = status;
        return status;
    }

    run.size = sizeof(run);
    run.abi_version = SSLAB_ABI_VERSION;
    run.product_key = profile->product_key;
    run.preset_key = profile->preset_key;
    run.width = profile->width;
    run.height = profile->height;
    run.seed = profile->seed;
    run.delta_ms = profile->delta_ms;
    run.frame_count = frame_index;
    rgba = 0;
    status = sslab_alloc_rgba(profile->width, profile->height, &rgba, &stride, &byte_count);
    if (status == SSLAB_STATUS_OK) {
        status = sslab_run_rendered_frame(context, &run, frame_index, rgba, stride);
    }
    if (status == SSLAB_STATUS_OK) {
        sslab_sha256_hex(rgba, byte_count, receipt->rgba_sha256);
        receipt->frame_index = frame_index;
        receipt->width = profile->width;
        receipt->height = profile->height;
    }
    receipt->status = status;
    free(rgba);
    return status;
}

sslab_status sslab_run_lifecycle(
    sslab_context *context,
    const sslab_lifecycle_desc *desc,
    sslab_lifecycle_receipt *receipt)
{
    sslab_product *product;
    sslab_session *session;
    sslab_surface_desc surface;
    sslab_status status;
    unsigned char *rgba;
    unsigned long stride;
    unsigned long byte_count;
    sslab_u32 step_index;
    sslab_u32 cycle_index;
    sslab_u32 render_width;
    sslab_u32 render_height;

    if (context == 0 || desc == 0 || receipt == 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    memset(receipt, 0, sizeof(*receipt));
    receipt->size = sizeof(*receipt);
    receipt->abi_version = SSLAB_ABI_VERSION;
    receipt->status = SSLAB_STATUS_FAIL;
    status = sslab_validate_desc(desc->size, sizeof(*desc), desc->abi_version);
    if (status != SSLAB_STATUS_OK) {
        receipt->status = status;
        return status;
    }
    status = sslab_validate_desc(desc->run.size, sizeof(desc->run), desc->run.abi_version);
    if (status != SSLAB_STATUS_OK) {
        receipt->status = status;
        return status;
    }

    render_width = desc->resize_width != 0UL ? desc->resize_width : desc->run.width;
    render_height = desc->resize_height != 0UL ? desc->resize_height : desc->run.height;
    product = 0;
    session = 0;
    rgba = 0;

    status = sslab_open_product(context, desc->run.product_key, &product);
    if (status == SSLAB_STATUS_OK) {
        status = sslab_create_session(product, &desc->run, &session);
    }
    if (status == SSLAB_STATUS_OK) {
        receipt->create_session = 1UL;
        if (desc->resize_width != 0UL && desc->resize_height != 0UL) {
            status = sslab_resize_session(session, desc->resize_width, desc->resize_height);
            receipt->resize_session = status == SSLAB_STATUS_OK ? 1UL : 0UL;
        }
    }
    for (step_index = 0UL; status == SSLAB_STATUS_OK && step_index < desc->run.frame_count; ++step_index) {
        status = sslab_step_session(session, desc->run.delta_ms);
        if (status == SSLAB_STATUS_OK) {
            ++receipt->step_count;
        }
    }
    if (status == SSLAB_STATUS_OK) {
        status = sslab_alloc_rgba(render_width, render_height, &rgba, &stride, &byte_count);
    }
    if (status == SSLAB_STATUS_OK) {
        surface.size = sizeof(surface);
        surface.abi_version = SSLAB_ABI_VERSION;
        surface.width = render_width;
        surface.height = render_height;
        surface.stride = stride;
        surface.rgba = rgba;
        status = sslab_render_session(session, &surface);
        receipt->render_session = status == SSLAB_STATUS_OK ? 1UL : 0UL;
    }
    if (status == SSLAB_STATUS_OK) {
        receipt->checksum = sslab_checksum_rgba(rgba, byte_count);
    }
    sslab_destroy_session(session);
    receipt->destroy_session = 1UL;
    session = 0;

    for (cycle_index = 0UL; status == SSLAB_STATUS_OK && cycle_index < desc->create_destroy_cycles; ++cycle_index) {
        status = sslab_create_session(product, &desc->run, &session);
        sslab_destroy_session(session);
        session = 0;
        if (status == SSLAB_STATUS_OK) {
            ++receipt->create_destroy_cycles;
        }
    }

    sslab_close_product(product);
    free(rgba);
    receipt->status = status;
    return status;
}

sslab_status sslab_run_profile(
    sslab_context *context,
    const sslab_profile_desc *desc,
    sslab_profile_receipt *receipt)
{
    sslab_product *product;
    sslab_session *session;
    sslab_surface_desc surface;
    sslab_status status;
    unsigned char *rgba;
    unsigned long stride;
    unsigned long byte_count;
    double *samples;
    sslab_u32 frame_index;
    clock_t start_clock;
    clock_t end_clock;

    if (context == 0 || desc == 0 || receipt == 0) {
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }
    memset(receipt, 0, sizeof(*receipt));
    receipt->size = sizeof(*receipt);
    receipt->abi_version = SSLAB_ABI_VERSION;
    receipt->status = SSLAB_STATUS_FAIL;
    status = sslab_validate_desc(desc->size, sizeof(*desc), desc->abi_version);
    if (status != SSLAB_STATUS_OK) {
        receipt->status = status;
        return status;
    }
    status = sslab_validate_desc(desc->run.size, sizeof(desc->run), desc->run.abi_version);
    if (status != SSLAB_STATUS_OK) {
        receipt->status = status;
        return status;
    }
    if (desc->measured_frames == 0UL) {
        receipt->status = SSLAB_STATUS_INVALID_ARGUMENT;
        return SSLAB_STATUS_INVALID_ARGUMENT;
    }

    product = 0;
    session = 0;
    rgba = 0;
    samples = (double *)calloc((size_t)desc->measured_frames, sizeof(*samples));
    if (samples == 0) {
        receipt->status = SSLAB_STATUS_OUT_OF_MEMORY;
        return SSLAB_STATUS_OUT_OF_MEMORY;
    }
    status = sslab_alloc_rgba(desc->run.width, desc->run.height, &rgba, &stride, &byte_count);
    (void)byte_count;
    if (status == SSLAB_STATUS_OK) {
        status = sslab_open_product(context, desc->run.product_key, &product);
    }
    if (status == SSLAB_STATUS_OK) {
        status = sslab_create_session(product, &desc->run, &session);
    }

    surface.size = sizeof(surface);
    surface.abi_version = SSLAB_ABI_VERSION;
    surface.width = desc->run.width;
    surface.height = desc->run.height;
    surface.stride = stride;
    surface.rgba = rgba;

    for (frame_index = 0UL; status == SSLAB_STATUS_OK && frame_index < desc->warmup_frames; ++frame_index) {
        status = sslab_step_session(session, desc->run.delta_ms);
        if (status == SSLAB_STATUS_OK) {
            status = sslab_render_session(session, &surface);
        }
    }
    for (frame_index = 0UL; status == SSLAB_STATUS_OK && frame_index < desc->measured_frames; ++frame_index) {
        start_clock = clock();
        status = sslab_step_session(session, desc->run.delta_ms);
        if (status == SSLAB_STATUS_OK) {
            status = sslab_render_session(session, &surface);
        }
        end_clock = clock();
        if (status == SSLAB_STATUS_OK) {
            samples[frame_index] = ((double)(end_clock - start_clock) * 1000.0) / (double)CLOCKS_PER_SEC;
            ++receipt->measured_frames;
        }
    }
    for (frame_index = 0UL; status == SSLAB_STATUS_OK && frame_index < desc->short_soak_frames; ++frame_index) {
        status = sslab_step_session(session, desc->run.delta_ms);
        if (status == SSLAB_STATUS_OK) {
            status = sslab_render_session(session, &surface);
            ++receipt->short_soak_frames;
        }
    }
    if (status == SSLAB_STATUS_OK) {
        sslab_sort_doubles(samples, receipt->measured_frames);
        receipt->frame_time_ms_p50 = sslab_percentile(samples, receipt->measured_frames, 50UL);
        receipt->frame_time_ms_p95 = sslab_percentile(samples, receipt->measured_frames, 95UL);
        receipt->frame_time_ms_p99 = sslab_percentile(samples, receipt->measured_frames, 99UL);
    }

    sslab_destroy_session(session);
    sslab_close_product(product);
    free(samples);
    free(rgba);
    receipt->status = status;
    return status;
}
