# BenchLab

Purpose: developer-facing diagnostics and validation harness for the current ScreenSave platform.
Belongs here: BenchLab app lifecycle, harness control, overlay diagnostics, app-local persistence, and smoke checks.
Does not belong here: the real `suite` app, the `gallery` saver, packaging logic, or broad platform abstractions that only one app needs.

Series 07 makes BenchLab real:

- a real windowed harness for the current real saver module
- deterministic restart and reseed controls for repeatable debugging
- a modest diagnostics info panel with renderer, runtime, seed, and config summary data
- direct use of the shared saver/config/renderer contracts without going through the `.scr` host path

Series 08 extends the same app into the renderer-selection harness:

- explicit `auto`, `gdi`, and `gl11` selection controls
- honest requested-versus-active renderer reporting
- explicit fallback-reason reporting when GL11 cannot be used
- backend vendor, renderer, and detail reporting through the shared renderer-info surface

Series 09 extends the same app into the current multi-product harness:

- explicit saver selection for Nocturne, Ricochet, and Deepfield
- persistence of the selected saver key alongside renderer and deterministic-run preferences
- continued use of the shared saver dialogs and shared renderer path without turning BenchLab into the real `suite` app or the `gallery` saver

Series 10 extends the same app to the current five-product harness:

- explicit saver selection for Nocturne, Ricochet, Deepfield, Plasma, and Phosphor
- continued use of the shared saver dialogs and shared renderer path for both motion-family and framebuffer/vector-family validation
- preservation of deterministic restart/reseed value while keeping the UI modest and developer-facing

Series 11 extends the same app to the current seven-product harness:

- explicit saver selection for Nocturne, Ricochet, Deepfield, Plasma, Phosphor, Pipeworks, and Lifeforms
- continued use of the shared saver dialogs and shared renderer path for motion-family, framebuffer/vector-family, and grid/simulation-family validation
- preservation of deterministic restart/reseed value while keeping the UI modest and developer-facing

Series 12 extends the same app to the current ten-product harness:

- explicit saver selection for Nocturne, Ricochet, Deepfield, Plasma, Phosphor, Pipeworks, Lifeforms, Signals, Mechanize, and Ecosystems
- continued use of the shared saver dialogs and shared renderer path for motion-family, framebuffer/vector-family, grid/simulation-family, and systems/ambient-family validation
- preservation of deterministic restart/reseed value while keeping the UI modest and developer-facing

Series 13 extends the same app to the current thirteen-product harness:

- explicit saver selection for Nocturne, Ricochet, Deepfield, Plasma, Phosphor, Pipeworks, Lifeforms, Signals, Mechanize, Ecosystems, Stormglass, Transit, and Observatory
- continued use of the shared saver dialogs and shared renderer path for motion-family, framebuffer/vector-family, grid/simulation-family, systems/ambient-family, and places/atmosphere-family validation
- preservation of deterministic restart/reseed value while keeping the UI modest and developer-facing

Series 14 extends the same app to the current seventeen-product harness:

- explicit saver selection for Nocturne, Ricochet, Deepfield, Plasma, Phosphor, Pipeworks, Lifeforms, Signals, Mechanize, Ecosystems, Stormglass, Transit, Observatory, Vector, Explorer, City, and Atlas
- continued use of the shared saver dialogs and shared renderer path for heavyweight software-3D, traversal, urban-scenic, and fractal-family validation alongside the earlier families
- preservation of deterministic restart/reseed value while keeping the UI modest and developer-facing

Series 15 extends the same app into the current eighteen-product and versioned renderer-tier harness:

- explicit saver selection for Nocturne, Ricochet, Deepfield, Plasma, Phosphor, Pipeworks, Lifeforms, Signals, Mechanize, Ecosystems, Stormglass, Transit, Observatory, Vector, Explorer, City, Atlas, and Gallery
- explicit `auto`, `gdi`, `gl11`, `gl21`, `gl33`, and `gl46` request paths with honest requested-versus-active renderer reporting
- explicit fallback-reason reporting plus backend vendor, renderer, and detail reporting when renderer introspection is available
- continued use of the shared saver dialogs and shared renderer path without turning BenchLab into the real `suite` app or a suite-level meta-product

Continuation `C04` extends the same app into the shared-settings inspection harness:

- per-session resolved config alongside stored config so within-saver randomization can be inspected honestly
- overlay reporting for active preset, theme, detail, and randomization state
- continued use of shared saver dialogs and shared settings import/export helpers without turning BenchLab into the final end-user `suite` app

Continuation `C08` extends the same app into the suite meta-saver inspection harness:

- explicit saver selection now includes `anthology` beside the existing standalone saver line
- the overlay surfaces anthology's current inner saver, previous saver, selection reason, family filter summary, and renderer-aware fallback context
- BenchLab remains diagnostics-only and does not become the final suite browser or control app

Continuation `C12` keeps BenchLab diagnostics-focused while normalizing its shared outer vocabulary:

- `Renderer preference`, `Selection path`, `Fallback cause`, and `Renderer status` are now the canonical renderer diagnostics labels
- `Detail level`, `Randomization mode`, `Preset key`, and `Theme key` distinguish diagnostics identity from end-user display names

`SX09` closes the substrate series by keeping BenchLab diagnostics-only while making it a better bounded proof harness:

- overlay and report output now surface requested renderer, saver routing profile, policy target, selected band, degraded path, fallback cause, and backend detail together
- deterministic `/report` captures can record a bounded multi-frame run without turning BenchLab into the real product shell
- hidden report runs now keep the GDI floor honest by presenting through an offscreen compatible target instead of depending on a visible child-window blit

BenchLab is intentionally narrow. It is a validation harness for developers, not the end-user `suite` app.
C03 leaves that boundary intact: the standalone saver `.scr` line now ships as one saver binary per product, while BenchLab continues to run the saver modules directly as a diagnostics harness instead of becoming a launcher for distributed saver binaries.
Continuation `C09` keeps that boundary intact: Suite now owns the bounded end-user browse, preview, launch, and saver-settings surface, while BenchLab remains the renderer, runtime, and deterministic diagnostics harness.
Type: product runtime space.
