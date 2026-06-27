# PAW-I Plasma v2 Stable Promotion And Instrument Architecture Audit

PAW-I starts from Plasma v2 release candidate `plasma-v2-rc1` at commit
`c8b5ecf180f3b25adf7fdd2ca67dcd6693b90409`.

This is the first wave allowed to decide whether Plasma v2 becomes stable. The
decision is still gated: stable promotion requires package, proof, support,
security, provenance, Manager, Workbench, final artistic acceptance, repair
queue evidence, and a blocking instrument-architecture audit.

PAW-I must not merely ask whether `plasma-v2-rc1` passes validators. It must
ask whether the release candidate embodies the Plasma v2 product architecture:

```text
direct controls
-> plasma_v2_spec
-> plasma_v2_plan
-> plasma_v2_runtime
-> field/output/material/treatment/presentation
-> proof bundle
```

The product thesis is:

```text
Plasma is not a preset picker. Plasma is a visual instrument.
```

## Intake State

- Active program: `plasma-v2-stable-promotion`
- Plasma v2 status: `release-candidate`
- Release candidate: `plasma-v2-rc1`
- Stable: `false`
- Release promotion: `blocked`
- Active proof profile: `plasma.v2.reference.preview`

## Allowed Outcomes

- `stable-promoted`
- `request-changes`
- `defer-to-labs`
- `release-candidate-hold`

## Instrument Architecture Gates

PAW-I stable promotion also requires these separate facts:

- plasma_v2_spec_contract_passes
- plasma_v2_plan_contract_passes
- plasma_v2_runtime_contract_passes
- legacy_preset_authority_removed
- direct_controls_influence_passes
- field_pipeline_boundaries_pass
- material_mapping_distinctness_passes
- treatment_boundaries_pass
- software_reference_is_canonical
- gl11_is_not_hidden_minimum
- packc_data_only_passes
- visualintent_candidates_reduce_to_plasma_spec
- aide_not_runtime_or_truth

Missing instrument facts hold the candidate for repair. They do not become
publication evidence, compatibility certification, or automatic stable
promotion.

## Boundary

If stable promotion passes, PAW-I may set `stable = true` and
`release_promotion = "accepted"`. Public release upload or publication remains
separately gated unless an explicit publication-prep packet is added.

## PAW-I Decision

PAW-I ended in `release-candidate-hold`, not stable promotion.

The stable-promotion gate remains structurally runnable, but the final stable
artistic decision remained `request-changes` and the instrument-architecture
audit is now a distinct promotion requirement. The stable-promotion report must
recommend `release-candidate-hold` unless both final artistic acceptance and
instrument architecture are ready. It keeps `stable = false` with
`release_promotion = "blocked"`.

## Repair Handoff

- Next active program: `plasma-v2-instrument-repair`
- Blocking WorkUnit: `SS-PLV2-I-REPAIR-001`
- Gate report:
  `validation/captures/plasma-v2/stable-promotion/gate-report.json`
- Instrument architecture audit:
  `validation/captures/plasma-v2/stable-promotion/instrument-architecture-audit.json`
- Repair burndown:
  `validation/captures/plasma-v2/stable-promotion/repair-burndown.json`

No publication packet, broad compatibility certification, public SDK stability
claim, automatic AIDE promotion, or all-saver migration was admitted.
