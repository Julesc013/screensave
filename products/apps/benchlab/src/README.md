# BenchLab Source

Purpose: BenchLab app-local implementation for the Series 09 diagnostics harness.
Belongs here: app lifecycle, harness control, app config persistence, diagnostics buffering, current saver selection, renderer selection, and info-panel rendering.
Does not belong here: shared platform runtime or product-specific saver internals beyond the narrow active-module binding.

Key files:

- `benchlab_main.c` provides the real app entry point
- `benchlab_app.c` owns the window lifecycle, menu commands, and message loop
- `benchlab_session.c` owns saver binding, restart/reseed control, and shared renderer usage
- `benchlab_diag.c` captures shared diagnostics into a small app-local ring buffer
- `benchlab_overlay.c` renders the developer-facing info panel
- `benchlab_config.c` owns app-local persistence and command-line overrides

Type: product runtime source.
