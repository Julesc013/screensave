# GL33 Modern Backend

Purpose: the first real bounded OpenGL 3.3 modern renderer tier.
Belongs here: private modern-context lifecycle, dynamic function loading, modern-capability capture, shader-backed primitives, bounded modern present behavior, and modern-lane diagnostics detail.
Does not belong here: public API expansion, GL46-era assumptions, broad engine work, or hidden degradation logic.

Current stage: `SX05` makes the GL33 tier real while keeping it optional, capability-gated, and bounded.

- `gl33_context.c` owns compatibility-profile modern-context creation, teardown, and partial-init cleanup
- `gl33_caps.c` owns the private modern capability bundle and required-function capture
- `gl33_primitives.c` owns the bounded shader-backed primitive and bitmap path used by the modern lane
- `gl33_present.c` and `gl33_capture.c` keep present detail and modern-lane proof text private to the backend
- `gl33_backend.c` integrates the lane with the existing renderer contract without widening public product headers

This directory exists so the renderer ladder can expose a real modern tier without weakening the lower-tier fallback story.
Type: runtime implementation.
