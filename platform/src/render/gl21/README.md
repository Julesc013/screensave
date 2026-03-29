# GL21 Backend

Purpose: backend-private OpenGL 2.1 compatibility-tier implementation for later-capability systems.
Belongs here: optional renderer code, advanced context creation, capability capture, primitive presentation, and backend-private state management.
Does not belong here: shared renderer policy, product code, or baseline fallback selection.

Current stage: Series 15 made the first capability-gated advanced GL tier real here, and the current repo now names that tier honestly as `gl21`:

- `gl21_backend.c` owns backend creation, frame setup, resize handling, and shutdown
- `gl21_context.c` owns explicit advanced-context probing, compatibility-profile fallback, and teardown
- `gl21_caps.c` captures vendor, renderer, version, and backend-private capability flags after context creation
- `gl21_present.c` owns buffer-swap presentation
- `gl21_primitives.c` provides the current rect, line, polyline, and bitmap path required by GL Gallery
- `gl21_state.c` keeps backend-private renderer-info and diagnostics helpers local

This backend stays strictly optional and capability-gated. The shared runtime may select it only when requested or when automatic selection proves it is supported; otherwise the runtime must fall back explicitly to GL11, GDI, or the internal null safety backend.
Type: runtime implementation.


