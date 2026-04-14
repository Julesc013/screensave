# Plasma U01 Runtime Proof

## Purpose

This note records the actual evidence used for the `U01` runtime-unification
checkpoint.

It distinguishes fresh execution evidence from inherited documentation.

## Commands Actually Run

MSVC XP-compatible rebuild:

```powershell
& 'C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe' `
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

cmd /c "call `"$devCmd`" -arch=x86 -host_arch=x86 >nul && cl /nologo /c /TC /D _USING_V110_SDK71_ /Iplatform\include /MDd /Zi /Od /W4 /Fo`"out\intermediate\msvc\vs2022\Debug\plasma\smoke.obj`" /Fd`"out\intermediate\msvc\vs2022\Debug\plasma\vc141.pdb`" `"products\savers\plasma\tests\smoke.c`""

cmd /c "call `"$devCmd`" -arch=x86 -host_arch=x86 >nul && link /nologo /DEBUG /INCREMENTAL /SUBSYSTEM:CONSOLE,5.01 /OUT:`"out\msvc\vs2022\Debug\plasma\plasma_smoke.exe`" `"out\intermediate\msvc\vs2022\Debug\plasma\smoke.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_advanced.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_authoring.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_benchlab.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_config.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_content.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_entry.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_modern.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_module.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_output.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_plan.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_premium.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_presentation.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_presets.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_render.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_selection.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_settings.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_sim.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_themes.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_transition.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_treatment.obj`" `"out\intermediate\msvc\vs2022\Debug\plasma\plasma_validate.obj`" `"out\msvc\vs2022\Debug\screensave_platform\screensave_platform.lib`" advapi32.lib gdi32.lib user32.lib opengl32.lib"

& .\out\msvc\vs2022\Debug\plasma\plasma_smoke.exe
```

Authoring and content validation:

```powershell
python tools\scripts\plasma_lab.py validate
git diff --check
```

## What Actual Evidence This Produces

Fresh execution evidence from this checkpoint:

- Plasma and BenchLab rebuilt successfully on the installed VS2022 MSVC lane
  with `v141_xp`
- the manually rebuilt `plasma_smoke.exe` completed with exit code `0`
- authored-content and compatibility validation completed successfully through
  `plasma_lab.py validate`

Smoke assertions exercised by this checkpoint include:

- canonical content helpers and Classic compatibility wrappers resolve the same
  preserved alias behavior
- the default stable path still compiles and validates on `gdi` and `gl11`
- canonical validation keys resolve
- legacy Classic validation keys still resolve as aliases

## Evidence That Classic Runtime Dependence Was Removed

The current checkpoint materially proves:

- the runtime no longer depends on a `classic_execution`-style plan flag
- direct-plan smoke coverage no longer needs a Classic-mode helper name
- canonical validation entries now describe the preserved default stable path,
  not a Classic execution regime

## Evidence That Classic Content And Defaults Remain Valid

This checkpoint also re-verifies:

- `plasma_lava` remains the default preset and theme
- `ember_lava -> plasma_lava` still resolves
- Classic-named content collections and journey identity remain part of the
  preserved compatibility surface
- legacy helper and validation lookup names still resolve for older callers

## Passed, Failed, And Blocked

Passed:

- `MSBuild` rebuild for `plasma` and `benchlab`
- manual `plasma_smoke.exe` rebuild
- `plasma_smoke.exe`
- `python tools\scripts\plasma_lab.py validate`
- `git diff --check`

Failed:

- none in this checkpoint

Blocked:

- no fresh cross-hardware or capture-lab expansion was attempted in `U01`
- no new screenshot or perceptual-diff evidence is claimed here

## Documentation-Only Carry-Forward

This checkpoint still relies on inherited checked-in evidence for:

- earlier PL13 lane captures
- earlier transition and release-cut proof notes
- broader stable-versus-experimental posture

Those remain useful, but they are not claimed here as fresh reruns.
