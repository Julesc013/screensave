# SY10 Environment Safety Contract

This document freezes the shared support-facing vocabulary for environment safety and recovery.

It standardizes what later product notes mean when they talk about safe preview behavior, long-run posture, and support-visible fallback or recovery.
It does not add new runtime safety systems.

## Purpose

Use this contract when describing:

- environment-sensitive safety terms
- config and recovery wording
- support-facing clamp or fallback expectations
- which safety terms are current shared claims versus reserved future vocabulary

## Shared Safety And Recovery Vocabulary

Use these meanings:

| Term | SY10 Meaning | Current Shared Status |
| --- | --- | --- |
| `preview_safe` | remains usable and bounded in the embedded preview path without requiring fullscreen-only composition or excessive startup cost | current shared claim |
| `long_run_safe` | remains operationally stable and visually bounded during unattended runs | current shared support meaning |
| `long_run_stable` | current shipped manifest wording that expresses the present shared long-run capability claim | current shared manifest term |
| `dark_room_safe` | restrained enough for dark-room use rather than assuming aggressive brightness or flash | descriptive term only |
| `oled_safe` | avoids patterns or brightness behavior likely to be unsuitable for OLED-style use | reserved vocabulary only |
| `battery_saving` | reduces cost or activity for power-sensitive environments | reserved vocabulary only |
| `thermal_governed` | intentionally reduces load in response to heat-sensitive posture | reserved vocabulary only |
| `remote_desktop_safe` | behaves coherently under remote-session constraints | reserved vocabulary only |
| `safe_mode_fallback` | falls back to a bounded recoverable mode when a richer path cannot be supported safely | reserved vocabulary only |

Shared-status rule:

- `current shared claim` means the repo already uses the term or an equivalent capability claim across products
- `descriptive term only` means the repo may describe it, but it is not a shared runtime guarantee
- `reserved vocabulary only` means the term is frozen for future honest use but is not yet a current shared support promise

## Recovery Semantics

Shared support-facing recovery expectations are:

- if stored saver settings cannot be loaded, the product should recover to editable defaults rather than pretending saved state was applied
- if saver settings or renderer preference cannot be saved, support-facing wording should say so explicitly
- invalid preset, theme, or pack imports should fail safely without partial apply
- when a richer request cannot be honored, the resolved and degraded outcome should stay explicit in diagnostics or proof surfaces

These rules freeze wording and support posture.
They do not add new shared runtime code.

## Clamping And Fallback Expectations

At the shared-policy level:

- invalid or unsupported requests should clamp or fall back to a truthful supported outcome
- support surfaces should preserve requested versus resolved versus degraded truth when that distinction matters
- fallback should not be described as a supported richer path when it actually landed on a lower or safer path
- the internal `null` path remains an emergency safety fallback, not a normal user-facing safety mode

## What Counts As Shared Now

Shared now:

- `preview_safe`
- the current `long_run_stable` manifest capability wording
- the support-facing meaning that later product docs may describe as `long_run_safe`
- explicit recovery wording for load, save, and import failure
- truthful clamp and fallback wording

Not shared now:

- exact product clamps
- exact product fallback presets or themes
- exact visual restrictions for one product under dark-room or long-run posture
- any runtime implementation behind the reserved safety terms

## What Remains Product-Local

The following stay product-local for now:

- exact Plasma fallback presets or default-safe content choices
- exact treatment, transition, or presentation restrictions under safety-sensitive modes
- dark-room tuning for a specific saver
- any future OLED, thermal, battery, or remote-session behavior unless at least one more product or a host contract truly needs it

`SY10` freezes the language.
Later product work still owns the actual behavior unless the promotion rule is satisfied.
