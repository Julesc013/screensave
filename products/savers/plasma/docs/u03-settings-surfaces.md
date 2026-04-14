# U03 Settings Surfaces

The Plasma dialog now matches the layered salvage model instead of exposing a flat pile of mixed-scope controls.

## Surface Behavior

- the surface selector still switches between `Basic`, `Advanced`, and `Author/Lab`
- each dialog binding now follows the setting descriptor surface instead of hand-curated exceptions
- `Advanced` includes `Basic` plus grammar controls
- `Author/Lab` includes the full bounded product-local curation and transition layer

## Control Truth

- output-family and output-mode are now real dialog controls
- output-mode choices are filtered by the currently selected output family so the UI does not offer impossible pairings
- filter, emulation, accent, and presentation are now explicit dialog controls backed by persisted config
- transition policy moved out of the first two layers and into `Author/Lab`
- content-pool control moved up into `Basic`
- `sampling_treatment` stayed off the dialog because it is still not a supportable user-facing surface

## Preset Semantics

Preset selection still reapplies the curated preset bundle on purpose.
That keeps presets as intentional starting points while allowing later explicit advanced overrides to persist and compile truthfully.

## Proof

Smoke now asserts the intended descriptor-to-surface mapping for:

- `detail_level`
- `content_filter`
- `transitions_enabled`
- `output_family`
- `transition_policy`
- `preset_set_key`

That gives the layered model a regression bar even without a separate dialog-automation harness.
