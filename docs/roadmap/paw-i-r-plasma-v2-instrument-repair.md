# PAW-I-R Plasma v2 Instrument Repair

PAW-I-R starts from the pushed `428266b7dee3ee02269b603d714b47156c93f960`
state where Plasma v2 is held:

```text
status = release-candidate-hold
stable = false
release_promotion = blocked
instrument architecture audit = active blocker
```

This is a product-center correction wave, not a feature wave and not another
broad roadmap layer. It exists to repair the instrument gates that prevent
`plasma-v2-rc1` from being promoted.

## Doctrine

The north star remains:

```text
Portable meaning.
Native delivery.
Deterministic proof.
Optional automation.
```

For Plasma v2, that means:

```text
direct controls first
presets second
software/reference truth first
optional acceleration second
```

Plasma is not a preset picker. Plasma is a visual instrument.

## Scope

PAW-I-R may repair the current release-candidate hold by proving that Plasma v2
is a deterministic modular field instrument:

```text
stored config / preset / pack / VisualIntent
-> plasma_v2_spec
-> plasma_v2_plan
-> plasma_v2_runtime
-> providers / analytic sources
-> field generators
-> field modifiers
-> output transform
-> material mapping
-> treatment stack
-> presentation
-> surface / renderer / presenter
```

The stable realization remains conservative:

```text
fixed-point field
-> material map to screensave_visual_buffer
-> optional treatment
-> flat present buffer
-> screensave_renderer_blit_bitmap
-> GDI or optional GL11
```

PAW-I-R must not admit GL46, heightfield, ribbon, glyph, arbitrary graph
runtime, new preset breadth that hides weak controls, publication, broad OS
certification, or AIDE runtime truth.

## Current Blocking Gates

The active instrument audit currently holds on:

- `plasma_v2_plan_contract_passes`
- `plasma_v2_runtime_contract_passes`
- `legacy_preset_authority_removed`
- `field_pipeline_boundaries_pass`
- `visualintent_candidates_reduce_to_plasma_spec`

The following gates already have passing evidence and must stay green:

- `plasma_v2_spec_contract_passes`
- `direct_controls_influence_passes`
- `material_mapping_distinctness_passes`
- `treatment_boundaries_pass`
- `software_reference_is_canonical`
- `gl11_is_not_hidden_minimum`
- `packc_data_only_passes`
- `aide_not_runtime_or_truth`

## Commit Train

The intended bounded train is:

```text
state: checkpoint Plasma v2 instrument hold
plasma: codify v2 instrument constitution
plasma: enforce v2 product-center boundaries
plasma: repair v2 spec as direct-control authority
plasma: repair v2 plan compiler and degradation law
plasma: repair deterministic v2 runtime buffers
plasma: repair v2 field pipeline stages
proof: repair Plasma v2 direct-control influence matrix
plasma: repair v2 material response and distinctness
plasma: repair legacy preset migration into v2 specs
proof: repair Plasma v2 reference and GL11 optionality evidence
workbench: repair Plasma v2 instrument inspection
aide: record Plasma instrument repair evidence
gate: rerun Plasma v2 instrument architecture audit
gate: rerun Plasma v2 stable promotion after instrument repairs
state: promote Plasma v2 to stable
```

If final gates still fail, replace the final promotion commit with:

```text
state: keep Plasma v2 on release-candidate hold
```

## State Rule

PAW-I-R may move Plasma from `release-candidate-hold` to one of:

- `release-candidate-restored`
- `stable-promoted`
- `request-changes`
- `defer-to-labs`

Stable promotion is allowed only after both:

- `py -3 tools/scripts/check_plasma_instrument_architecture.py`
- `py -3 tools/scripts/check_plasma_v2_stable_promotion.py`

report promotion readiness. Until then:

```toml
[plasma_v2]
status = "release-candidate-hold"
stable = false
release_promotion = "blocked"
opened_next = "plasma-v2-instrument-repair"
```

## AIDE Boundary

AIDE may provide WorkUnits, preflight context, proposal-only agent sessions,
repair queue records, EvidencePackets, postflight receipts, and drift reports.

AIDE may not mutate source autonomously, self-approve repairs, merge, publish,
certify compatibility, accept final artistic quality, or become saver runtime
truth.
