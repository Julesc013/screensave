# GDI Backend

Purpose: mandatory universal renderer floor.
Belongs here: GDI-specific rendering code required for baseline product support.
Does not belong here: assumptions that require OpenGL or newer-only graphics paths.
Current stage: Series 05 made the first reusable renderer backend real here, and `SX02` hardens it into the durable universal floor:

- `gdi_backend.c` creates and owns the backend instance
- `gdi_surface.c` owns non-destructive offscreen DIB-section backbuffer preparation and lifetime
- `gdi_present.c` owns target acquisition plus the `BitBlt` versus `StretchBlt` present path
- `gdi_primitives.c` provides the baseline rect/line/polyline path
- `gdi_bitmap.c` provides modest bitmap-view blit support with shared DIB metadata rules
- `gdi_capture.c` owns private capture signatures and backend-detail text for diagnostics surfaces
- `gdi_state.c` keeps renderer info, color, and shared bitmap-info helpers private

The design stays immediate and compatibility-first. Series 06 exercises this backend through the real Nocturne product, Series 07 exercises the same path through the BenchLab windowed harness, Series 08 keeps it as the explicit fallback floor when the optional GL11 backend is unavailable or not requested, Series 15 preserves it as the guaranteed floor beneath GL11, GL21, GL33, GL46, and the internal null safety fallback, and `SX02` centralizes the floor enough that later tiers can fall back here without treating GDI as disposable scaffolding.
Type: runtime implementation.
