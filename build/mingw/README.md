# MinGW Build Lanes

This directory holds GCC-family Windows lanes that preserve the x86 target intent outside the MSVC family.

## Strategy

The MinGW lane exists so the build story is not defined only by one Microsoft toolchain generation.
At the current `C01` baseline, the practical lane is `i686/`.

## Current State

Real now:

- `i686/` contains a concrete make-based lane for the same shared core, host, mandatory GDI backend, optional GL11 and GL21 capability paths, the current canonical saver targets through `gallery.scr`, and the real BenchLab app used by the VS2022 lane

Deferred:

- broader MinGW variants
- packaging-ready build flows
- validation on every historical environment implied by the compatibility target band

The lane is concrete enough to be reviewable and extendable now without pretending complete cross-environment support.
