# Plasma U01 Runtime Unification

## Purpose

This note records the first corrective runtime cut in the salvage pass:
`Plasma Classic` stops acting like a runtime-regime gate and remains only as preserved content identity, defaults, aliases, and compatibility surface.

## What Changed

- removed the `classic_execution` field from the compiled plan
- removed validation paths that treated that flag as an architectural center
- kept the preserved lower-band default path checks, but grounded them directly in resolved plan behavior instead of in a synthetic classic-mode switch
- stopped valid direct preset and theme requests from being silently rewritten by set, filter, favorite, and exclusion routing; those authored filters now apply only when the request is missing or invalid
- stopped config clamping from replacing invalid preset or theme requests with the default pair before the selection layer could resolve them honestly

## What Did Not Change

- product name remains `Plasma`
- preserved default preset and theme remain `plasma_lava`
- `ember_lava -> plasma_lava` compatibility remains unchanged
- truthful `gdi` floor and truthful `gl11` preferred lane remain unchanged
- `Plasma Classic` remains a preserved content identity and compatibility story

## Unification Rule

After this cut, there is one Plasma runtime plan.

Classic behavior is now defined by:

- the selected content identity
- the resolved lower-band-safe visual grammar
- the preserved default routing posture

It is no longer defined by a dedicated runtime flag.

## Proof

This checkpoint was validated against the installed VS2022 MSVC toolchain with the `v141_xp` platform toolset override so the build lane remains XP-compatible while using the current supported IDE.

- `MSBuild.exe build\\msvc\\vs2022\\ScreenSave.sln /m /nologo /p:Configuration=Debug /p:Platform=Win32 /p:PlatformToolset=v141_xp /t:plasma;benchlab`
- manual `plasma_smoke.exe` rebuild and run from the same `v141_xp`-compatible object set
- `python tools\\scripts\\plasma_lab.py validate`
