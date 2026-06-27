# Plasma v2 Product-Center Boundary Report

- Status: hold
- Stable eligible: False
- Claim boundary: Product-center boundary audit only; hold means legacy files are not yet proven to be wrappers over the direct v2 instrument center.

## Blocking Reasons

- new_product_center_incomplete
- legacy_bypass_candidates_still_present

## File Groups

### new_product_center
- present: products/savers/plasma/src/v2/plasma_v2_spec.h
- present: products/savers/plasma/src/v2/plasma_v2_spec.c
- missing: products/savers/plasma/src/v2/plasma_v2_plan.h
- missing: products/savers/plasma/src/v2/plasma_v2_plan.c
- missing: products/savers/plasma/src/v2/plasma_v2_runtime.h
- missing: products/savers/plasma/src/v2/plasma_v2_runtime.c
- missing: products/savers/plasma/src/v2/field
- missing: products/savers/plasma/src/v2/output

### compatibility_shims
- present: products/savers/plasma/src/plasma_config.c
- present: products/savers/plasma/src/plasma_presets.c
- present: products/savers/plasma/src/plasma_themes.c
- present: products/savers/plasma/src/plasma_content.c
- present: products/savers/plasma/src/plasma_selection.c
- present: products/savers/plasma/src/plasma_benchlab.c

### legacy_bypass_candidates
- present: products/savers/plasma/src/plasma_sim.c
- present: products/savers/plasma/src/plasma_render.c
- present: products/savers/plasma/src/plasma_plan.c
- present: products/savers/plasma/src/plasma_advanced.c
- present: products/savers/plasma/src/plasma_modern.c
- present: products/savers/plasma/src/plasma_premium.c
- present: products/savers/plasma/src/plasma_output.c
- present: products/savers/plasma/src/plasma_treatment.c
- present: products/savers/plasma/src/plasma_presentation.c
