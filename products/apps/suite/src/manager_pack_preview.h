#ifndef MANAGER_PACK_PREVIEW_H
#define MANAGER_PACK_PREVIEW_H

#define MANAGER_PACK_PREVIEW_TEXT 128U
#define MANAGER_PACK_PREVIEW_PATH 260U
#define MANAGER_PACK_PREVIEW_BOUNDARY 256U

typedef struct manager_pack_preview_entry_tag {
    char package_id[MANAGER_PACK_PREVIEW_TEXT];
    char product_key[32];
    char profile_key[MANAGER_PACK_PREVIEW_TEXT];
    char pack_manifest_ref[MANAGER_PACK_PREVIEW_PATH];
    char provenance[MANAGER_PACK_PREVIEW_TEXT];
    char license[MANAGER_PACK_PREVIEW_TEXT];
    char product_compatibility[MANAGER_PACK_PREVIEW_TEXT];
    char installability_status[32];
    char refusal_reason[MANAGER_PACK_PREVIEW_TEXT];
    char proof_bundle_ref[MANAGER_PACK_PREVIEW_PATH];
    char rollback_note[MANAGER_PACK_PREVIEW_TEXT];
    char claim_boundary[MANAGER_PACK_PREVIEW_BOUNDARY];
    int install_mutation_allowed;
} manager_pack_preview_entry;

int manager_pack_preview_plasma_v2(manager_pack_preview_entry *entry);
int manager_pack_preview_is_installable(const manager_pack_preview_entry *entry);
const char *manager_pack_preview_refusal_reason(const manager_pack_preview_entry *entry);

#endif /* MANAGER_PACK_PREVIEW_H */
