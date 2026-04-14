# Plasma U00 Settings Influence Matrix

## Scope

This matrix records whether each current shipped setting materially affects:

- selection state
- resolved runtime plan
- rendered output
- reporting only

It describes the repo state before the salvage rewrite.

## Current Matrix

| Surface Control | Selection | Plan | Output | Notes |
| --- | --- | --- | --- | --- |
| `preset_key` | Yes | Yes | Yes | Also silently selects output family, output mode, treatments, and presentation through `plasma_find_preset_values()` |
| `theme_key` | Yes | Yes | Yes | Palette and accent only |
| `speed_mode` | No | Yes | Yes | Real motion change |
| `detail_level` | No | Yes | Yes | Field size and long-run density change |
| `effect_mode` | No | Yes | Yes | Real generator change |
| `resolution_mode` | No | Yes | Yes | Real field-size change |
| `smoothing_mode` | No | Yes | Yes | Real field blur amount change |
| `content_filter` | Yes | Sometimes | Indirect | Affects selection pool, not the currently selected preset if it already remains valid |
| `favorites_only` | Yes | Sometimes | Indirect | Same caveat as content filter |
| `preset_set_key` | Yes | Yes | Indirect | Matters only for set-driven selection and transitions |
| `theme_set_key` | Yes | Yes | Indirect | Matters only for set-driven selection and transitions |
| `transitions_enabled` | No | Yes | Yes | Long-run behavior only |
| `transition_policy` | No | Yes | Yes | Long-run behavior only |
| `journey_key` | No | Yes | Yes | Long-run behavior only |
| `transition_fallback_policy` | No | Yes | Yes | Long-run unsupported-pair behavior only |
| `transition_seed_policy` | No | Yes | Yes | Transition continuity only |
| `transition_interval_millis` | No | Yes | Yes | Long-run cadence only |
| `transition_duration_millis` | No | Yes | Yes | Transition timing only |
| `use_deterministic_seed` | No | Yes | Yes | Real reproducibility change |
| `deterministic_seed` | No | Yes | Yes | Real reproducibility change when deterministic mode is enabled |
| `diagnostics_overlay_enabled` | No | No | No | Reporting/UI only |
| `settings_surface` | No | No | No | UI only |

## Hidden Visual-Grammar Authority

The following resolved plan fields are currently not settings-owned:

- `output_family`
- `output_mode`
- `sampling_treatment`
- `filter_treatment`
- `emulation_treatment`
- `accent_treatment`
- `presentation_mode`

They come from the preset-value table after settings resolution.

## Dead Or Near-Dead Surfaces In Current State

- `sampling_treatment` is enumerated in the runtime but only `none` validates in normal plan compilation
- `PLASMA_OUTPUT_MODE_DITHERED_RASTER` exists in the enum but is not admitted by the output validator
- `PLASMA_OUTPUT_FAMILY_SURFACE` is enumerated but not supported
- `PLASMA_FILTER_TREATMENT_KALEIDOSCOPE_MIRROR` and `PLASMA_FILTER_TREATMENT_RESTRAINED_GLITCH` are enumerated but not implemented
- `PLASMA_PRESENTATION_MODE_BOUNDED_BILLBOARD_VOLUME` is enumerated but unsupported

## Correction Goal

After the salvage rewrite, every user-facing control must land in one of these buckets only:

- real selection control
- real runtime-plan control
- real rendered-output control
- explicit reporting-only control

Nothing user-facing should remain a hidden preset-only master switch.
