# MinGW i686 Lane

This is the concrete GCC-family x86 lane for Series 08.

## What Exists Now

- `Makefile` for the shared platform library, the real `nocturne.scr` target, the real `benchlab.exe` target, the core-plus-GDI-plus-GL11 platform objects, the Nocturne product objects, the host/product dialog resources, and the conservative `opengl32` link dependency required for the optional backend

The target graph mirrors the VS2022 lane closely so later implementation work does not have to invent two unrelated build stories.

## Entry Path

The checked-in entry path for this lane is `build/mingw/i686/Makefile`.
When a MinGW i686 toolchain is available, the expected invocation shape is:

- `mingw32-make -C build/mingw/i686 PROFILE=debug`

The current makefile assumes a MinGW shell environment that can run commands such as `mkdir -p`, `rm -rf`, and `windres`.
That assumption remains documented here because it has not been validated yet on every Windows host setup.

## X86 Intent

This lane targets 32-bit Windows output only.
That matches the constitutional saver baseline and keeps the build scaffold aligned with the long-term compatibility contract.

## Output Conventions

Generated output belongs under:

- `out/mingw/i686/<profile>/<target>/`
- `out/intermediate/mingw/i686/<profile>/<target>/`

Profiles are currently `debug` or `release`.

## Deferred Work

- historical MinGW variant documentation beyond this lane
- GL-plus and later renderer tiers
- validation on actual target machines

This lane is concrete enough for static review and later extension, not a claim of complete shipping readiness.
