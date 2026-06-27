# Plasma v2 Legacy Boundary Report

- Status: hold
- Legacy authority removed: False
- Claim boundary: Legacy boundary audit only; hold means old Plasma files are not yet proven to be wrappers or migration-only inputs to the direct v2 instrument center.

## Blocking Reasons

- legacy_bypass_candidates_not_wrapped_over_v2
- compatibility_shims_not_explicitly_wrapped_over_v2

## Compatibility Shims

- products/savers/plasma/src/plasma_config.c: references_v2=False
- products/savers/plasma/src/plasma_presets.c: references_v2=False
- products/savers/plasma/src/plasma_themes.c: references_v2=False
- products/savers/plasma/src/plasma_content.c: references_v2=False
- products/savers/plasma/src/plasma_selection.c: references_v2=False
- products/savers/plasma/src/plasma_benchlab.c: references_v2=False

## Legacy Bypass Candidates

- products/savers/plasma/src/plasma_sim.c: references_v2=False
- products/savers/plasma/src/plasma_render.c: references_v2=False
- products/savers/plasma/src/plasma_plan.c: references_v2=False
- products/savers/plasma/src/plasma_advanced.c: references_v2=False
- products/savers/plasma/src/plasma_modern.c: references_v2=False
- products/savers/plasma/src/plasma_premium.c: references_v2=False
- products/savers/plasma/src/plasma_output.c: references_v2=False
- products/savers/plasma/src/plasma_treatment.c: references_v2=False
- products/savers/plasma/src/plasma_presentation.c: references_v2=False
