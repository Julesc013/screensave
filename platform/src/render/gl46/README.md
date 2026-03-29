# GL46 Placeholder Backend

Purpose: explicit placeholder for a later optional OpenGL 4.6 renderer tier.
Belongs here: bounded backend-private scaffolding that lets the runtime, diagnostics, and docs name the tier honestly before it exists.
Does not belong here: fake feature coverage, broad engine work, or silent fallback logic hidden from the shared runtime.

Current stage: the GL46 tier is only a placeholder.

- `gl46_backend.c` fails creation explicitly with a diagnosable placeholder reason
- `gl46_internal.h` keeps the placeholder create hook private to the renderer runtime

This directory exists so the renderer ladder can be explicit without pretending that an OpenGL 4.6 implementation is already real.
Type: runtime placeholder.
