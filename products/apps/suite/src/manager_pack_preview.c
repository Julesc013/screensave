#include "manager_pack_preview.h"

#include <string.h>

static void manager_pack_preview_copy(char *target, unsigned int target_size, const char *source)
{
    unsigned int index;

    if (target == 0 || target_size == 0U) {
        return;
    }
    if (source == 0) {
        target[0] = '\0';
        return;
    }

    index = 0U;
    while (index + 1U < target_size && source[index] != '\0') {
        target[index] = source[index];
        ++index;
    }
    target[index] = '\0';
}

int manager_pack_preview_plasma_v2(manager_pack_preview_entry *entry)
{
    if (entry == 0) {
        return 0;
    }

    memset(entry, 0, sizeof(*entry));
    manager_pack_preview_copy(entry->package_id, sizeof(entry->package_id), "screensave.plasma-v2.preview-stage");
    manager_pack_preview_copy(entry->product_key, sizeof(entry->product_key), "plasma");
    manager_pack_preview_copy(entry->profile_key, sizeof(entry->profile_key), "plasma.v2.reference.preview");
    manager_pack_preview_copy(
        entry->pack_manifest_ref,
        sizeof(entry->pack_manifest_ref),
        "products/savers/plasma/content/v2/examples/plasma_lava_v2.toml");
    manager_pack_preview_copy(entry->provenance, sizeof(entry->provenance), "PAW-DX U09 deterministic migration");
    manager_pack_preview_copy(entry->license, sizeof(entry->license), "ScreenSave sample content");
    manager_pack_preview_copy(entry->product_compatibility, sizeof(entry->product_compatibility), "plasma_spec_v2 schema 2 only");
    manager_pack_preview_copy(entry->installability_status, sizeof(entry->installability_status), "preview-only");
    manager_pack_preview_copy(entry->refusal_reason, sizeof(entry->refusal_reason), "real pack installation mutation is not admitted");
    manager_pack_preview_copy(
        entry->proof_bundle_ref,
        sizeof(entry->proof_bundle_ref),
        "validation/captures/plasma-v2/reference-preview/proof-bundle-v1.json");
    manager_pack_preview_copy(entry->rollback_note, sizeof(entry->rollback_note), "remove staged preview refs; no install state is changed");
    manager_pack_preview_copy(
        entry->claim_boundary,
        sizeof(entry->claim_boundary),
        "Manager preview only; not installation, release promotion, compatibility certification, or stable release.");
    entry->install_mutation_allowed = 0;
    return 1;
}

int manager_pack_preview_is_installable(const manager_pack_preview_entry *entry)
{
    if (entry == 0) {
        return 0;
    }
    return entry->install_mutation_allowed != 0 && strcmp(entry->installability_status, "installable") == 0;
}

const char *manager_pack_preview_refusal_reason(const manager_pack_preview_entry *entry)
{
    if (entry == 0) {
        return "missing preview entry";
    }
    if (manager_pack_preview_is_installable(entry)) {
        return "";
    }
    if (entry->refusal_reason[0] == '\0') {
        return "pack is not installable in preview mode";
    }
    return entry->refusal_reason;
}
