# MinGW i686 Lane

This is the concrete GCC-family x86 lane for Series 02.

## What Exists Now

- `Makefile` for the build-only platform stub library and the build-only `nocturne.scr` target

The target graph mirrors the VS2022 lane closely so later implementation work does not have to invent two unrelated build stories.

## Entry Path

The checked-in entry path for this lane is `build/mingw/i686/Makefile`.
When a MinGW i686 toolchain is available, the expected invocation shape is:

- `mingw32-make -C build/mingw/i686 PROFILE=debug`

The current makefile assumes a standard MinGW shell environment that can run commands such as `mkdir -p` and `rm -rf`.
That assumption is documented here because it has not been validated yet on every Windows host setup.

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
- real resource handling
- real saver metadata
- validation on actual target machines

This lane is concrete enough for static review and later extension, not a claim of complete shipping readiness.
