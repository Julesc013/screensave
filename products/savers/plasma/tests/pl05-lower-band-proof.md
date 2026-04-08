# Plasma PL05 Lower-Band Proof

## Purpose

This note records the actual lower-band proof work run for `PL05`.
It is limited to what was really rebuilt and executed in-repo for the preserved Plasma Classic baseline.

## Commands Used

```powershell
mingw32-make -C build/mingw/i686 PROFILE=debug CC=gcc "../../../out/mingw/i686/debug/plasma/plasma.scr"

gcc -std=c89 -Wall -Wextra -pedantic -Iplatform/include -g `
  products/savers/plasma/tests/smoke.c `
  out/intermediate/mingw/i686/debug/plasma/plasma_config.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_module.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_presets.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_render.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_sim.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_themes.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_output.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_plan.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_presentation.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_treatment.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_validate.o `
  out/intermediate/mingw/i686/debug/screensave_platform/libscreensave_platform.a `
  -ladvapi32 -lgdi32 -luser32 -lopengl32 `
  -o out/mingw/i686/debug/plasma/plasma_smoke.exe

& "out/mingw/i686/debug/plasma/plasma_smoke.exe"; $LASTEXITCODE
```

## What Was Validated On `gdi`

- The preserved classic/default plan validates for `SCREENSAVE_RENDERER_KIND_GDI`.
- The classic/default path remains `raster` plus `native_raster` plus `flat` with all treatment slots at explicit `none`.
- Every preserved classic preset compiles into a plan that validates for `gdi`.
- Session creation without a live renderer still resolves to the honest `gdi` floor and runs the classic baseline allocation path.
- The explicit output, treatment, and presentation stages all execute successfully for the classic path after a session step.

## What Was Validated On `gl11`

- The preserved classic/default plan validates for `SCREENSAVE_RENDERER_KIND_GL11`.
- Every preserved classic preset compiles into a plan that validates for `gl11`.
- Unsupported glyph output, unsupported non-default filter treatment, and unsupported non-flat presentation mutations are rejected for the lower-band validation path, including `gl11`.
- The compat lane remains the same product identity as the universal baseline in plan terms; no richer `gl21+`-style behavior is claimed.

## Invariants Checked

- Product identity remains `Plasma`.
- Default preset remains `plasma_lava`.
- Default theme remains `plasma_lava`.
- Classic preset keys and theme keys remain resolvable.
- `ember_lava -> plasma_lava` remains a valid alias.
- Manifest and module routing posture remain `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`.
- Lower-band validation rejects `gl21` as part of the PL05 baseline proof.

## Non-Fatal Differences Observed

- None were observed in the plan/session proof that was actually run.
- No live windowed renderer capture comparison between `gdi` and `gl11` was run in this note, so no claim is made here about pixel-identical backend captures.

## Remaining Blockers

- None blocked the PL05 lower-band proof work that was actually run.

## Evidence Boundary

Actually run:

- rebuilt `plasma.scr`
- rebuilt `plasma_smoke.exe`
- executed `plasma_smoke.exe` successfully with exit code `0`

Documented but not claimed as run here:

- live interactive validation with an actual `gl11` window/context
- capture-based visual comparison between `gdi` and `gl11`
