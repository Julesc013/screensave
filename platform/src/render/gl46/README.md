# GL46 Premium Backend

Purpose: bounded optional OpenGL 4.6 premium renderer tier.
Belongs here: premium backend-private context creation, function loading, capability capture, shader-backed drawing, present detail, and degradable premium-lane behavior beneath the shared substrate.
Does not belong here: public API changes, product-specific scene logic, non-GL backend work, or premium-only assumptions leaking into the baseline tiers.

Current stage: `SX06` makes GL46 the real premium lane.

- `gl46_backend.c` owns renderer creation, dispatch, and bounded frame-state setup for the premium lane
- `gl46_context.c` owns 4.6 compatibility-context creation, bootstrap handling, and auditable teardown
- `gl46_caps.c` owns the private premium capability bundle, required function loading, and missing-capability reporting
- `gl46_primitives.c` owns shader-backed primitive and bitmap rendering with premium-only texture-storage and sampler-object support
- `gl46_present.c` and `gl46_capture.c` keep present-path status and backend-detail text private to the renderer runtime

The premium lane stays optional and capability-gated.
When it cannot run honestly, the private substrate must degrade explicitly through `gl33`, `gl21`, `gl11`, and `gdi`.
Type: runtime implementation.
