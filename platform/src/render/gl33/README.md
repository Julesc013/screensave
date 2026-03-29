# GL33 Placeholder Backend

Purpose: explicit placeholder for a later optional OpenGL 3.3 renderer tier.
Belongs here: bounded backend-private scaffolding that lets the runtime, diagnostics, and docs name the tier honestly before it exists.
Does not belong here: fake feature coverage, broad engine work, or silent fallback logic hidden from the shared runtime.

Current stage: the GL33 tier is only a placeholder.

- `gl33_backend.c` fails creation explicitly with a diagnosable placeholder reason
- `gl33_internal.h` keeps the placeholder create hook private to the renderer runtime

This directory exists so the renderer ladder can be explicit without pretending that an OpenGL 3.3 implementation is already real.
Type: runtime placeholder.
