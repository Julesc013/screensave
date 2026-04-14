# Plasma U00 Settings Influence Matrix

## Purpose

This matrix records what the current settings surface actually controls in the
current repository snapshot.

It is not a wishlist.
It is the blunt map later corrective work must use when deciding what to keep,
hide, demote, or remove.

## Current Settings Truth

In the current snapshot, most surfaced grammar controls are no longer
preset-hidden.
That is real progress.

The remaining settings problems are narrower and more specific:

- some controls are only indirectly visual or only visible over long runs
- some controls are conditionally available and effectively dead outside their
  enabling context
- some unsupported breadth still survives in config or enum vocabulary even
  though it is not a truthful surfaced setting
- some authoring and curation controls are still present in the product dialog
  even though they are not normal day-to-day settings

## Influence Matrix

| Setting Or Family | Affects Plan | Affects Rendered Output | Visually Meaningful | Preset-Authority-Dominated | Exposed In UI | Later Action | Rationale |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `preset_key` | Yes | Yes | Yes | Partial | Yes | Keep | Preset identity is still a real product control, but it is now a curated bundle and content identity rather than the hidden final grammar owner. |
| `theme_key` | Yes | Yes | Yes | No | Yes | Keep | Theme still materially changes palette and room mood. |
| `speed_mode` | Yes | Yes | Yes | No | Yes | Keep | Real motion pacing change. |
| `detail_level` | Yes | Yes | Yes | No | Yes | Keep | Real density and intensity change. |
| `effect_mode` | Yes | Yes | Yes | No | Yes | Keep | Real generator-family control. |
| `resolution_mode` | Yes | Yes | Yes | No | Yes | Keep | Real field-density change. |
| `smoothing_mode` | Yes | Yes | Yes | No | Yes | Keep | Real smoothing treatment choice. |
| `output_family` | Yes | Yes | Yes | No | Yes | Keep | Real grammar owner now; no longer a preset-hidden sidecar. |
| `output_mode` | Yes | Yes | Yes | No | Yes | Keep | Real mode selection inside the active output family. |
| `filter_treatment` | Yes | Yes | Yes | No | Yes | Keep | Real surfaced treatment slot. |
| `emulation_treatment` | Yes | Yes | Yes | No | Yes | Keep | Real surfaced display-emulation slot. |
| `accent_treatment` | Yes | Yes | Yes | No | Yes | Keep | Real surfaced accent slot, though some choices remain more bounded than others. |
| `presentation_mode` | Yes | Yes | Yes | No | Yes | Keep | Real presentation owner now, but some modes remain experimentally bounded and lane-sensitive. |
| `content_filter` | Indirect | Indirect | Conditional | No | Yes | Keep | Truthful stable versus experimental pool control, but it changes selection posture rather than every currently active frame. |
| `transitions_enabled` | Yes | Conditional | Conditional | No | Yes | Keep | Real long-run behavior switch when a valid transition surface exists; not a direct one-frame look control. |
| `transition_policy` | Yes | Conditional | Conditional | No | Yes | Demote to lab-only | Truthful but authoring-grade; visible effect only over time and only when transitions are enabled. |
| `journey_key` | Yes | Conditional | Conditional | No | Yes | Demote to lab-only | Real only when transitions are enabled and journey routing is active. |
| `transition_fallback_policy` | Yes | Conditional | Conditional | No | Yes | Demote to lab-only | A real support control, not a normal end-user visual control. |
| `transition_seed_policy` | Yes | Conditional | Conditional | No | Yes | Demote to lab-only | Real but authoring-grade and only relevant during transitions. |
| `transition_interval_millis` | Yes | Conditional | Weak | No | Yes | Demote to lab-only | Real only as long-run cadence tuning. |
| `transition_duration_millis` | Yes | Conditional | Weak | No | Yes | Demote to lab-only | Real only as transition timing envelope tuning. |
| `use_deterministic_seed` | Yes | Yes | Conditional | No | Yes | Keep | Real reproducibility switch; visually meaningful when comparing or validating. |
| `deterministic_seed` | Yes | Yes | Conditional | No | Yes | Demote to lab-only | Real, but only when deterministic mode is enabled and mainly useful for proof and authoring. |
| `preset_set_key` | Yes | Indirect | Conditional | No | Yes | Demote to lab-only | Real curation control, not a primary visual grammar control. |
| `theme_set_key` | Yes | Indirect | Conditional | No | Yes | Demote to lab-only | Same as preset-set routing. |
| `favorites_only` | Indirect | Indirect | Weak | No | Yes | Demote to lab-only | Real only when favorites are configured; the dialog still does not expose favorite editing directly. |
| `diagnostics_overlay_enabled` | No | No | No | No | Yes | Demote to lab-only | Diagnostics-only; not part of visual grammar. |
| `settings_surface` | No | No | No | No | Yes | Keep | UI-layer control only. It is honest, but it does not change the runtime result. |
| `favorite_*` and `excluded_*` key lists | Indirect | Indirect | Conditional | No | No | Keep | Real authored selection controls, but file-first and not dialog-owned. |
| `sampling_treatment` | Yes after import, then clamped | No | No | No | No | Hide | Exists in config and export vocabulary but still clamps to `none`. It is not a truthful product control. |
| unsupported output and treatment enums | No in supported paths | No | No | No | No | Remove | Surviving enum or parser vocabulary is paper breadth and should not be allowed to look like real product authority. |

## Dead Or Conditionally Dead Combinations

The following combinations are not fake in the abstract, but they are dead in
normal use unless their enabling context exists:

- `deterministic_seed` without `use_deterministic_seed=true`
- `journey_key` without transitions enabled and a journey-capable policy
- transition policy, fallback, seed, interval, and duration without
  `transitions_enabled=true`
- `favorites_only=true` without configured favorite keys
- set-driven controls without authored preset sets or theme sets

These surfaces should be treated as conditional authoring controls, not as
always-meaningful user settings.

## Bounded Non-Claims

The current repo still carries several non-claim surfaces that later phases
should not promote casually:

- `sampling_treatment` beyond `none`
- `PLASMA_OUTPUT_FAMILY_SURFACE`
- `PLASMA_OUTPUT_MODE_DITHERED_RASTER`
- `PLASMA_FILTER_TREATMENT_KALEIDOSCOPE_MIRROR`
- `PLASMA_FILTER_TREATMENT_RESTRAINED_GLITCH`
- `PLASMA_PRESENTATION_MODE_BOUNDED_BILLBOARD_VOLUME`

They are code vocabulary, not truthful current product controls.

## U00 Conclusion

The main settings story is no longer "everything is fake."
The truthful current diagnosis is narrower:

- the core grammar settings are now real
- selection, transition, and diagnostics controls still need sharper product
  placement
- dead or unsupported vocabulary still needs hiding or removal
- authoring-grade controls should stay demoted until a later phase proves they
  deserve broader exposure
