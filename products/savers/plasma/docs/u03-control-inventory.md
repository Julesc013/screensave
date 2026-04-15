# U03 Control Inventory

## Purpose

This inventory records every meaningful Plasma settings control after the `U03`
surface recut and names its disposition explicitly.

## Active Surface Controls

| Control | Setting / Role | Surface | Disposition | Rationale |
| --- | --- | --- | --- | --- |
| Surface selector | `settings_surface` | Global | Keep | The user needs an explicit way to switch among the three bounded surfaces. |
| Safe Defaults | reset action | Global | Keep | Resets to the safe `plasma_lava` default posture without widening stable scope. |
| Preset | `preset_key` | Basic | Keep | First-pass identity control. Preset selection intentionally reseeds the curated bundle. |
| Theme | `theme_key` | Basic | Keep | First-pass palette identity control. |
| Speed | `speed_mode` | Basic | Keep | Meaningful stable motion control. |
| Visual Intensity | `detail_level` | Basic | Keep | Safe high-level density and activity control. |
| Content Pool | `content_filter` | Basic | Keep | Honest stable-versus-experimental pool control; disabled when no experimental content exists. |
| Allow Transitions | `transitions_enabled` | Basic | Keep | Real coarse transition gate; later transition controls depend on it. |
| Generator | `effect_mode` | Advanced | Keep | Real runtime grammar field. |
| Output Family | `output_family` | Advanced | Keep | Real runtime grammar field; broader families are now marked experimental in the UI. |
| Output Mode | `output_mode` | Advanced | Keep | Real runtime grammar field; disabled when the chosen family only has one supported mode. |
| Resolution | `resolution_mode` | Advanced | Keep | Real runtime density control. |
| Smoothing | `smoothing_mode` | Advanced | Keep | Real treatment-facing control. |
| Filter | `filter_treatment` | Advanced | Keep | Real treatment slot. |
| Emulation | `emulation_treatment` | Advanced | Keep | Real treatment slot. |
| Accent | `accent_treatment` | Advanced | Keep | Real treatment slot. |
| Presentation | `presentation_mode` | Advanced | Keep | Real presentation control; premium paths are labeled honestly. |
| Deterministic Mode | `use_deterministic_seed` | Advanced | Keep | Real execution-state control; fixed seed stays in Author/Lab. |
| Preset Set | `preset_set_key` | Author/Lab | Keep | Real curated selection surface. |
| Theme Set | `theme_set_key` | Author/Lab | Keep | Real curated selection surface. |
| Favorites Only | `favorites_only` | Author/Lab | Keep | Real bounded filter; disabled until favorites exist. |
| Transition Policy | `transition_policy` | Author/Lab | Keep | Real transition control; disabled until transitions are enabled. |
| Journey | `journey_key` | Author/Lab | Keep | Real authored journey selection; disabled unless journeys exist and policy allows it. |
| Fallback Policy | `transition_fallback_policy` | Author/Lab | Keep | Real bounded transition behavior. |
| Seed Continuity | `transition_seed_policy` | Author/Lab | Keep | Real transition execution behavior. |
| Interval | `transition_interval_millis` | Author/Lab | Keep | Real authored timing control. |
| Duration | `transition_duration_millis` | Author/Lab | Keep | Real authored timing control. |
| Fixed Seed | `deterministic_seed` | Author/Lab | Keep | Real seed-value control; disabled unless deterministic mode is on. |
| Show Diagnostics | `diagnostics_overlay_enabled` | Author/Lab | Keep | Real authoring and proof surface. |

## Hidden, Disabled, Or Deferred Controls

| Control / Surface | Disposition | Rationale |
| --- | --- | --- |
| `sampling_treatment` | Hide | Still clamps to `none`; not honest enough to surface. |
| Favorite preset/theme key-list editors | Hide | The underlying list state is real, but the dialog does not yet have a bounded honest editor. |
| Exclusion key-list editors | Hide | Same reason as favorites lists. |
| Renderer-lane forcing | Hide | Product-local dialog should not impersonate BenchLab or future lab tooling. |
| BenchLab forcing | Hide | Support and validation surface, not normal config UI. |
| Old cumulative Basic-in-Advanced and Advanced-in-Author behavior | Remove | It was misleading surface inflation, not a real product layer. |
| Hidden-control reread on routine refresh | Remove | It let invisible controls silently affect the working state. |

## Nontrivial Decisions

- Preset selection stays in `Basic`, but only preset selection itself reseeds
  the preset bundle.
  Normal refreshes do not rebuild the working config from defaults plus preset.
- `Output Mode` stays visible in `Advanced`, but it is disabled whenever the
  chosen output family only has one supported mode.
  That is more honest than pretending a nonexistent second choice exists.
- `Fixed Seed` stays in `Author/Lab` even though deterministic mode moved to
  `Advanced`.
  The mode is grammar-facing; the explicit numeric seed is authoring-facing.

## Deferred To Later Phases

Later phases still need to decide whether to keep, demote, merge, or remove:

- broader non-raster grammar that still proves visually weak
- weak transition subsets that survive only as authoring surfaces
- any hidden list-editor or authoring UI beyond the bounded `U03` shell
