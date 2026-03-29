# GL Plus Backend

Purpose: backend-private advanced GL implementation for later capability-gated systems.
Belongs here: optional renderer code, advanced context creation, capability capture, primitive presentation, and backend-private state management.
Does not belong here: shared renderer policy, product code, or baseline fallback selection.

Current stage: Series 15 makes the first capability-gated advanced GL tier real here:

- `glp_backend.c` owns backend creation, frame setup, resize handling, and shutdown
- `glp_context.c` owns explicit advanced-context probing, compatibility-profile fallback, and teardown
- `glp_caps.c` captures vendor, renderer, version, and backend-private capability flags after context creation
- `glp_present.c` owns buffer-swap presentation
- `glp_primitives.c` provides the current rect, line, polyline, and bitmap path required by GL Gallery
- `glp_state.c` keeps backend-private renderer-info and diagnostics helpers local

This backend stays strictly optional and capability-gated. The shared runtime may select it only when requested or when automatic selection proves it is supported; otherwise the runtime must fall back explicitly to GL11 or GDI.
Type: runtime implementation.
