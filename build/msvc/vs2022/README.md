# VS2022 Lane

This is the concrete modern MSVC lane for Series 02.

## What Exists Now

- `ScreenSave.sln`
- `screensave_platform_stub.vcxproj`
- `nocturne_stub.vcxproj`

These files are intentionally small.
They define one shared static-library target and one stub saver target so the repository has a real checked-in build graph without claiming real screensaver behavior exists yet.

## What The Targets Mean

- `screensave_platform_stub` is a build-only placeholder for shared platform linkage.
- `nocturne_stub` is a build-only `.scr` target that proves the product lane, output conventions, and toolchain wiring.

Both targets use tiny Series 02 scaffold sources.
Later series should replace those sources rather than adding parallel placeholder targets.

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

- real host implementation
- real saver logic
- real resources, metadata, and version stamping
- broader solution structure for additional products and apps
