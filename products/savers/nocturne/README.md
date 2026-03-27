# Nocturne

Purpose: the first real ScreenSave product and the dark-room reference saver.
Belongs here: Nocturne-specific runtime code, presets, themes, manifest data, and smoke checks.
Does not belong here: shared host, renderer, or cross-product framework code.

Series 06 makes Nocturne real:

- a real saver module lifecycle on the shared host/runtime contract
- restrained dark rendering through the shared GDI backend
- curated presets and monochrome dark themes
- product-owned config persistence and config dialog wiring
- long-run fade and reseed behavior for calm but non-dead motion
- shared-contract integration that now allows BenchLab to run Nocturne in a windowed diagnostics harness without private host shortcuts

Series 08 keeps Nocturne renderer-agnostic while validating the same product through both the mandatory GDI backend and the optional GL11 backend.
Series 09 keeps Nocturne as a first-class built-in saver while the host and BenchLab add narrow selection support for Nocturne, Ricochet, and Deepfield.

Nocturne stays intentionally narrow. It is a dark blanker-plus saver, not a general ambient effects engine.
Type: product runtime space.
