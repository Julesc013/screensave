#ifndef PLASMA_V3_PROOF_H
#define PLASMA_V3_PROOF_H

#include "../plasma_v3_types.h"

ss_u32 plasma_v3_proof_signature(
    const ss_u8 *bytes,
    ss_u32 byte_count,
    plasma_v3_signature *signature_out
);
ss_u32 plasma_v3_proof_difference_count(const ss_u8 *left, const ss_u8 *right, ss_u32 byte_count);

#endif /* PLASMA_V3_PROOF_H */
