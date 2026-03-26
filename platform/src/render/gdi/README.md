# GDI Backend

Purpose: mandatory universal renderer floor.
Belongs here: GDI-specific rendering code required for baseline product support.
Does not belong here: assumptions that require OpenGL or newer-only graphics paths.
Current stage: Series 05 makes the first reusable renderer backend real here:

- `gdi_backend.c` creates and owns the backend instance
- `gdi_surface.c` owns the offscreen DIB-section backbuffer
- `gdi_present.c` presents the offscreen buffer to the target window
- `gdi_primitives.c` provides the baseline rect/line/polyline path
- `gdi_bitmap.c` provides modest bitmap-view blit support
- `gdi_state.c` keeps backend diagnostics/info helpers private

The design stays immediate and compatibility-first. Series 06 exercises this backend through the real Nocturne product, and Series 07 exercises the same path through the BenchLab windowed harness while OpenGL remains deferred.
Type: runtime implementation.
