# Plasma PL08 Modern-Lane Proof

## Purpose

This note records the actual proof work run for `PL08`.
It is limited to what was really rebuilt and executed for Plasma's first bounded `gl33` modern lane.

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

## What Was Validated For Modern-Capable Execution

- The saver now advertises `gl33` support while keeping the same `gdi` and `gl11` routing floor and preference.
- The runtime plan can bind to a real modern-capable execution profile when the active renderer kind is `gl33`.
- The modern profile stays layered on the advanced-derived path instead of bypassing it.
- The bounded PL08 refined subset is active on `gl33`:
  - advanced-derived field history, domain warp, turbulence, blur, and glow remain active
  - a modern-only refined field pass runs on top of that base
  - a modern-only refined filter pass runs as an extra post stage
  - a dedicated modern presentation buffer is used before the renderer blit
- The modern path still remains `raster` plus `native_raster` plus `flat`; PL08 does not claim premium dimensional presentation or premium-only output families.

## What Was Validated For Degrade Paths

- A plan compiled with a modern request and an active `gl21` renderer degrades honestly to the advanced lane while preserving preset and theme identity.
- A plan compiled with a modern request and a lower-band active renderer degrades honestly to the lower-band baseline.
- Modern-only components are not silently enabled on `gdi`, `gl11`, or the plain `gl21` advanced path.
- The lower-band baseline validation and the PL07 advanced validation both remain intact after the modern seam is added.

## Invariants Checked

- Product identity remains `Plasma`.
- Plasma Classic remains preserved.
- Default preset remains `plasma_lava`.
- Default theme remains `plasma_lava`.
- Alias `ember_lava -> plasma_lava` still holds.
- Routing posture remains `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`.
- Lower-band and advanced validation remain intact for their preserved paths.

## Non-Fatal Differences And Evidence Boundary

The modern execution checks in the smoke test use a deterministic fake renderer-info harness to simulate requested and active renderer kinds for unit-style plan and session validation.

That means this note does **not** claim:

- a live interactive GL33 window/context run
- capture-based visual comparison between lower-band, advanced, and modern-lane backends
- any GL46 premium-lane behavior

It **does** claim the actual code above was rebuilt and the smoke executable ran successfully with exit code `0`.

## Remaining Blockers

- None blocked the bounded PL08 modern-lane work that was actually implemented and run here.
