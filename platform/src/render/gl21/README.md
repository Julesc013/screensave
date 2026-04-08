# GL21 Backend

Purpose: backend-private OpenGL 2.1 advanced-lane implementation for later-capability systems.
Belongs here: optional renderer code, explicit advanced-context creation, private capability-bundle capture, bounded degradation details, primitive presentation, and backend-private state management.
Does not belong here: shared renderer policy, product code, or public modern-tier doctrine.

Current stage: Series 15 made the first capability-gated advanced GL tier real here, and `SX04` hardens it into the first real bounded advanced lane beneath the stable public contract:

- `gl21_backend.c` owns backend creation, frame setup, resize handling, and shutdown
- `gl21_context.c` owns explicit advanced-context probing, pixel-format inspection, partial-init cleanup, and auditable teardown
- `gl21_caps.c` captures vendor, renderer, version, and the private advanced-tier capability bundle instead of relying on scattered extension folklore
- `gl21_capture.c` keeps backend-detail text and lightweight proof diagnostics local to the advanced lane
- `gl21_present.c` owns swap-or-flush presentation plus present-count detail for diagnostics surfaces
- `gl21_primitives.c` provides the current rect, line, polyline, and bitmap path required by `gallery` and other advanced-capable proof surfaces
- `gl21_state.c` keeps backend-private renderer-info and diagnostics helpers local

This backend stays strictly optional and capability-gated. The shared runtime may select it only when requested or when automatic selection proves it is supported; otherwise the runtime must degrade explicitly through GL11, then GDI, and only then the internal null safety backend. A 3.x compatibility context may be used privately when it is the cleanest way to satisfy the bounded `gl21` contract, but that does not turn this subtree into a public `gl33` lane.
Type: runtime implementation.


