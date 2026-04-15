# Plasma U06 Retuning Proof

## Purpose

This note records the actual evidence used for the `U06` retuning checkpoint.

It distinguishes fresh execution evidence from carried-forward documentation and
source-inspection claims.

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
$cmd = 'call "' + $devCmd + '" -arch=x86 -host_arch=x86 >nul && cl /nologo /c /TC /D _USING_V110_SDK71_ /Iplatform\include /MDd /Zi /Od /W4 /Fo"out\intermediate\msvc\vs2022\Debug\plasma\smoke_u06.obj" /Fd"out\intermediate\msvc\vs2022\Debug\plasma\vc141.pdb" "products\savers\plasma\tests\smoke.c"'
cmd.exe /d /c $cmd

$objects = @(
  'out\intermediate\msvc\vs2022\Debug\plasma\smoke_u06.obj',
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
$cmd = 'call "' + $devCmd + '" -arch=x86 -host_arch=x86 >nul && link /nologo /DEBUG /INCREMENTAL /SUBSYSTEM:CONSOLE,5.01 /OUT:"out\msvc\vs2022\Debug\plasma\plasma_smoke_u06.exe" ' + $quoted + ' advapi32.lib gdi32.lib user32.lib opengl32.lib'
cmd.exe /d /c $cmd

& .\out\msvc\vs2022\Debug\plasma\plasma_smoke_u06.exe
```

Product-local validation and audit refresh:

```powershell
python tools\scripts\plasma_lab.py validate
python tools\scripts\plasma_lab.py preset-audit --threshold 2
```

Capture and support-report refresh:

```powershell
New-Item -ItemType Directory -Force validation\captures\u06 | Out-Null

& .\out\msvc\vs2022\Debug\benchlab\benchlab.exe `
  /deterministic `
  /seed:743 `
  /saver:plasma `
  /frames:4 `
  /report:validation\captures\u06\benchlab-plasma-default-gdi.txt `
  /renderer:gdi

& .\out\msvc\vs2022\Debug\benchlab\benchlab.exe `
  /deterministic `
  /seed:743 `
  /saver:plasma `
  /frames:4 `
  /report:validation\captures\u06\benchlab-plasma-default-gl11.txt `
  /renderer:gl11

& .\out\msvc\vs2022\Debug\benchlab\benchlab.exe `
  /deterministic `
  /seed:424242 `
  /saver:plasma `
  /frames:4 `
  /report:validation\captures\u06\benchlab-plasma-classic-cycle-gl11.txt `
  /renderer:gl11 `
  /plasma-preset:plasma_lava `
  /plasma-theme:plasma_lava `
  /plasma-transitions:true `
  /plasma-transition-policy:journey `
  /plasma-journey:classic_cycle

python tools\scripts\plasma_lab.py control-report --profile deterministic_classic | Out-File -Encoding ascii validation\captures\u06\control-profile-deterministic-classic.txt
python tools\scripts\plasma_lab.py control-report --profile dark_room_support | Out-File -Encoding ascii validation\captures\u06\control-profile-dark-room-support.txt

python tools\scripts\plasma_lab.py capture-diff --left D:\Projects\ScreenSave\screensave\validation\captures\u06\benchlab-plasma-default-gdi.txt --right D:\Projects\ScreenSave\screensave\validation\captures\u06\benchlab-plasma-default-gl11.txt | Out-File -Encoding ascii validation\captures\u06\capture-diff-default-gdi-vs-gl11.txt
python tools\scripts\plasma_lab.py capture-diff --left D:\Projects\ScreenSave\screensave\validation\captures\u06\benchlab-plasma-default-gl11.txt --right D:\Projects\ScreenSave\screensave\validation\captures\u06\benchlab-plasma-classic-cycle-gl11.txt | Out-File -Encoding ascii validation\captures\u06\capture-diff-default-gl11-vs-classic-cycle.txt
```

Repository hygiene:

```powershell
git diff --check
```

## What Actual Evidence This Produces

Fresh execution evidence from this checkpoint:

- Plasma and BenchLab rebuilt successfully on the installed VS2022 MSVC lane
  with `v141_xp`
- the manual `plasma_smoke_u06.exe` rebuild completed and exited with code `0`
- `python tools\scripts\plasma_lab.py validate` completed successfully
- `python tools\scripts\plasma_lab.py preset-audit --threshold 2` completed
  successfully
- the checked-in U06 BenchLab captures, control-profile reports, and
  capture-diff reports refreshed successfully

The rebuild still emits the known VS2022 `v141_xp` deprecation warning and the
manual smoke compile still emits the existing `strncpy` warning inside the smoke
helper, but neither warning blocked the checkpoint.

## Evidence That Surviving Presets And Themes Are More Distinct

Fresh audit evidence from this checkpoint:

- no compiled preset pairs remain within the audit's near-duplicate threshold
  of `<= 2`
- the closest stable preset pair is now `museum_phosphor <-> plasma_lava` at
  distance `3`
- the closest theme palette pair is now
  `midnight_interference <-> oceanic_blue` at distance `146`
- the previous warm clustering is reduced;
  `amber_terminal <-> plasma_lava` now sits at distance `181`

This is bounded evidence, not the later screenshot-grade QA bar, but it is
enough to show that the surviving catalogue is materially less crowded than it
was before `U06`.

## Evidence That The Default Path Is Stronger

Fresh U06 capture evidence:

- `validation/captures/u06/benchlab-plasma-default-gdi.txt` shows the preserved
  default path still resolving to `plasma_lava` / `plasma_lava` on the truthful
  `gdi` floor
- `validation/captures/u06/benchlab-plasma-default-gl11.txt` shows the same
  default path resolving cleanly on the truthful `gl11` preferred stable lane
- both captures report the same stable grammar:
  `fire`, `gentle`, `standard`, `soft`, `raster`, `native_raster`, and `flat`
- `validation/captures/u06/capture-diff-default-gdi-vs-gl11.txt` shows that the
  default-path delta is now renderer and lane routing truth rather than a
  silent change in preset or theme identity

## Evidence That Preview, Fullscreen Intent, And Long-Run Behavior Are More Coherent

Fresh execution and support-surface evidence from this checkpoint:

- `validation/captures/u06/benchlab-plasma-classic-cycle-gl11.txt` shows the
  retuned journey path keeping `plasma_lava` / `plasma_lava` while enabling
  `classic_cycle` transitions
- `validation/captures/u06/capture-diff-default-gl11-vs-classic-cycle.txt`
  isolates the intended delta to journey and transition posture, not a surprise
  grammar swap
- `validation/captures/u06/control-profile-deterministic-classic.txt` and
  `validation/captures/u06/control-profile-dark-room-support.txt` show the two
  stable deterministic review profiles now routing through
  `classic_cycle` and `dark_room_cycle`, with the dark-room profile explicitly
  held at low detail

Source-backed but not independently fullscreen-captured claims in this
checkpoint:

- preview variation cadence now uses `9000ms`
- preview resolution penalty is reduced to one extra divisor step
- default transition timing is now `14000ms / 1800ms`
- authored stable journey dwell values were lengthened for calmer long-run use

Those values are current repo truth, but the repo still lacks the later
fullscreen-host capture layer.

## Evidence That Lower-Band Degrade Behavior Is More Supportable

This checkpoint materially proves:

- the truthful `gdi` floor still presents the default stable path
- the truthful `gl11` preferred lane still presents the same default content
  identity without widening stable scope
- the `preview_safe` profile and `dark_room_support` control profile were
  refreshed to calmer, more supportable defaults

The last point is partly source and profile-report evidence rather than a
standalone visual-diff proof.
That is acceptable for `U06`, but not the later final QA bar.

## Passed, Failed, And Blocked

Passed:

- `MSBuild` rebuild for `plasma` and `benchlab`
- manual `plasma_smoke_u06.exe` rebuild
- `plasma_smoke_u06.exe`
- `python tools\scripts\plasma_lab.py validate`
- `python tools\scripts\plasma_lab.py preset-audit --threshold 2`
- refreshed U06 BenchLab captures
- refreshed U06 control-profile reports
- refreshed U06 capture-diff reports
- `git diff --check`

Failed:

- none in this checkpoint

Blocked:

- this checkpoint still does not add the later fullscreen `.scr` host capture
  layer
- BenchLab preset forcing still changes preset identity rather than replaying
  the full preset bundle
- the repo still does not have the later screenshot-grade perceptual harness or
  final visual QA gate

## Actual Evidence Versus Documentation Only

Actual execution evidence:

- successful `v141_xp` rebuilds
- successful manual smoke rebuild and execution
- successful validation and preset audit
- successful BenchLab capture, control-report, and capture-diff refresh

Documentation-only or source-inspection claims:

- final keep or remove decisions for every surviving experimental preset
- final visual ranking of every surviving theme
- fullscreen `.scr` host parity with BenchLab and preview paths
