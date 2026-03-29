# VS2022 Lane

This is the concrete modern MSVC lane for Series 15.

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
- `benchlab.vcxproj`

These files stay intentionally small.
They define one shared static-library target, eighteen `.scr` saver targets, and one windowed diagnostics app target with a shared core runtime, the mandatory GDI backend, the optional GL11 backend, the optional GL21 backend, explicit GL33 and GL46 placeholders, the internal null safety backend, a real host skeleton, eighteen real saver products, and the current diagnostics app.

## What The Targets Mean

- `screensave_platform` compiles the shared non-renderer core runtime, the mandatory GDI backend, the optional GL11 backend, the optional GL21 backend, the explicit GL33 and GL46 placeholders, the internal null safety backend, and the Win32 host implementation.
- `nocturne` compiles the real Nocturne product sources, the current built-in saver registry, the product config dialog resources, the shared host resource, links `opengl32` for the optional GL11 path, and links the `.scr` executable target that defaults to Nocturne.
- `ricochet` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Ricochet.
- `deepfield` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Deepfield.
- `plasma` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Plasma.
- `phosphor` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Phosphor.
- `pipeworks` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Pipeworks.
- `lifeforms` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Lifeforms.
- `signals` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Signals.
- `mechanize` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Mechanize.
- `ecosystems` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Ecosystems.
- `stormglass` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Stormglass.
- `transit` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Transit.
- `observatory` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Observatory.
- `vector` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Vector.
- `explorer` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Explorer.
- `city` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to City.
- `atlas` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Atlas.
- `gallery` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Gallery.
- `benchlab` compiles the BenchLab app sources, reuses the real Nocturne, Ricochet, Deepfield, Plasma, Phosphor, Pipeworks, Lifeforms, Signals, Mechanize, Ecosystems, Stormglass, Transit, Observatory, Vector, Explorer, City, Atlas, and Gallery product sources through the shared saver contract, links `opengl32` for explicit tier validation, and links the diagnostics harness executable target.

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

- later real renderer tiers beyond the current GL21 path and the explicit GL33/GL46 placeholders
- richer version stamping and metadata resources
- broader solution structure for later saver families and suite apps
