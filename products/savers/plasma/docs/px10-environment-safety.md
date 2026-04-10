# PX10 Environment Safety

## Purpose

This document ties Plasma's current safety and recovery language back to actual product behavior.
It uses the shared `SY10` vocabulary, but it only claims what Plasma currently implements or currently supports honestly.

## Current Supported Terms

| Term | Current Plasma Meaning | Current Status | Evidence |
| --- | --- | --- | --- |
| `preview_safe` | preview mode reduces field density and motion intensity, keeps the product operational in the small child-window path, and avoids depending on fullscreen-only presentation | current product claim | saver manifest, current runtime preview logic, and shared host preview policy |
| `long_run_safe` | the product remains bounded for unattended use on the stable default path and keeps long-run posture restrained rather than drifting into a lab-only identity | current support meaning | saver manifest `long_run_stable=1`, PL13 soak coverage, and current product identity |
| `long_run_stable` | current shipped manifest wording for the long-run claim | current product claim | [`../manifest.ini`](../manifest.ini) |
| `dark_room_safe` | curated restrained presets, themes, and sets exist for quieter rooms, but the term is still descriptive rather than parser-enforced metadata | descriptive product term | built-in classic content and sets |
| `oled_safe` | no product-modeled OLED policy exists | unsupported / unmodeled | no current runtime or metadata support |
| `battery_saving` | no product-modeled battery policy exists | unsupported / unmodeled | no current runtime or metadata support |
| `thermal_governed` | no product-modeled thermal policy exists | unsupported / unmodeled | no current runtime or metadata support |
| `remote_desktop_safe` | no product-modeled remote-session policy exists | unsupported / unmodeled | no current runtime or metadata support |
| `safe_mode_fallback` | no named user-facing safe mode exists beyond current truthful fallback and clamp behavior | unsupported / unmodeled | current product does not expose a dedicated safe mode |

## Actual Current Behaviors

### Preview-Safe Behavior

Current runtime code already makes preview mode calmer and smaller in scope than the normal fullscreen or windowed path:

- field density is reduced in preview mode
- effective speed is reduced in preview mode
- smoothing blend is capped in preview mode
- the product still uses the same truthful lower-band routing rules

This is a real current behavior.
It is not a marketing label only.

### Long-Run-Safe Behavior

Current repo-grounded long-run posture means:

- the stable default path remains the strongest exercised runtime surface
- long-run claims are read together with the existing repeated lower-band soak coverage
- calmer long-run content exists through the restrained dark-room and phosphor-style presets and sets

It does not mean:

- a hard runtime SLA
- an hours-long richer-lane proof matrix
- a product-modeled thermal or burn-in policy

### Dark-Room-Safe Descriptive Surface

Current built-in restrained content includes:

- `quiet_darkroom`
- `museum_phosphor`
- `midnight_interference`
- `amber_terminal`
- the `dark_room_classics` preset and theme sets

These are real curated product choices.
They are not yet first-class serialized safety tags with enforced selection rules.

## Actual Clamp, Fallback, And Recovery Behavior

Current product-local recovery behavior includes:

- config clamp back to known preset, theme, and selection values when imports contain unknown keys
- truthful fallback from unsupported richer-lane or presentation requests back to supported runtime outcomes
- BenchLab forcing clamps for invalid or unsupported diagnostic values
- explicit preservation of requested versus resolved versus degraded truth in the diagnostic surface

These are real current behaviors and are already exercised by the current product-local smoke surface.

## Unsupported Or Not-Yet-Modeled Safety Behavior

Plasma does not currently model or claim:

- OLED protection policy
- battery-sensitive policy
- thermal-governed scaling
- remote-desktop-safe policy
- a named dedicated safe-mode fallback profile
- automatic dark-room content switching

Those terms must remain explicit non-claims until later work implements and proves them.

## Remaining Support Caveats

- Preview-safe behavior is real, but a fresh native preview-host rerun was not added in this tranche.
- Long-run-safe behavior is still bounded by the existing soak surface and does not become a numeric SLA here.
- Dark-room-safe remains a descriptive product-local content statement, not parser-enforced safety metadata.
- Richer reserved safety terms remain unmodeled.

## Scope Boundary

`PX10` makes current Plasma safety and recovery posture explicit.
It does not add new host policies or a new safety runtime framework.
