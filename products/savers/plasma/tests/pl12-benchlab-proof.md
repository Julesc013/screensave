# Plasma PL12 BenchLab Proof

## Purpose

This note records the actual rebuild, smoke, and BenchLab capture work run for `PL12`.
It is limited to what was really implemented and executed for Plasma's product-local BenchLab reporting and bounded forcing surface.

## Commands Used

```powershell
$gcc='C:\Users\Jules\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\gcc.exe'
$ar='C:\Users\Jules\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\ar.exe'
$rc='C:\Users\Jules\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\mingw64\bin\windres.exe'

mingw32-make -C build/mingw/i686 PROFILE=debug CC=$gcc AR=$ar RC=$rc `
  "../../../out/mingw/i686/debug/plasma/plasma.scr"

mingw32-make -C build/mingw/i686 PROFILE=debug CC=$gcc AR=$ar RC=$rc `
  "../../../out/mingw/i686/debug/benchlab/benchlab.exe"

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

& "out/mingw/i686/debug/plasma/plasma_smoke.exe"; $LASTEXITCODE

New-Item -ItemType Directory -Force validation/captures/pl12 | Out-Null

$runs = @(
  @(
    '/deterministic',
    '/seed:743',
    '/saver:plasma',
    '/frames:4',
    '/report:validation\captures\pl12\benchlab-plasma-auto.txt'
  ),
  @(
    '/deterministic',
    '/seed:743',
    '/saver:plasma',
    '/frames:4',
    '/report:validation\captures\pl12\benchlab-plasma-forced-heightfield.txt',
    '/renderer:gl46',
    '/plasma-presentation:heightfield'
  ),
  @(
    '/deterministic',
    '/seed:743',
    '/saver:plasma',
    '/frames:4',
    '/report:validation\captures\pl12\benchlab-plasma-forced-journey.txt',
    '/plasma-preset-set:classic_core',
    '/plasma-theme-set:warm_classics',
    '/plasma-journey:classic_cycle',
    '/plasma-transitions:on',
    '/plasma-transition-policy:journey',
    '/plasma-transition-duration:1800',
    '/plasma-transition-interval:9000'
  )
)

foreach ($args in $runs) {
  $p = Start-Process `
    -FilePath 'out/mingw/i686/debug/benchlab/benchlab.exe' `
    -ArgumentList $args `
    -PassThru `
    -Wait `
    -WorkingDirectory 'd:\Projects\ScreenSave\screensave'
  $p.ExitCode
}
```

## What Was Validated For Reporting Fields

- Plasma now exports a product-local overlay summary and detailed report section when BenchLab targets the Plasma saver.
- The exported fields distinguish requested lane, resolved lane, and degraded lane outcome.
- Content identity, output/treatment/presentation state, transition state, seed state, settings surface, and clamp summary all appear in the Plasma-specific report section.

## What Was Validated For Forcing And Override Behavior

- The smoke suite validates parsing, clamping, config application, plan forcing, snapshot assembly, overlay composition, and report composition for the implemented forcing subset.
- Real BenchLab report-mode runs validated the no-forcing default case, a premium-capable heightfield presentation request, and a journey-focused transition forcing case.
- Product-local forcing remained bounded and still flowed through normal config clamp and plan validation paths.

## What Was Validated For Requested Versus Resolved Versus Degraded Reporting

- The default capture reports `auto -> compat` with no forcing and no degrade.
- The premium-heightfield capture reports a real `premium -> premium` resolved path on `gl46`.
- The journey capture reports forced content and transition intent while keeping the resolved compat baseline intact on the truthful lower-band path.

## Invariants Checked

- Product identity remains `Plasma`.
- Plasma Classic remains preserved.
- Default preset remains `plasma_lava`.
- Default theme remains `plasma_lava`.
- Alias `ember_lava -> plasma_lava` still holds.
- Routing posture remains `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`.
- With BenchLab inactive, the default classic path remains `raster` + `native_raster` + `flat`.

## Non-Fatal Observed Differences

- Real BenchLab lane selection depends on the actual requested renderer and hardware/backend availability at runtime.
- For report-mode automation, passing each switch as a separate process argument was the reliable Win32 path. Supplying one giant quoted command string from PowerShell did not exercise the parser honestly.

## Blockers

- None remained after the product-local BenchLab export and forcing path was wired and verified.

## Evidence Boundary

This note claims only what was actually run:

- `plasma.scr` rebuilt successfully
- `benchlab.exe` rebuilt successfully
- `plasma_smoke.exe` rebuilt and ran successfully with exit code `0`
- the three textual captures under `validation/captures/pl12/` were generated successfully

It does not claim final performance sign-off, full BenchLab UI depth, or release readiness.
