#include "../src/manager_pack_preview.h"

#include <string.h>

int main(void)
{
    manager_pack_preview_entry entry;

    if (!manager_pack_preview_plasma_v2(&entry)) {
        return 1;
    }
    if (strcmp(entry.product_key, "plasma") != 0) {
        return 2;
    }
    if (strcmp(entry.profile_key, "plasma.v2.reference.preview") != 0) {
        return 3;
    }
    if (entry.provenance[0] == '\0' || entry.license[0] == '\0') {
        return 4;
    }
    if (entry.proof_bundle_ref[0] == '\0' || entry.rollback_note[0] == '\0') {
        return 5;
    }
    if (manager_pack_preview_is_installable(&entry)) {
        return 6;
    }
    if (strstr(manager_pack_preview_refusal_reason(&entry), "not admitted") == 0) {
        return 7;
    }
    if (strstr(entry.claim_boundary, "not installation") == 0) {
        return 8;
    }

    return 0;
}
