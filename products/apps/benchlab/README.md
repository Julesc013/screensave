# BenchLab

Purpose: developer-facing diagnostics and validation harness for the current ScreenSave platform.
Belongs here: BenchLab app lifecycle, harness control, overlay diagnostics, app-local persistence, and smoke checks.
Does not belong here: the final player, gallery, packaging logic, or broad platform abstractions that only one app needs.

Series 07 makes BenchLab real:

- a real windowed harness for the current real saver module
- deterministic restart and reseed controls for repeatable debugging
- a modest diagnostics info panel with renderer, runtime, seed, and config summary data
- direct use of the shared saver/config/renderer contracts without going through the `.scr` host path

Series 08 extends the same app into the renderer-selection harness:

- explicit `auto`, `gdi`, and `gl11` selection controls
- honest requested-versus-active renderer reporting
- explicit fallback-reason reporting when GL11 cannot be used
- GL vendor, renderer, and version reporting when the optional backend is active

Series 09 extends the same app into the current multi-product harness:

- explicit saver selection for Nocturne, Ricochet, and Deepfield
- persistence of the selected saver key alongside renderer and deterministic-run preferences
- continued use of the shared saver dialogs and shared renderer path without turning BenchLab into the final player or gallery

Series 10 extends the same app to the current five-product harness:

- explicit saver selection for Nocturne, Ricochet, Deepfield, Ember, and Oscilloscope Dreams
- continued use of the shared saver dialogs and shared renderer path for both motion-family and framebuffer/vector-family validation
- preservation of deterministic restart/reseed value while keeping the UI modest and developer-facing

Series 11 extends the same app to the current seven-product harness:

- explicit saver selection for Nocturne, Ricochet, Deepfield, Ember, Oscilloscope Dreams, Pipeworks, and Lifeforms
- continued use of the shared saver dialogs and shared renderer path for motion-family, framebuffer/vector-family, and grid/simulation-family validation
- preservation of deterministic restart/reseed value while keeping the UI modest and developer-facing

BenchLab is intentionally narrow. It is a validation harness for developers, not the final end-user player.
Type: product runtime space.
