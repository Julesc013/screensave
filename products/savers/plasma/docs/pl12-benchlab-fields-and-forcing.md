# Plasma PL12 BenchLab Fields And Forcing

## Purpose

This document freezes the implemented Plasma-specific BenchLab field catalog and forcing subset landed in `PL12`.
It describes what BenchLab can inspect and force now, where each value comes from, and how clamps or degrade outcomes are reported.

## Canonical Field Catalog

The current implemented field catalog is intentionally runtime-grounded.

| Field | Meaning | Source of truth |
| --- | --- | --- |
| `requested_lane` | Requested lane intent derived from shared renderer request and product-local presentation forcing | BenchLab request plus `plasma_benchlab_requested_lane()` |
| `resolved_lane` | Actual active Plasma lane | Compiled runtime plan |
| `degraded_from_lane` | Requested lane dropped because it could not be honored | Compiled runtime plan plus forcing state |
| `degraded_to_lane` | Actual lower lane used after degrade | Compiled runtime plan |
| `preset_key` | Active preset key | Compiled runtime plan / selection |
| `theme_key` | Active theme key | Compiled runtime plan / selection |
| `preset_set_key` | Active preset-set key or `none` | Resolved selection/config state |
| `theme_set_key` | Active theme-set key or `none` | Resolved selection/config state |
| `journey_key` | Active journey identifier or `none` | Transition plan |
| `profile_class` | Stable versus experimental posture of the active preset | Content registry |
| `quality_class` | Current product quality posture | Saver identity / manifest-backed policy |
| `preset_source` | Built-in, pack, portable, or user preset origin | Content registry |
| `preset_channel` | Stable or experimental preset channel | Content registry |
| `theme_source` | Built-in, pack, portable, or user theme origin | Content registry |
| `theme_channel` | Stable or experimental theme channel | Content registry |
| `requested_detail_level` / `detail_level` | Requested and resolved detail levels | Requested and resolved plan fields |
| `requested_generator_family` / `generator_family` | Requested and resolved field generator families | Requested and resolved plan fields |
| `requested_speed_mode` / `speed_mode` | Requested and resolved speed modes | Requested and resolved plan fields |
| `requested_resolution_mode` / `resolution_mode` | Requested and resolved resolution modes | Requested and resolved plan fields |
| `requested_smoothing_mode` / `smoothing_mode` | Requested and resolved smoothing modes | Requested and resolved plan fields |
| `requested_output_family` / `output_family` | Requested and resolved output families | Requested and resolved plan fields |
| `requested_output_mode` / `output_mode` | Requested and resolved output modes | Requested and resolved plan fields |
| `requested_sampling_treatment` / `sampling_treatment` | Requested and resolved sampling treatment slot values | Requested and resolved plan fields |
| `requested_filter_treatment` / `filter_treatment` | Requested and resolved filter or post treatment slot values | Requested and resolved plan fields |
| `requested_emulation_treatment` / `emulation_treatment` | Requested and resolved emulation treatment slot values | Requested and resolved plan fields |
| `requested_accent_treatment` / `accent_treatment` | Requested and resolved accent or overlay treatment slot values | Requested and resolved plan fields |
| `requested_presentation_mode` / `presentation_mode` | Requested and resolved presentation modes | Requested and resolved plan fields |
| `transition_requested` | Whether transitions were requested | Compiled runtime plan |
| `transition_enabled` | Whether transitions are actually active | Compiled runtime plan |
| `transition_policy` | Active transition policy | Transition plan |
| `transition_type` | Active or fallback transition type | Transition runtime |
| `transition_fallback` | Active fallback policy | Transition plan |
| `transition_seed_policy` | Seed continuity posture across transitions | Transition plan |
| `transition_source_preset` | Active transition source preset or `none` | Transition runtime |
| `transition_target_preset` | Active transition target preset or `none` | Transition runtime |
| `transition_source_theme` | Active transition source theme or `none` | Transition runtime |
| `transition_target_theme` | Active transition target theme or `none` | Transition runtime |
| `preset_morph_class` | Preset compatibility class | Content/transition metadata |
| `theme_morph_class` | Theme compatibility class | Content/transition metadata |
| `bridge_class` | Bridge compatibility class | Transition metadata |
| `seed_policy` | Normal session seed policy | Compiled runtime plan |
| `configured_seed` | Explicit configured deterministic seed value | Resolved config |
| `base_seed` | Base session seed | Session runtime state |
| `stream_seed` | Current stream seed | Session runtime state |
| `resolved_rng_seed` | Active runtime RNG seed | Session runtime state |
| `settings_surface` | Active resolved settings surface | Settings resolution |
| `content_filter` | Active stable/experimental content filter | Selection/settings resolution |
| `favorites_requested` | Whether favorites-only was requested | Resolved config and forcing |
| `favorites_applied` | Whether favorites-only actually applied | Resolved selection state |
| `forcing_active` | Whether Plasma-specific BenchLab forcing is active | Product-local BenchLab forcing state |
| `clamp_flags` | Bitfield of clamp or invalid-request outcomes | Product-local BenchLab forcing and snapshot assembly |
| `clamp_summary` | Human-readable clamp summary | Product-local BenchLab snapshot assembly |

## Field Meanings In The Current Overlay And Report

Current export surfaces:

- overlay summary: short operator-facing state snapshot
- report section: longer proof-facing product section

Both surfaces now report the shipped grammar subset as requested and resolved
truth, not just as one final resolved state.
`PX30` extends that truth surface so `profile_class`, output-family, output-mode, and treatment-slot fields now carry a bounded experimental contour, banded, and treatment subset rather than only the classic raster default.
`PX31` extends it further so `generator_family`, `output_family`, and `output_mode` now also carry a bounded experimental glyph subset and bounded field-family-I studies rather than leaving those names taxonomy only.
`PX32` extends it again so `generator_family` and `presentation_mode` now also carry bounded field-family-II and dimensional-subset truth instead of leaving those names taxonomy only.
`U02` adds the requested-versus-resolved detail, generator, speed, resolution,
smoothing, output, treatment-slot, and presentation lines needed to audit
settings authority and honest degradation.

## Forcing And Override Catalog

Implemented forcing subset:

| Forcing key | Effect | Availability rule |
| --- | --- | --- |
| `/plasma-preset:` | Overrides preset selection by key | key must resolve after canonicalization; this changes selection identity rather than reapplying the full preset bundle |
| `/plasma-theme:` | Overrides theme selection by key | key must resolve after canonicalization |
| `/plasma-preset-set:` | Overrides preset-set selection | set key must exist |
| `/plasma-theme-set:` | Overrides theme-set selection | set key must exist |
| `/plasma-journey:` | Overrides journey selection | journey key must exist |
| `/plasma-content-filter:` | Overrides stable/experimental filter posture | enum must parse and remain valid |
| `/plasma-favorites-only:` | Requests favorites-only selection | bool must parse; normal selection rules still apply |
| `/plasma-transitions:` | Enables or disables transitions | bool must parse |
| `/plasma-transition-policy:` | Requests transition policy | policy must parse |
| `/plasma-transition-fallback:` | Requests fallback policy | fallback must parse |
| `/plasma-transition-seed:` | Requests transition seed continuity policy | policy must parse |
| `/plasma-transition-interval:` | Requests transition interval | value is clamped into supported bounds |
| `/plasma-transition-duration:` | Requests transition duration | value is clamped into supported bounds |
| `/plasma-presentation:` | Requests bounded product-local presentation uplift | `auto`, `heightfield`, `curtain`, `ribbon`, `contour_extrusion`, and `bounded_surface` are supported |

Shared BenchLab controls still used:

- `/renderer:` for requested renderer kind
- `/seed:` and `/deterministic` for deterministic seed request

## Gating And Availability Rules

Key availability rules in the current implementation:

- invalid preset, theme, set, or journey keys are cleared and reported as clamps
- unsupported presentation requests are reset to `auto` and reported as clamps
- transition interval and duration overrides are bounded and clamped
- product-local forcing still flows through normal config clamping and plan validation
- a requested premium-only presentation uplift only becomes real if the resolved lane and current plan actually support it

## Resolved, Clamped, And Degraded Reporting Rules

Reporting follows these rules:

- requested values are kept visible when they matter to the proof surface,
  including the shipped grammar subset added in `U02`
- resolved values describe what Plasma actually executed
- `degraded_from` and `degraded_to` identify lane degrade outcomes
- clamp flags and clamp summary identify invalid or unsupported forcing outcomes

This keeps impossible or unsupported requests visible instead of silently disappearing.

## Deterministic Capture And Proof Direction

Current product-local proof direction:

- use deterministic seeds for textual proof runs
- use BenchLab report mode so the session exits after bounded frame exercise
- prefer separate focused captures over one oversized forcing command

`PL12` does not claim final performance or release evidence.

## Implemented Now Versus Prepared For Later

Implemented now:

- product-local field export
- bounded product-local forcing
- resolved/clamped/degraded reporting
- real overlay/report composition inside BenchLab when the active saver is `Plasma`

Prepared for later:

- broader forcing coverage
- richer inspection depth
- deeper validation automation
- final release-grade evidence layers
