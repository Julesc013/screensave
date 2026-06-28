# Plasma v2 Instrument Re-Audit

- Status: hold
- Candidate: plasma-v2-rc1
- Recommended state: release-candidate-hold
- Instrument architecture: promotion-ready
- Stable eligible: True
- Claim boundary: PAW-I-R2 instrument re-audit only; it does not publish, certify compatibility, accept artistic quality, or promote stable release.

## Blocking Gates

- none

## Remaining Project Blockers

- final stable artistic acceptance

## Checks

- pass: instrument-audit-rerun - Authoritative Plasma v2 instrument audit reran for PAW-I-R2.
- pass: instrument-audit-outcome - Instrument audit is either promotion-ready or an explicit non-promotion hold.
- pass: gate:aide_not_runtime_or_truth - Required instrument gate passes.
- pass: gate:direct_controls_influence_passes - Required instrument gate passes.
- pass: gate:field_pipeline_boundaries_pass - Required instrument gate passes.
- pass: gate:gl11_is_not_hidden_minimum - Required instrument gate passes.
- pass: gate:material_mapping_distinctness_passes - Required instrument gate passes.
- pass: gate:packc_data_only_passes - Required instrument gate passes.
- pass: gate:plasma_v2_plan_contract_passes - Required instrument gate passes.
- pass: gate:plasma_v2_runtime_contract_passes - Required instrument gate passes.
- pass: gate:plasma_v2_spec_contract_passes - Required instrument gate passes.
- pass: gate:software_reference_is_canonical - Required instrument gate passes.
- pass: gate:treatment_boundaries_pass - Required instrument gate passes.
- pass: gate:workbench_inspection_passes - Required instrument gate passes.
- pass: gate:legacy_preset_authority_removed - Gate is either admitted for stable promotion or explicitly blocking the hold.
- pass: gate:visualintent_candidates_reduce_to_plasma_spec - Gate is either admitted for stable promotion or explicitly blocking the hold.
- pass: no-generic-command-capability - Project adapter exposes no generic command, publication, unchecked promotion, or agent-apply capability.
- pass: no-stable-promotion-yet - Project state remains a release-candidate hold until the stable-promotion gate admits promotion.
