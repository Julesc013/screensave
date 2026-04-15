# Plasma U07 Settings Influence Proof

## Purpose

This note records the actual U07 evidence used to prove that surviving Plasma
controls materially affect the runtime plan and, where supportable, the rendered
result.

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

Manual smoke rebuild and execution:

```powershell
$devCmd = 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat'
$cmd = 'call "' + $devCmd + '" -arch=x86 -host_arch=x86 >nul && cl /nologo /c /TC /D _USING_V110_SDK71_ /Iplatform\include /MDd /Zi /Od /W4 /Fo"out\intermediate\msvc\vs2022\Debug\plasma\smoke_u07.obj" /Fd"out\intermediate\msvc\vs2022\Debug\plasma\vc141.pdb" "products\savers\plasma\tests\smoke.c"'
cmd.exe /d /c $cmd

$objects = @(
  'out\intermediate\msvc\vs2022\Debug\plasma\smoke_u07.obj',
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
$cmd = 'call "' + $devCmd + '" -arch=x86 -host_arch=x86 >nul && link /nologo /DEBUG /INCREMENTAL /SUBSYSTEM:CONSOLE,5.01 /OUT:"out\msvc\vs2022\Debug\plasma\plasma_smoke_u07.exe" ' + $quoted + ' advapi32.lib gdi32.lib user32.lib opengl32.lib'
cmd.exe /d /c $cmd

& .\out\msvc\vs2022\Debug\plasma\plasma_smoke_u07.exe
```

Generated reports:

```powershell
python tools\scripts\plasma_lab.py influence-report
python tools\scripts\plasma_lab.py proof-result-taxonomy
```

## Settings Or Controls Exercised

Fresh execution evidence from this checkpoint proves render-visible influence
for:

- `preset_key`
- `theme_key`
- `effect_mode`
- `detail_level`
- `speed_mode`
- `resolution_mode`
- `smoothing_mode`
- `output_family`
- `output_mode`
- `filter_treatment`
- `emulation_treatment`
- `accent_treatment`
- `presentation_mode`

The deterministic smoke harness now checks these through signature deltas or
stable-matrix validation rather than only through source inspection.

## What Was Found To Matter, Stay Partial, Or Stay Bounded

Validated plan and render influence:

- first-class Basic and Advanced grammar controls in
  `validation/captures/u07/influence-report.txt`

Validated plan influence but only partial render proof:

- `preset_set_key`
- `theme_set_key`
- `transitions_enabled`
- `transition_policy`
- `journey_key`
- transition fallback and seed policies
- transition interval and duration
- deterministic seed policy and explicit seed value
- `content_filter`

These are real controls, but their truth is more honestly proved through
selection routing, BenchLab captures, or temporal behavior than through one
still frame.

Weak or bounded compatibility state:

- `favorites_only` is still only `partial` on plan truth and
  `documented_only` on render truth because it survives as hidden compatibility
  state rather than a first-class U07 proof target
- `diagnostics_overlay_enabled` remains a support-facing surface with bounded
  structural proof rather than a stable visual-identity proof target

## Passed, Failed, And Blocked

Passed:

- XP-compatible `plasma` and `benchlab` rebuild
- manual `plasma_smoke_u07.exe` rebuild
- `plasma_smoke_u07.exe`
- `python tools\scripts\plasma_lab.py influence-report`
- `python tools\scripts\plasma_lab.py proof-result-taxonomy`

Failed:

- none in this checkpoint

Blocked:

- transition and seed surfaces still do not claim one-frame screenshot-grade
  proof
- the repo still does not provide a universal rendered perceptual harness

## Actual Evidence Versus Documentation Only

Actual evidence:

- successful rebuilds
- successful manual smoke execution
- generated influence report
- generated proof-result taxonomy

Documentation-only claims:

- final U08 keep or cut decisions for any still-bounded control
- screenshot-grade perceptual judgments about transition quality
