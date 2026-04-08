# VS2022 Lane

This is the concrete modern MSVC lane for the frozen `C16` baseline plus the current `SX06` substrate state.

## What Exists Now

- `ScreenSave.sln`
- `screensave_platform.vcxproj`
- `nocturne.vcxproj`
- `ricochet.vcxproj`
- `deepfield.vcxproj`
- `plasma.vcxproj`
- `phosphor.vcxproj`
- `pipeworks.vcxproj`
- `lifeforms.vcxproj`
- `signals.vcxproj`
- `mechanize.vcxproj`
- `ecosystems.vcxproj`
- `stormglass.vcxproj`
- `transit.vcxproj`
- `observatory.vcxproj`
- `vector.vcxproj`
- `explorer.vcxproj`
- `city.vcxproj`
- `atlas.vcxproj`
- `gallery.vcxproj`
- `anthology.vcxproj`
- `benchlab.vcxproj`
- `suite.vcxproj`

These files stay intentionally small.
They define one shared static-library target, nineteen first-class `.scr` saver targets, and two windowed app targets with a shared core runtime, the mandatory GDI backend, the optional GL11 backend, the optional GL21 backend, the real optional GL33 modern lane, the real optional GL46 premium lane, the internal null safety backend, a real host skeleton, nineteen real saver products, and the current app pair.

## What The Targets Mean

- `screensave_platform` compiles the shared non-renderer core runtime, the mandatory GDI backend, the optional GL11 backend, the optional GL21 backend, the real optional GL33 modern lane, the real optional GL46 premium lane, the internal null safety backend, and the Win32 host implementation.
- `nocturne` through `gallery` each compile only their owning saver sources, their product config dialog resource, their product version resource, and the shared host resource, then emit the matching saver-named `.scr` target.
- `anthology` stays explicit as the suite meta-saver target: it compiles its owning anthology sources plus the current eighteen inner saver modules in-process, then emits `anthology.scr` without turning the build graph into a plugin loader or external-orchestration wrapper.
- Each saver target binds directly to its owning saver module through the shared host entry instead of depending on the older multi-saver default-selection distribution pattern.
- `benchlab` compiles the BenchLab app sources, reuses the real Nocturne, Ricochet, Deepfield, Plasma, Phosphor, Pipeworks, Lifeforms, Signals, Mechanize, Ecosystems, Stormglass, Transit, Observatory, Vector, Explorer, City, Atlas, Gallery, and Anthology product sources through the shared saver contract, links `opengl32` for explicit tier validation, and links the diagnostics harness executable target.
- `suite` compiles the Suite app sources, reuses the real saver modules plus saver-owned config dialog resources for bounded preview, detached windowed run, metadata browsing, and saver `Settings...` handoff, and emits `suite.exe` without replacing the standalone `.scr` products, `anthology`, or BenchLab.

The concrete host lifecycle now lives in the checked-in target graph.
Later series should extend these targets in place instead of introducing parallel demo targets.

## Entry Path

The checked-in entry path for this lane is `build/msvc/vs2022/ScreenSave.sln`.
When a VS2022 toolchain is available, the expected invocation shape is:

- open the solution in Visual Studio, or
- run `msbuild build\msvc\vs2022\ScreenSave.sln /p:Configuration=Debug /p:Platform=Win32`

## Output Conventions

The projects write outputs under:

- `out/msvc/vs2022/<Configuration>/<ProjectName>/`
- `out/intermediate/msvc/vs2022/<Configuration>/<ProjectName>/`

No generated files should be checked into this directory.

## Deferred Work

- later renderer work beyond the current GL21, GL33, and GL46 paths
- packaging, installer, and release-production work
- broader solution structure for later suite-level products beyond the current real `suite` app
