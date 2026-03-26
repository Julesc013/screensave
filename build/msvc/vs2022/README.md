# VS2022 Lane

This is the concrete modern MSVC lane for Series 08.

## What Exists Now

- `ScreenSave.sln`
- `screensave_platform.vcxproj`
- `nocturne.vcxproj`
- `benchlab.vcxproj`

These files stay intentionally small.
They define one shared static-library target, one `.scr` saver target, and one windowed diagnostics app target with a shared core runtime, the mandatory GDI backend, the optional GL11 backend, a real host skeleton, the first real saver product, and the first real diagnostics app.

## What The Targets Mean

- `screensave_platform` compiles the shared non-renderer core runtime, the mandatory GDI backend, the optional GL11 backend, and the Win32 host implementation.
- `nocturne` compiles the real Nocturne product sources, the product config dialog resource, the shared host resource, links `opengl32` for the optional GL11 path, and links the first real `.scr` executable target.
- `benchlab` compiles the BenchLab app sources, reuses the real Nocturne product sources through the shared saver contract, links `opengl32` for explicit GL11 validation, and links the first real diagnostics harness executable target.

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
- broader solution structure for additional savers and apps
