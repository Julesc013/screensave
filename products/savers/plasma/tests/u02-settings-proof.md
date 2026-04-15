# Plasma U02 Settings Proof

## Purpose

This note records the actual evidence used for the `U02` settings-authority
checkpoint.

It distinguishes fresh execution evidence from inherited documentation.

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
$cmd = 'call "' + $devCmd + '" -arch=x86 -host_arch=x86 >nul && cl /nologo /c /TC /D _USING_V110_SDK71_ /Iplatform\include /MDd /Zi /Od /W4 /Fo"out\intermediate\msvc\vs2022\Debug\plasma\smoke_u02.obj" /Fd"out\intermediate\msvc\vs2022\Debug\plasma\vc141.pdb" "products\savers\plasma\tests\smoke.c"'
cmd.exe /d /c $cmd

$objects = @(
  'out\intermediate\msvc\vs2022\Debug\plasma\smoke_u02.obj',
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
$cmd = 'call "' + $devCmd + '" -arch=x86 -host_arch=x86 >nul && link /nologo /DEBUG /INCREMENTAL /SUBSYSTEM:CONSOLE,5.01 /OUT:"out\msvc\vs2022\Debug\plasma\plasma_smoke_u02.exe" ' + $quoted + ' advapi32.lib gdi32.lib user32.lib opengl32.lib'
cmd.exe /d /c $cmd

& .\out\msvc\vs2022\Debug\plasma\plasma_smoke_u02.exe
```

Authoring validation and capture refresh:

```powershell
python tools\scripts\plasma_lab.py validate

& .\out\msvc\vs2022\Debug\benchlab\benchlab.exe `
  /deterministic `
  /seed:743 `
  /saver:plasma `
  /frames:4 `
  /report:validation\captures\u02\benchlab-plasma-degrade-heightfield-auto.txt `
  /plasma-presentation:heightfield
```

Repository hygiene:

```powershell
git diff --check
```

## What Actual Evidence This Produces

Fresh execution evidence from this checkpoint:

- Plasma and BenchLab rebuilt successfully on the installed VS2022 MSVC lane
  with `v141_xp`
- the manual `plasma_smoke_u02.exe` rebuild completed and exited with code `0`
- `python tools\scripts\plasma_lab.py validate` completed successfully
- the refreshed `validation/captures/u02/benchlab-plasma-degrade-heightfield-auto.txt`
  capture shows requested premium presentation truth degrading honestly back to
  the lower-lane resolved flat presentation

Smoke assertions exercised by this checkpoint include:

- settings-owned generator, output, treatment, and presentation fields driving
  the compiled plan directly
- advanced-treatment degradation that keeps `blur` and `overlay_pass`
  requested while resolving both slots back to `none` on lower lanes
- requested-versus-resolved BenchLab report lines for generator, output,
  treatment, and presentation grammar
- preserved default stable path behavior on the `plasma_lava` baseline

## Evidence That Settings Now Influence Runtime-Plan Fields

The current checkpoint materially proves:

- preset loading seeds config through an explicit bundle helper instead of
  pretending to be the final runtime authority
- the compiled plan copies requested grammar through
  `plasma_plan_apply_settings_resolution`
- BenchLab snapshot and report builders now expose requested and resolved
  values for the shipped grammar subset
- presentation forcing updates the requested presentation field before degrade,
  so requested and resolved truth no longer collapse together in the report

## Evidence That Hidden Preset Overrides Were Reduced

This checkpoint also re-verifies:

- product defaults and preset-bundle seeding are separate steps
- explicit persisted or session-level overrides remain free to replace preset
  bundle defaults before plan compilation
- BenchLab `/plasma-preset:` remains a selection override rather than a hidden
  bundle re-import path, which keeps its current bounded semantics explicit

## Evidence That Unsupported Combinations Fail Honestly

The current proof surface includes:

- advanced-only treatment requests degrading back to `none` with requested
  values still reported
- premium-only presentation requests degrading back to `flat` with requested
  presentation still reported
- clamp reporting that names the surviving unsupported-path outcome instead of
  silently erasing it

## Passed, Failed, And Blocked

Passed:

- `MSBuild` rebuild for `plasma` and `benchlab`
- manual `plasma_smoke_u02.exe` rebuild
- `plasma_smoke_u02.exe`
- `python tools\scripts\plasma_lab.py validate`
- refreshed U02 BenchLab capture
- `git diff --check`

Failed:

- none in this checkpoint

Blocked:

- no fullscreen visual-host proof was rerun in `U02`
- no screenshot-diff or perceptual image-diff framework is claimed here
- `sampling_treatment` remains a bounded non-claim surface and therefore is not
  proven as a distinct shipped render path

## Documentation-Only Carry-Forward

This checkpoint still relies on inherited checked-in evidence for:

- older PL13 lane captures outside the focused U02 degrade refresh
- broader release-cut, stable-versus-experimental, and retune claims
- later visual distinctness and QA work beyond settings authority

Those remain useful, but they are not claimed here as fresh reruns.
