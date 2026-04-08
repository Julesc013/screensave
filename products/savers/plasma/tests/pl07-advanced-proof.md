# Plasma PL07 Advanced-Lane Proof

## Purpose

This note records the actual proof work run for `PL07`.
It is limited to what was really rebuilt and executed for Plasma's first bounded `gl21` advanced lane.

## Commands Used

```powershell
mingw32-make -C build/mingw/i686 PROFILE=debug CC=gcc "../../../out/mingw/i686/debug/plasma/plasma.scr"

gcc -std=c89 -Wall -Wextra -pedantic -Iplatform/include -g `
  products/savers/plasma/tests/smoke.c `
  out/intermediate/mingw/i686/debug/plasma/plasma_config.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_advanced.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_content.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_module.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_output.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_plan.o `
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

## What Was Validated For Advanced-Capable Execution

- The saver now advertises `gl21` support while keeping the same `gdi` and `gl11` routing floor and preference.
- The runtime plan can bind to a real advanced-capable execution profile when the active renderer kind is `gl21`.
- The advanced profile enables the bounded PL07 richer subset:
  - history feedback
  - domain warp
  - turbulence influence
  - blur filter
  - overlay glow accent
- An advanced-capable session allocates and runs with the extra field history and treatment scratch buffers required by that profile.
- The advanced path still remains `raster` plus `native_raster` plus `flat`; PL07 does not claim glyph, surface, or non-flat presentation support.

## What Was Validated For Degrade Paths

- A plan compiled with an advanced request and a lower-band active renderer degrades honestly back to the lower-band baseline.
- Advanced-only components are not silently enabled on `gdi` or `gl11`.
- Preset and theme identity remain preserved across that degrade path.
- The lower-band baseline validation still succeeds after the advanced subset is dropped.

## Invariants Checked

- Product identity remains `Plasma`.
- Plasma Classic remains preserved.
- Default preset remains `plasma_lava`.
- Default theme remains `plasma_lava`.
- Alias `ember_lava -> plasma_lava` still holds.
- Routing posture remains `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`.
- Lower-band validation still remains intact for the preserved default path.

## Non-Fatal Differences And Evidence Boundary

The advanced execution checks in the smoke test use a deterministic fake renderer-info harness to simulate requested and active renderer kinds for unit-style plan and session validation.

That means this note does **not** claim:

- a live interactive GL21 window/context run
- capture-based visual comparison between lower-band and advanced-lane backends
- any modern-lane or premium-lane proof

It **does** claim the actual code above was rebuilt and the smoke executable ran successfully with exit code `0`.

## Remaining Blockers

- None blocked the bounded PL07 advanced-lane work that was actually implemented and run here.
