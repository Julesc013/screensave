# Plasma v2 Legacy Authority Report

- Status: blocked
- Claim boundary: Legacy content is migration input only; Plasma v2 spec/plan/runtime own execution truth.

## Blocking Reasons

- workbench_legacy_migration_facts_missing

## Authority Groups

### legacy_inputs
- present: products/savers/plasma/src/plasma_config.c
- present: products/savers/plasma/src/plasma_presets.c
- present: products/savers/plasma/src/plasma_themes.c
- present: products/savers/plasma/src/plasma_content.c
- present: products/savers/plasma/src/plasma_selection.c
- present: products/savers/plasma/src/plasma_benchlab.c
- present: products/savers/plasma/presets
- missing: products/savers/plasma/themes (optional)

### v2_authority_files
- present: products/savers/plasma/src/v2/plasma_v2_spec.h
- present: products/savers/plasma/src/v2/plasma_v2_spec.c
- present: products/savers/plasma/src/v2/plasma_v2_plan.h
- present: products/savers/plasma/src/v2/plasma_v2_plan.c
- present: products/savers/plasma/src/v2/plasma_v2_runtime.h
- present: products/savers/plasma/src/v2/plasma_v2_runtime.c
- present: products/savers/plasma/src/v2/plasma_v2_migration.h
- present: products/savers/plasma/src/v2/plasma_v2_migration.c
- present: products/savers/plasma/src/v2/field
- present: products/savers/plasma/src/v2/output

### compatibility_shims
- present: products/savers/plasma/src/plasma_config.c
- present: products/savers/plasma/src/plasma_presets.c
- present: products/savers/plasma/src/plasma_themes.c
- present: products/savers/plasma/src/plasma_content.c
- present: products/savers/plasma/src/plasma_selection.c
- present: products/savers/plasma/src/plasma_benchlab.c

### bypass_candidates
- present: products/savers/plasma/src/plasma_sim.c
- present: products/savers/plasma/src/plasma_render.c
- present: products/savers/plasma/src/plasma_plan.c
- present: products/savers/plasma/src/plasma_advanced.c
- present: products/savers/plasma/src/plasma_modern.c
- present: products/savers/plasma/src/plasma_premium.c
- present: products/savers/plasma/src/plasma_output.c
- present: products/savers/plasma/src/plasma_treatment.c
- present: products/savers/plasma/src/plasma_presentation.c
