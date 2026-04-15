# Plasma U04 Visual Grammar Proof

## Purpose

This note records the actual evidence used for the `U04` visual-grammar
checkpoint.

It distinguishes fresh execution evidence from older carried-forward
documentation.

## Commands Actually Run

MSVC XP-compatible rebuild:

```powershell
$devCmd = 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat'
$cmd = 'call "' + $devCmd + '" -arch=x86 -host_arch=x86 >nul && msbuild build\\msvc\\vs2022\\ScreenSave.sln /m /nologo /p:Configuration=Debug /p:Platform=Win32 /p:PlatformToolset=v141_xp /t:plasma,benchlab'
cmd.exe /d /c $cmd
```

Manual smoke rebuild from the same `v141_xp` object set:

```powershell
$devCmd = 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat'
$cmd = 'call "' + $devCmd + '" -arch=x86 -host_arch=x86 >nul && cl /nologo /c /TC /D _USING_V110_SDK71_ /Iplatform\include /MDd /Zi /Od /W4 /Fo"out\intermediate\msvc\vs2022\Debug\plasma\smoke_u04.obj" /Fd"out\intermediate\msvc\vs2022\Debug\plasma\vc141.pdb" "products\savers\plasma\tests\smoke.c"'
cmd.exe /d /c $cmd

$objects = @(
  'out\intermediate\msvc\vs2022\Debug\plasma\smoke_u04.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_advanced.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_authoring.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_benchlab.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_config.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_content.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_entry.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_modern.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_module.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_output.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_plan.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_premium.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_presentation.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_presets.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_render.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_selection.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_settings.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_sim.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_themes.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_transition.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_treatment.obj',
  'out\intermediate\msvc\vs2022\Debug\plasma\plasma_validate.obj',
  'out\msvc\vs2022\Debug\screensave_platform\screensave_platform.lib'
)
$quoted = ($objects | ForEach-Object { '"' + $_ + '"' }) -join ' '
$cmd = 'call "' + $devCmd + '" -arch=x86 -host_arch=x86 >nul && link /nologo /DEBUG /INCREMENTAL /SUBSYSTEM:CONSOLE,5.01 /OUT:"out\msvc\vs2022\Debug\plasma\plasma_smoke_u04.exe" ' + $quoted + ' advapi32.lib gdi32.lib user32.lib opengl32.lib'
cmd.exe /d /c $cmd

& .\out\msvc\vs2022\Debug\plasma\plasma_smoke_u04.exe
```

Product-local validation and capture refresh:

```powershell
python tools\scripts\plasma_lab.py validate

python tools\scripts\plasma_lab.py degrade-report --capture validation\captures\u04\benchlab-plasma-degrade-ribbon-auto.txt
python tools\scripts\plasma_lab.py capture-diff --left validation\captures\u04\benchlab-plasma-presentation-ribbon.txt --right validation\captures\u04\benchlab-plasma-degrade-ribbon-auto.txt

& .\out\msvc\vs2022\Debug\benchlab\benchlab.exe `
  /deterministic `
  /seed:743 `
  /saver:plasma `
  /frames:4 `
  /report:validation\captures\u04\benchlab-plasma-presentation-ribbon.txt `
  /renderer:gl46 `
  /plasma-presentation:ribbon

& .\out\msvc\vs2022\Debug\benchlab\benchlab.exe `
  /deterministic `
  /seed:743 `
  /saver:plasma `
  /frames:4 `
  /report:validation\captures\u04\benchlab-plasma-degrade-ribbon-auto.txt `
  /plasma-presentation:ribbon

& .\out\msvc\vs2022\Debug\benchlab\benchlab.exe `
  /deterministic `
  /seed:743 `
  /saver:plasma `
  /frames:4 `
  /report:validation\captures\u04\benchlab-plasma-degrade-contour-extrusion-raster.txt `
  /renderer:gl46 `
  /plasma-presentation:contour_extrusion
```

Repository hygiene:

```powershell
git diff --check
```

## What Actual Evidence This Produces

Fresh execution evidence from this checkpoint:

- Plasma and BenchLab rebuilt successfully on the installed VS2022 MSVC lane
  with `v141_xp`
- the manual `plasma_smoke_u04.exe` rebuild completed and exited with code `0`
- `python tools\scripts\plasma_lab.py validate` completed successfully
- the U04 BenchLab captures refreshed successfully
- `plasma_lab.py degrade-report` and `capture-diff` completed successfully

## Evidence That Output Families Now Materially Differ

Actual code-backed evidence from this checkpoint:

- the admitted output subset is centralized in shared support helpers instead of
  private scattered checks
- the smoke harness now requires deterministic render-signature separation for:
  - `banded / posterized_bands`
  - `contour / contour_bands`
  - `glyph / ascii_glyph`
  relative to the raster baseline
- unsupported output-family and output-mode claims now resolve through
  canonical `"unsupported"` tokens rather than product-like names

BenchLab does not independently force output-family changes in this phase, so
the actual output-family distinctness proof comes from the smoke harness rather
than from standalone BenchLab capture forcing.

## Evidence That Treatment Families Now Materially Differ

Actual code-backed evidence from this checkpoint:

- the admitted treatment subset is centralized in shared support helpers
- the smoke harness now requires deterministic render-signature separation for:
  - `glow_edge`
  - `halftone_stipple`
  - `phosphor`
  - `crt`
  - `accent_pass`
  relative to the untreated baseline
- unsupported sampling and unsupported novelty-filter names now resolve to
  `"unsupported"` tokens
- advanced-only treatment slots remain explicitly gated rather than silently
  accepted everywhere

BenchLab still does not independently force treatment-slot changes in this
phase, so the actual treatment distinctness proof comes from the smoke harness
while BenchLab remains responsible for truthful reporting when an admitted
combination is already present in the resolved plan.

## Evidence That Presentation Modes Now Materially Differ

Fresh phase-local evidence from this checkpoint:

- `validation/captures/u04/benchlab-plasma-presentation-ribbon.txt` shows a
  requested premium `ribbon` mode resolving cleanly to `ribbon`
- `validation/captures/u04/benchlab-plasma-degrade-ribbon-auto.txt` shows the
  same requested premium `ribbon` mode degrading back to `flat` on the lower
  lane while keeping requested-versus-resolved truth visible
- `validation/captures/u04/benchlab-plasma-degrade-contour-extrusion-raster.txt`
  shows `contour_extrusion` degrading back to `flat` when the resolved output
  family is `raster`
- the smoke harness requires a premium `ribbon` presentation signature to differ
  materially from its `flat` baseline

## Evidence That Degrade Behavior Is Honest

The current checkpoint materially proves:

- premium-only presentation requests remain visible as requested state even when
  the resolved path falls back to `flat`
- incompatible `contour_extrusion` requests clamp cleanly to `flat` with a
  `presentation` clamp summary instead of being silently treated as valid
- validation and known-limit lookups now point to the narrower admitted grammar
  subset instead of broader historical phase claims

## Passed, Failed, And Blocked

Passed:

- `MSBuild` rebuild for `plasma` and `benchlab`
- manual `plasma_smoke_u04.exe` rebuild
- `plasma_smoke_u04.exe`
- `python tools\scripts\plasma_lab.py validate`
- `python tools\scripts\plasma_lab.py degrade-report --capture validation\captures\u04\benchlab-plasma-degrade-ribbon-auto.txt`
- `python tools\scripts\plasma_lab.py capture-diff --left validation\captures\u04\benchlab-plasma-presentation-ribbon.txt --right validation\captures\u04\benchlab-plasma-degrade-ribbon-auto.txt`
- refreshed U04 BenchLab captures
- `git diff --check`

Failed:

- none in this checkpoint

Blocked:

- BenchLab still does not provide independent forcing for output-family or
  treatment-slot overrides, so those distinctness checks remain smoke-backed
- there is still no universal screenshot or perceptual-diff framework in this
  repo
- this checkpoint does not perform the later preset retune or destructive
  simplification pass

## Actual Evidence Versus Documentation Only

Actual execution evidence:

- successful `v141_xp` rebuilds
- successful manual smoke rebuild and execution
- successful U04 capture refresh
- successful `degrade-report` and `capture-diff` runs

Documentation-only or source-inspection claims:

- final keep, merge, hide, or remove decisions for the broader taxonomy
- subjective quality ranking of every experimental preset after the stronger
  grammar becomes available
- universal cross-renderer perceptual parity
