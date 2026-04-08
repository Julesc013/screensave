# Plasma PL09 Premium-Lane Proof

## Purpose

This note records the actual proof work run for `PL09`.
It is limited to what was really rebuilt and executed for Plasma's first bounded `gl46` premium lane and its first real dimensional presentation path.

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
  out/intermediate/mingw/i686/debug/plasma/plasma_treatment.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_validate.o `
  out/intermediate/mingw/i686/debug/screensave_platform/libscreensave_platform.a `
  -ladvapi32 -lgdi32 -luser32 -lopengl32 `
  -o out/mingw/i686/debug/plasma/plasma_smoke.exe

& "out/mingw/i686/debug/plasma/plasma_smoke.exe"; $LASTEXITCODE
```

## What Was Validated For Premium-Capable Execution

- The saver now advertises `gl46` support while keeping the same `gdi` and `gl11` routing floor and preference.
- The runtime plan can bind to a real premium-capable execution profile when the active renderer kind is `gl46`.
- The premium profile remains layered on top of the advanced-derived and modern-derived base rather than bypassing them.
- The bounded PL09 premium subset is active on `gl46`:
  - extended field refinement on top of the modern field state
  - a premium-only post chain after the existing blur and overlay stages
  - dedicated premium treatment and presentation buffers
  - `heightfield` presentation mode as the first real bounded dimensional presentation path

## What Was Validated For Dimensional Presentation Execution

- The runtime plan selects `heightfield` only for the premium-capable `gl46` path.
- A premium-capable session allocates the premium presentation buffer and uses it as the actual renderer-facing bitmap source.
- The dimensional path stays bitmap-presented, field-derived, and bounded.
- Unsupported lower-lane requests for `heightfield` remain rejected rather than silently faked.

## What Was Validated For Degrade Paths

- A plan compiled with a premium request and an active `gl33` renderer degrades honestly to the modern lane while preserving preset and theme identity.
- A plan compiled with a premium request and an active `gl21` renderer degrades honestly to the advanced lane.
- A plan compiled with a premium request and a lower-band active renderer degrades honestly to the lower-band baseline.
- Premium-only components are not silently enabled on `gdi`, `gl11`, `gl21`, or plain `gl33`.

## Invariants Checked

- Product identity remains `Plasma`.
- Plasma Classic remains preserved.
- Default preset remains `plasma_lava`.
- Default theme remains `plasma_lava`.
- Alias `ember_lava -> plasma_lava` still holds.
- Routing posture remains `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`.
- Lower-band, advanced, and modern validation remain intact for their preserved paths.

## Non-Fatal Differences And Evidence Boundary

The premium execution checks in the smoke test use a deterministic fake renderer-info harness to simulate requested and active renderer kinds for unit-style plan and session validation.

That means this note does **not** claim:

- a live interactive GL46 window/context run
- capture-based visual comparison between lower-band, advanced, modern, and premium backends
- free-camera or scenic 3D behavior
- any transition, journey, settings-surface, or BenchLab work

It **does** claim the actual code above was rebuilt and the smoke executable ran successfully with exit code `0`.

## Remaining Blockers

- None blocked the bounded PL09 premium-lane work that was actually implemented and run here.
