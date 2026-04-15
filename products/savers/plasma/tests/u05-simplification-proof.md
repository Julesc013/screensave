# Plasma U05 Simplification Proof

## Purpose

This note records the actual evidence used for the `U05` destructive
simplification checkpoint.

It distinguishes fresh execution evidence from documentation-only judgment.

## Commands Actually Run

MSVC XP-compatible rebuild:

```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" `
  build\msvc\vs2022\ScreenSave.sln `
  /m /nologo `
  /p:Configuration=Debug `
  /p:Platform=Win32 `
  /p:PlatformToolset=v141_xp `
  /t:plasma,benchlab
```

Manual smoke rebuild from the same `v141_xp` object set:

```powershell
$devCmd = 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat'
$cmd = 'call "' + $devCmd + '" -arch=x86 -host_arch=x86 >nul && cl /nologo /c /TC /D _USING_V110_SDK71_ /Iplatform\include /MDd /Zi /Od /W4 /Fo"out\intermediate\msvc\vs2022\Debug\plasma\smoke_u05.obj" /Fd"out\intermediate\msvc\vs2022\Debug\plasma\vc141.pdb" "products\savers\plasma\tests\smoke.c"'
cmd.exe /d /c $cmd

$objects = @(
  'out\intermediate\msvc\vs2022\Debug\plasma\smoke_u05.obj',
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
$cmd = 'call "' + $devCmd + '" -arch=x86 -host_arch=x86 >nul && link /nologo /DEBUG /INCREMENTAL /SUBSYSTEM:CONSOLE,5.01 /OUT:"out\msvc\vs2022\Debug\plasma\plasma_smoke_u05.exe" ' + $quoted + ' advapi32.lib gdi32.lib user32.lib opengl32.lib'
cmd.exe /d /c $cmd

& .\out\msvc\vs2022\Debug\plasma\plasma_smoke_u05.exe
```

Product-local validation and audit refresh:

```powershell
python tools\scripts\plasma_lab.py validate
python tools\scripts\plasma_lab.py preset-audit --threshold 2
```

Repository hygiene:

```powershell
git diff --check
```

## What Actual Evidence This Produces

Fresh execution evidence from this checkpoint:

- Plasma and BenchLab rebuilt successfully on the installed VS2022 MSVC lane
  with `v141_xp`
- the manual `plasma_smoke_u05.exe` rebuild completed and exited with code `0`
- `python tools\scripts\plasma_lab.py validate` completed successfully
- `python tools\scripts\plasma_lab.py preset-audit --threshold 2` completed
  successfully

## Evidence That Dead Settings And Surfaces Were Weak Enough To Cut

Actual code-backed evidence from this checkpoint:

- the smoke harness now proves `content_filter` and `transitions_enabled` live
  on `Advanced`, not `Basic`
- the smoke harness now proves `favorites_only` is hidden even when favorites
  are configured, which keeps the filter real without leaving a dishonest dialog
  toggle behind
- shared helper functions now distinguish first-class versus compatibility-only
  treatment and presentation values directly in code
- the dialog now preserves hidden compatibility values as explicit
  `(Compatibility)` choices instead of silently dropping them

## Evidence That Near-Duplicate Content Was Weak Enough To Demote

Fresh audit evidence from this checkpoint:

- `amber_terminal <-> museum_phosphor` remains a distance-2 preset pair in the
  deterministic preset audit
- `midnight_interference <-> plasma_lava` remains a distance-2 preset pair in
  the same audit
- `aurora_curtain <-> cellular_bloom` remains a distance-2 pair, which justified
  keeping `cellular_bloom` first-class and demoting `aurora_curtain`
- the authored set validation now passes after the stable and curated metadata
  dropped those demoted first-class members

The preset audit is not a full perceptual lab, but it is strong enough to back
the subtractive decisions made here.

## Evidence That The Stable Default Path Still Works

This checkpoint materially proves:

- the `v141_xp` build still succeeds for both `plasma` and `benchlab`
- the manual smoke binary still exits with code `0`
- the preserved default baseline continues to resolve through the smaller
  first-class surface without breaking alias or compatibility behavior

## Passed, Failed, And Blocked

Passed:

- `MSBuild` rebuild for `plasma` and `benchlab`
- manual `plasma_smoke_u05.exe` rebuild
- `plasma_smoke_u05.exe`
- `python tools\scripts\plasma_lab.py validate`
- `python tools\scripts\plasma_lab.py preset-audit --threshold 2`
- `git diff --check`

Failed:

- none in this checkpoint

Blocked:

- this checkpoint does not add a new screenshot or perceptual-diff proof layer
- `preset-audit` remains a deterministic signature-distance report, not a full
  visual quality judge
- `U05` does not retune the surviving presets or themes yet

## Actual Evidence Versus Documentation Only

Actual execution evidence:

- successful `v141_xp` rebuilds
- successful manual smoke rebuild and execution
- successful authored-surface validation
- successful deterministic preset-crowding audit

Documentation-only or source-inspection claims:

- final visual quality ranking of the surviving first-class presets
- final keep versus remove decisions for every compatibility-only value
- any claim that the narrowed surface is already fully retuned
