# BenchLab

Purpose: developer-facing diagnostics and validation harness for the current ScreenSave platform.
Belongs here: BenchLab app lifecycle, harness control, overlay diagnostics, app-local persistence, and smoke checks.
Does not belong here: the final player, gallery, packaging logic, or broad platform abstractions that only one app needs.

Series 07 makes BenchLab real:

- a real windowed harness for the current built-in saver module
- deterministic restart and reseed controls for repeatable debugging
- a modest diagnostics info panel with renderer, runtime, seed, and config summary data
- direct use of the shared saver/config/renderer contracts without going through the `.scr` host path

Series 08 extends the same app into the renderer-selection harness:

- explicit `auto`, `gdi`, and `gl11` selection controls
- honest requested-versus-active renderer reporting
- explicit fallback-reason reporting when GL11 cannot be used
- GL vendor, renderer, and version reporting when the optional backend is active

BenchLab is intentionally narrow. It is a validation harness for developers, not the final end-user player.
Type: product runtime space.
