# VS2022 Lane

This is the concrete modern MSVC lane for Series 12.

## What Exists Now

- `ScreenSave.sln`
- `screensave_platform.vcxproj`
- `nocturne.vcxproj`
- `ricochet.vcxproj`
- `deepfield.vcxproj`
- `ember.vcxproj`
- `oscilloscope_dreams.vcxproj`
- `pipeworks.vcxproj`
- `lifeforms.vcxproj`
- `signal_lab.vcxproj`
- `mechanical_dreams.vcxproj`
- `ecosystems.vcxproj`
- `benchlab.vcxproj`

These files stay intentionally small.
They define one shared static-library target, ten `.scr` saver targets, and one windowed diagnostics app target with a shared core runtime, the mandatory GDI backend, the optional GL11 backend, a real host skeleton, ten real saver products, and the current diagnostics app.

## What The Targets Mean

- `screensave_platform` compiles the shared non-renderer core runtime, the mandatory GDI backend, the optional GL11 backend, and the Win32 host implementation.
- `nocturne` compiles the real Nocturne product sources, the current built-in saver registry, the product config dialog resources, the shared host resource, links `opengl32` for the optional GL11 path, and links the `.scr` executable target that defaults to Nocturne.
- `ricochet` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Ricochet.
- `deepfield` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Deepfield.
- `ember` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Ember.
- `oscilloscope_dreams` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Oscilloscope Dreams.
- `pipeworks` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Pipeworks.
- `lifeforms` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Lifeforms.
- `signal_lab` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Signal Lab.
- `mechanical_dreams` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Mechanical Dreams.
- `ecosystems` compiles the same shared platform plus saver set and links the `.scr` executable target that defaults to Ecosystems.
- `benchlab` compiles the BenchLab app sources, reuses the real Nocturne, Ricochet, Deepfield, Ember, Oscilloscope Dreams, Pipeworks, Lifeforms, Signal Lab, Mechanical Dreams, and Ecosystems product sources through the shared saver contract, links `opengl32` for explicit backend validation, and links the diagnostics harness executable target.

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

- GL-plus and later renderer tiers
- richer version stamping and metadata resources
- broader solution structure for later saver families and suite apps
