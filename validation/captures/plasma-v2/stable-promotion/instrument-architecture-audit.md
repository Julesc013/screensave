# Plasma v2 Instrument-Architecture Audit

- Status: hold
- Candidate: plasma-v2-rc1
- Stable eligible: False
- Recommended state: release-candidate-hold
- Claim boundary: Instrument-architecture audit only; it does not publish Plasma, certify compatibility, accept final artistic quality, or admit AIDE as runtime truth.

## Blocking Gates

- plasma_v2_plan_contract_passes
- plasma_v2_runtime_contract_passes
- legacy_preset_authority_removed
- field_pipeline_boundaries_pass
- visualintent_candidates_reduce_to_plasma_spec

## Gates

- pass: plasma_v2_spec_contract_passes - The direct-control Plasma v2 spec island exists and passes its C89 smoke check.
- hold: plasma_v2_plan_contract_passes - The direct v2 plan contract must exist in the v2 island before stable promotion.
- hold: plasma_v2_runtime_contract_passes - The direct v2 runtime contract must exist in the v2 island before stable promotion.
- hold: legacy_preset_authority_removed - Legacy preset/theme files remain compatibility evidence; PAW-I must prove they are migration inputs, not hidden runtime authority.
- pass: direct_controls_influence_passes - Basic controls with visual claims produce deterministic output differences.
- hold: field_pipeline_boundaries_pass - Field and output boundaries must be explicit in the v2 island before stable promotion.
- pass: material_mapping_distinctness_passes - Stable materials have distinct deterministic visual response.
- pass: treatment_boundaries_pass - Treatment remains post-material and bounded for the stable slice.
- pass: software_reference_is_canonical - Software/reference execution remains the canonical proof-bearing path.
- pass: gl11_is_not_hidden_minimum - GDI remains the floor and GL11 remains optional.
- pass: packc_data_only_passes - packc validates data-only Plasma packs and rejects executable or unsafe payloads.
- hold: visualintent_candidates_reduce_to_plasma_spec - VisualIntent is contractually descriptive, but a Plasma v2 candidate resolver to the direct spec is not yet admitted as stable evidence.
- pass: aide_not_runtime_or_truth - AIDE remains an evidence and repair coordinator, not product runtime or truth authority.

## Structural Checks

- pass: path:contracts/plasma_instrument_architecture_v1.md - Required audit authority path exists.
- pass: path:contracts/plasma_stable_promotion_v1.md - Required audit authority path exists.
- pass: path:docs/roadmap/paw-i-plasma-v2-stable-promotion.md - Required audit authority path exists.
- pass: instrument-contract-text - Instrument contract records the product thesis, pipeline, and blocking gates.
- pass: stable-contract-fold-in - Stable-promotion contract requires the instrument audit.
- pass: paw-i-fold-in - PAW-I roadmap records the instrument audit as a promotion gate.
- pass: direct-spec - Command passed.
- pass: legacy-core-boundaries - Command passed.
- pass: direct-control-influence - Command passed.
- pass: material-treatment - Command passed.
- pass: packc-data-only - Command passed.
- pass: visualintent-contract - Command passed.
- pass: aide-boundary - Command passed.
- pass: required-gate-set - Instrument audit emits the required PAW-I gate set.
- pass: project-state-not-overpromoted - Project state does not overclaim stable promotion.
