# Plasma U03 UI Proof

## Purpose

This note records the actual evidence used for the `U03` settings-surface
checkpoint.

It distinguishes fresh execution evidence from documentation-only claims.

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
$cmd = 'call "' + $devCmd + '" -arch=x86 -host_arch=x86 >nul && cl /nologo /c /TC /D _USING_V110_SDK71_ /Iplatform\include /MDd /Zi /Od /W4 /Fo"out\intermediate\msvc\vs2022\Debug\plasma\smoke_u03.obj" /Fd"out\intermediate\msvc\vs2022\Debug\plasma\vc141.pdb" "products\savers\plasma\tests\smoke.c"'
cmd.exe /d /c $cmd

$objects = @(
  'out\intermediate\msvc\vs2022\Debug\plasma\smoke_u03.obj',
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
$cmd = 'call "' + $devCmd + '" -arch=x86 -host_arch=x86 >nul && link /nologo /DEBUG /INCREMENTAL /SUBSYSTEM:CONSOLE,5.01 /OUT:"out\msvc\vs2022\Debug\plasma\plasma_smoke_u03.exe" ' + $quoted + ' advapi32.lib gdi32.lib user32.lib opengl32.lib'
cmd.exe /d /c $cmd

& .\out\msvc\vs2022\Debug\plasma\plasma_smoke_u03.exe
```

Authoring validation:

```powershell
python tools\scripts\plasma_lab.py validate
```

Repository hygiene:

```powershell
git diff --check
```

## What Actual Evidence This Produces

Fresh execution evidence from this checkpoint:

- Plasma and BenchLab rebuilt successfully on the installed VS2022 MSVC lane
  with `v141_xp`
- the manual `plasma_smoke_u03.exe` rebuild completed and exited with code `0`
- `python tools\scripts\plasma_lab.py validate` completed successfully

Smoke assertions exercised by this checkpoint include:

- exact Basic, Advanced, and Author/Lab surface membership through the new
  `plasma_settings_surface_contains_setting_key` helper
- preserved settings catalog integrity and current availability gating
- preserved default stable path behavior while the dialog controller changed

## Evidence That The New Surfaces Exist As Described

Actual code-backed evidence from this checkpoint:

- the dialog controller now uses exact surface membership instead of cumulative
  descriptor ordering
- routine dialog refresh now captures only the current visible surface
- surface switching preserves deeper state intentionally rather than rereading
  hidden widgets opportunistically
- the info panel now reports active preset/theme identity and major hidden or
  gated state

## Evidence That Surviving Controls Are Meaningful

The current checkpoint materially re-verifies:

- Basic controls still map to real preset, theme, speed, detail, content-pool,
  and transition settings
- Advanced controls still map to real generator, output, treatment, and
  presentation fields
- Author/Lab controls still map to real selection-set, journey, transition,
  seed-value, and diagnostics fields

## Evidence That Hidden Or Disabled Controls Are Justified

This checkpoint explicitly keeps these truths visible:

- single-mode output families disable `Output Mode` instead of pretending an
  extra choice exists
- unavailable favorites, journey, transition, and deterministic-seed-value
  controls disable honestly
- `sampling_treatment` remains hidden because it still clamps to `none`

## Passed, Failed, And Blocked

Passed:

- `MSBuild` rebuild for `plasma` and `benchlab`
- manual `plasma_smoke_u03.exe` rebuild
- `plasma_smoke_u03.exe`
- `python tools\scripts\plasma_lab.py validate`
- `git diff --check`

Failed:

- none during this checkpoint

Blocked:

- there is still no screenshot-backed Win32 dialog automation harness in this
  repo, so layout and copy are source-inspected rather than capture-proven

## Actual Evidence Versus Documentation Only

Actual execution evidence:

- successful `v141_xp` rebuilds
- successful manual smoke rebuild and execution
- successful `plasma_lab.py validate`

Documentation-only or source-inspection claims:

- exact pixel layout of the recut dialog
- final subjective readability of the info panel copy on every host font
- later keep versus remove decisions for broader Advanced or Author/Lab
  controls
