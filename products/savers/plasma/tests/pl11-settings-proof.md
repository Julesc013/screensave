# Plasma PL11 Settings-Surface Proof

## Purpose

This note records the actual rebuild and smoke work run for `PL11`.
It is limited to what was really implemented and executed for Plasma's layered settings surfaces, catalog, availability logic, and runtime settings resolution.

## Commands Used

```powershell
$gcc='C:\Users\Jules\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\gcc.exe'
$ar='C:\Users\Jules\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\ar.exe'
$rc='C:\Users\Jules\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\windres.exe'

mingw32-make -C build/mingw/i686 PROFILE=debug CC=$gcc AR=$ar RC=$rc `
  "../../../out/mingw/i686/debug/plasma/plasma.scr"

& $gcc -std=c89 -Wall -Wextra -pedantic -Iplatform/include -g `
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
  out/intermediate/mingw/i686/debug/plasma/plasma_settings.o `
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

## What Was Validated For The Basic Surface

- Default settings still resolve to the preserved `plasma_lava` preset and theme.
- The default settings surface remains `basic`.
- Default settings resolution still compiles to the preserved classic/default runtime path.

## What Was Validated For The Advanced Surface

- The settings catalog and descriptor table now exist and validate internally.
- Advanced-surface settings such as render detail, effect, resolution, smoothing, preset sets, theme sets, transition enablement, transition policy, and deterministic seed mode are all represented in the catalog.
- Runtime settings resolution now feeds the compiled plan rather than relying on scattered raw config reads.

## What Was Validated For The Author/Lab Surface

- Author/Lab-only descriptors for content filtering, favorites-only selection, journeys, transition fallback policy, transition seed continuity, transition timing, deterministic seed value, and diagnostics overlay are present in the catalog.
- Journey selection becomes available only when transitions are enabled and a journey-capable policy is active.
- Deterministic seed value becomes available only when deterministic seed mode is enabled.

## What Was Validated For Persistence, Resolution, And Clamping

- The saver rebuild includes the new `plasma_settings` module and the updated layered config dialog resources.
- The runtime plan compiles from resolved settings values after config defaults, content defaults, persisted state, session selections, and bounded gating.
- Catalog availability rules correctly disable unsupported settings for the current shipped product state:
  - experimental content filtering stays unavailable because the current registry is stable-only
  - favorites-only stays unavailable until favorite keys are actually configured

## Invariants Checked

- Product identity remains `Plasma`.
- Plasma Classic remains preserved.
- Default preset remains `plasma_lava`.
- Default theme remains `plasma_lava`.
- Alias `ember_lava -> plasma_lava` still holds.
- Routing posture remains `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`.
- The lower-band baseline and existing richer-lane behavior remain intact.
- The default classic runtime path remains `raster` + `native_raster` + `flat`.

## Non-Fatal Differences And Evidence Boundary

This proof note does **not** claim:

- full BenchLab settings inspection or forcing
- live UI automation coverage of every dialog interaction
- lane forcing controls
- full favorite/exclusion list editing
- PL12+ BenchLab or release-cut work

It **does** claim that the commands above were actually run, that Plasma rebuilt successfully with the new settings module and layered dialog resources, and that `plasma_smoke.exe` completed with exit code `0`.

## Remaining Blockers

- None blocked the bounded PL11 settings-surface work that was actually implemented and run here.
