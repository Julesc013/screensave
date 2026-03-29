# GL11 Backend

Purpose: optional conservative accelerated backend.
Belongs here: Win32/WGL/OpenGL 1.1 renderer code that cleanly maps to the shared renderer contract.
Does not belong here: modern-context creation, extension-required paths, shaders, or anything that turns GL11 into the baseline.

Current stage: Series 08 makes the optional universal GL11 path real here:

- `gl11_backend.c` owns backend creation, frame setup, resize handling, and shutdown
- `gl11_context.c` owns conservative pixel-format choice, WGL context creation, and explicit teardown
- `gl11_caps.c` captures the GL vendor, renderer, and version strings
- `gl11_present.c` owns buffer-swap or flush presentation
- `gl11_primitives.c` provides the current shared rect/line/polyline path
- `gl11_bitmap.c` provides the modest bitmap-view upload path required by the shared contract
- `gl11_state.c` keeps backend-private renderer-info and diagnostics helpers local

This backend is optional. The runtime selection path must fall back explicitly to GDI whenever GL11 cannot be initialized honestly, and Series 15 keeps it as the conservative universal accelerated tier beneath the optional higher-capability GL21 tier and any later explicit GL placeholders.
Type: runtime implementation.
