# PL13 Soak And Proof

## Purpose

This note records the exact PL13 proof and soak commands that were actually run for Plasma.

## Commands Actually Run

Toolchain-pinned saver and BenchLab rebuilds:

```powershell
$gcc='C:\Users\Jules\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\gcc.exe'
$ar='C:\Users\Jules\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\ar.exe'
$rc='C:\Users\Jules\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\windres.exe'

mingw32-make -C build/mingw/i686 PROFILE=debug CC=$gcc AR=$ar RC=$rc "../../../out/mingw/i686/debug/plasma/plasma.scr"
mingw32-make -C build/mingw/i686 PROFILE=debug CC=$gcc AR=$ar RC=$rc "../../../out/mingw/i686/debug/benchlab/benchlab.exe"
```

Smoke rebuild and execution:

```powershell
& $gcc -std=c89 -Wall -Wextra -pedantic -Iplatform/include -g `
  products/savers/plasma/tests/smoke.c `
  out/intermediate/mingw/i686/debug/plasma/plasma_advanced.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_benchlab.o `
  out/intermediate/mingw/i686/debug/plasma/plasma_config.o `
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

& "out/mingw/i686/debug/plasma/plasma_smoke.exe"
```

BenchLab capture runs:

```powershell
$benchlab = 'out/mingw/i686/debug/benchlab/benchlab.exe'

Start-Process -FilePath $benchlab -ArgumentList @('/deterministic','/seed:743','/saver:plasma','/frames:4','/report:validation\captures\pl13\benchlab-plasma-gdi.txt','/renderer:gdi') -WorkingDirectory 'd:\Projects\ScreenSave\screensave' -Wait -PassThru
Start-Process -FilePath $benchlab -ArgumentList @('/deterministic','/seed:743','/saver:plasma','/frames:4','/report:validation\captures\pl13\benchlab-plasma-gl11.txt','/renderer:gl11') -WorkingDirectory 'd:\Projects\ScreenSave\screensave' -Wait -PassThru
Start-Process -FilePath $benchlab -ArgumentList @('/deterministic','/seed:743','/saver:plasma','/frames:4','/report:validation\captures\pl13\benchlab-plasma-gl21.txt','/renderer:gl21') -WorkingDirectory 'd:\Projects\ScreenSave\screensave' -Wait -PassThru
Start-Process -FilePath $benchlab -ArgumentList @('/deterministic','/seed:743','/saver:plasma','/frames:4','/report:validation\captures\pl13\benchlab-plasma-gl33.txt','/renderer:gl33') -WorkingDirectory 'd:\Projects\ScreenSave\screensave' -Wait -PassThru
Start-Process -FilePath $benchlab -ArgumentList @('/deterministic','/seed:743','/saver:plasma','/frames:4','/report:validation\captures\pl13\benchlab-plasma-gl46-heightfield.txt','/renderer:gl46','/plasma-presentation:heightfield') -WorkingDirectory 'd:\Projects\ScreenSave\screensave' -Wait -PassThru
Start-Process -FilePath $benchlab -ArgumentList @('/deterministic','/seed:743','/saver:plasma','/frames:4','/report:validation\captures\pl13\benchlab-plasma-degrade-heightfield-auto.txt','/plasma-presentation:heightfield') -WorkingDirectory 'd:\Projects\ScreenSave\screensave' -Wait -PassThru
Start-Process -FilePath $benchlab -ArgumentList @('/deterministic','/seed:743','/saver:plasma','/frames:320','/report:validation\captures\pl13\benchlab-plasma-journey-gl11.txt','/renderer:gl11','/plasma-preset-set:classic_core','/plasma-theme-set:warm_classics','/plasma-journey:classic_cycle','/plasma-transitions:on','/plasma-transition-policy:journey','/plasma-transition-duration:1800','/plasma-transition-interval:9000') -WorkingDirectory 'd:\Projects\ScreenSave\screensave' -Wait -PassThru
```

## What Was Validated

- proof-matrix, performance-envelope, and known-limit catalog consistency
- forced `gdi`, `gl11`, `gl21`, `gl33`, and `gl46` report-mode runs
- requested-versus-resolved-versus-degraded proof for premium-only presentation requested on auto policy
- repeated lower-band session creation, stepping, render, and destroy behavior
- repeated journey-driven transition stepping for the implemented subset on `gl21`

## Invariants Checked

- product name remains `Plasma`
- `Plasma Classic` remains preserved
- default preset and theme remain `plasma_lava`
- `ember_lava -> plasma_lava` still resolves
- manifest routing posture remains `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`
- default classic path remains `raster` + `native_raster` + `flat`

## Non-Fatal Observed Differences

- BenchLab capture proof is strongest for requested-versus-resolved runtime truth, not for final ship-host behavior.
- Transition soak uses the implemented journey dwell values and bounded follow-up ticks, so it proves the current curated transition subset rather than all theoretical transition pairs.

## Blockers

- No multi-driver or lower-capability hardware sweep was available in PL13.
- No hard numeric frame-time or memory envelope was gathered in PL13.

## Evidence Boundary

- Actual evidence: successful rebuilds, `plasma_smoke.exe` exit code `0`, and the checked-in `validation/captures/pl13/*` BenchLab reports.
- Documentation only: broader future performance targets, broader hardware coverage, and any transition pair not exercised by the current smoke or capture set.
