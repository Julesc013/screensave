# Plasma PL10 Transition-Engine Proof

## Purpose

This note records the actual proof work run for `PL10`.
It is limited to what was really rebuilt and executed for Plasma's first bounded transition engine, compatibility rules, and journey progression support.

## Commands Used

```powershell
mingw32-make -C build/mingw/i686 PROFILE=debug CC=gcc "../../../out/mingw/i686/debug/plasma/plasma.scr"

gcc -std=c89 -Wall -Wextra -pedantic -Iplatform/include -g `
  products/savers/plasma/tests/smoke.c `
  out/intermediate/mingw/i686/debug/plasma/plasma_config.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_advanced.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_content.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_modern.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_module.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_output.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_plan.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_premium.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_presentation.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_presets.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_render.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_selection.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_sim.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_themes.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_transition.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_treatment.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_validate.o `
  out/intermediate/mingw/i686/debug/screensave_platform/libscreensave_platform.a `
  -ladvapi32 -lgdi32 -luser32 -lopengl32 `
  -o out/mingw/i686/debug/plasma/plasma_smoke.exe

& "out/mingw/i686/debug/plasma/plasma_smoke.exe"; $LASTEXITCODE
```

## What Was Validated For Theme Morphs

- Product-local transition preferences now import, clamp, export, and persist honestly.
- A lower-band `theme_set` transition plan compiles with explicit transition policy, timing, and supported-type flags.
- A real theme morph starts on the preserved lower-band path when a compatible theme-set pair is selected.
- Mid-transition palette resolution produces intermediate primary and accent colors rather than only hard-cutting between source and target themes.

## What Was Validated For Preset Morphs

- A lower-band `preset_set` transition plan compiles for a directly compatible preset-family pair.
- A real direct preset morph starts for compatible preset pairs rather than falling back immediately.
- Mid-transition speed and smoothing resolve to interpolated values, proving the transition path carries real preset-state morphing rather than only a terminal swap.

## What Was Validated For Bridge Morphs And Fallbacks

- A bounded bridge morph starts on `gl21` for a curated cross-family pair that shares a bridge class.
- The bridge morph commits the target preset at the bounded midpoint and completes cleanly.
- The same unsupported pair on the lower bands uses explicit fallback policy rather than silently pretending the bridge morph exists:
  - `theme_morph` fallback when a compatible companion theme-set path is available
  - explicit `reject` when the requested pair is unsupported and the fallback policy says to reject

## What Was Validated For Journeys

- Journey preferences compile into the runtime plan with a resolved journey entry.
- Journey execution waits for the curated step dwell instead of using the plain global interval.
- A journey step starts a real transition using its step-level preset/theme-set policy.
- Journey completion advances the ordered step index for the implemented subset.

## Invariants Checked

- Product identity remains `Plasma`.
- Plasma Classic remains preserved.
- Default preset remains `plasma_lava`.
- Default theme remains `plasma_lava`.
- Alias `ember_lava -> plasma_lava` still holds.
- Routing posture remains `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`.
- Non-transition lower-band behavior remains materially unchanged when transitions are disabled.
- Higher lanes keep their previously established advanced, modern, and premium gating while transitions remain optional.

## Non-Fatal Differences And Evidence Boundary

The transition execution checks in the smoke test use the same deterministic fake-renderer-info harness already used for lane validation. That means this note does **not** claim:

- a live interactive multi-minute visual capture session
- public settings-surface coverage for transition controls
- BenchLab inspection tooling
- a universal transition pair matrix across every preset/theme combination
- any PL11+ settings, BenchLab, or release-cut work

It **does** claim the actual code above was rebuilt and the smoke executable ran successfully with exit code `0`.

## Remaining Blockers

- None blocked the bounded PL10 transition-engine work that was actually implemented and run here.
