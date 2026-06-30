#include "plasma_v3_proof.h"

ss_u32 plasma_v3_proof_signature(
    const ss_u8 *bytes,
    ss_u32 byte_count,
    plasma_v3_signature *signature_out
)
{
    ss_u32 index;
    ss_u32 hash;
    ss_u32 lit;

    if (bytes == 0 || signature_out == 0 || byte_count == 0U) {
        return SS_V2_STATUS_BAD_ARGUMENT;
    }
    hash = 2166136261U;
    lit = 0U;
    for (index = 0U; index < byte_count; ++index) {
        hash ^= (ss_u32)bytes[index];
        hash *= 16777619U;
        if ((index % PLASMA_V3_PRESENT_PIXEL_BYTES) != 3U && bytes[index] > 8U) {
            ++lit;
        }
    }
    signature_out->struct_size = (ss_u32)sizeof(*signature_out);
    signature_out->hash = hash;
    signature_out->lit_pixels = lit / 3U;
    signature_out->byte_count = byte_count;
    return SS_V2_STATUS_OK;
}

ss_u32 plasma_v3_proof_difference_count(const ss_u8 *left, const ss_u8 *right, ss_u32 byte_count)
{
    ss_u32 index;
    ss_u32 count;

    if (left == 0 || right == 0) {
        return 0U;
    }
    count = 0U;
    for (index = 0U; index < byte_count; ++index) {
        if (left[index] != right[index]) {
            ++count;
        }
    }
    return count;
}
