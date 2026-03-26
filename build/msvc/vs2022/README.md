# VS2022 Lane

This is the concrete modern MSVC lane for Series 04.

## What Exists Now

- `ScreenSave.sln`
- `screensave_platform.vcxproj`
- `nocturne.vcxproj`

These files stay intentionally small.
They define one shared static-library target and one `.scr` saver target with a shared core runtime, a real host skeleton, and a no-op saver-module shell.

## What The Targets Mean

- `screensave_platform` compiles the shared non-renderer core runtime plus the Win32 host implementation.
- `nocturne` compiles the no-op saver-module entry point, host dialog resource, and links the first real `.scr` executable target.

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

- reusable renderer backends
- real saver-product behavior
- richer version stamping and metadata resources
- broader solution structure for additional products and apps
