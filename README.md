# ScreenSave

ScreenSave is a monorepo for a retro-compatible Windows screensaver platform and a suite of products built on top of it.

The platform target is intentionally conservative:

- C89 is the language baseline.
- x86 Win32 `.scr` binaries are the baseline product format.
- Windows 95 / 98 / ME and NT 4.0 through Windows 11 are the support band to preserve.
- The host model is the classic ANSI Win32 screensaver path.
- GDI is mandatory.
- OpenGL 1.1 is optional and must be capability-detected at runtime.
- Any later accelerated backend must remain optional and must not weaken the universal baseline.

## Current Status

The repository is in Series 15.
Series 00 established repository law, structure, roadmap, and agent guidance.
Series 01 added contribution, review, governance, and CI foundations.
Series 02 added the checked-in build scaffold and toolchain lane structure.
Series 03 added the first real Win32 `.scr` host skeleton.
Series 04 added the first shared runtime substrate and narrow public platform API.
Series 05 added the mandatory reusable GDI renderer backend.
Series 06 added Nocturne as the first real saver product:

- a real Nocturne saver module with product-owned config, presets, and themes
- restrained dark-room-safe rendering through the shared GDI backend
- real config dialog wiring and long-run fade/reseed behavior on the shared host path

Series 07 adds BenchLab as the first real developer-facing app product:

- a real windowed harness for the active saver module outside the `.scr` host path
- deterministic restart and reseed controls for repeatable debugging
- a modest runtime and renderer diagnostics overlay for validating Nocturne and the GDI baseline

Series 08 adds the optional OpenGL 1.1 backend without weakening the baseline:

- a conservative WGL/OpenGL 1.1 backend under the shared renderer contract
- automatic renderer selection with explicit fallback to GDI
- BenchLab controls for `auto`, `gdi`, and `gl11`, with active-renderer and fallback reporting
- Nocturne validation on both the mandatory GDI floor and the optional GL11 path

Series 09 adds the first true multi-product saver family:

- Ricochet as the polished bounce-motion saver with curated object, trail, and theme presets
- Deepfield as the atmospheric parallax and fly-through saver with restrained density, camera, and pulse presets
- narrow host and BenchLab product selection so Nocturne, Ricochet, and Deepfield can all be exercised honestly through the shared saver and renderer paths

Series 10 adds the Framebuffer and Vector Family:

- Ember as the palette-and-framebuffer saver with restrained plasma, fire, and interference modes
- Oscilloscope Dreams as the phosphor-and-vector saver with curated Lissajous, harmonograph, and dense-trace modes
- a narrow shared visual-buffer helper plus explicit host and BenchLab wiring so all current savers continue to run through the same shared saver and renderer paths

Series 11 adds the Grid and Simulation Family:

- Pipeworks as the procedural network-growth saver with curated density, branch, rebuild, and theme presets
- Lifeforms as the contemplative cellular-automata saver with curated ruleset, reseed, density, and theme presets
- a narrow shared grid-buffer helper plus explicit host and BenchLab wiring so all current savers continue to run through the same shared saver and renderer paths

Series 12 adds the Systems and Ambient Family:

- Signal Lab as the synthetic instrument-and-panel saver with curated scope, terminal, and telemetry presets
- Mechanical Dreams as the restrained kinetic-assembly saver with curated workshop, instrument, and machine-room presets
- Ecosystems as the ambient habitat-and-behavior saver with curated aquarium, aviary, and firefly presets
- explicit host and BenchLab wiring so all current savers continue to run through the same shared saver and renderer paths without adding a broad widget, animation, or ecology framework

Series 13 adds the Places and Atmosphere Family:

- Stormglass as the weather-on-glass saver with curated rain-pane, fogged-pane, and winter-pane presets
- Night Transit as the nocturnal infrastructure-and-route saver with curated motorway, rail-corridor, and harbor-night presets
- Observatory as the celestial exhibit saver with curated orrery, chart-room, and dome-watch presets
- explicit host and BenchLab wiring so all current savers continue to run through the same shared saver and renderer paths without adding a broad weather, route, astronomy, or scenic framework

Series 14 adds the Heavyweight Worlds Family:

- Vector Worlds as the abstract software-3D and wireframe flyover saver with curated structure-field, tunnel-flow, and terrain-line presets
- Retro Explorer as the restrained raycast-and-autopilot traversal saver with curated corridor, industrial passage, and canyon-run presets
- City Nocturne as the urban night-world scenic saver with curated skyline, harbor-edge, and rooftop-flyover presets
- Fractal Atlas as the curated fractal-voyage saver with atlas, voyage, and Julia-style presets plus restrained progressive refinement
- explicit host and BenchLab wiring so all current savers continue to run through the same shared saver and renderer paths without adding a broad 3D, map, city, or fractal framework

Series 15 adds the advanced GL capability path and GL Gallery:

- a real optional GL-plus backend under `platform/src/render/gl_plus/` with explicit advanced-context probing, capability capture, and backend-private state ownership
- explicit three-tier renderer selection and fallback among `gdi`, `gl11`, and `gl_plus`, with requested-versus-active reporting and explicit fallback reasons
- GL Gallery as the renderer-showcase saver with deliberate GDI, GL11, and GL-plus scene variants plus curated compatibility, classic, and advanced presets
- explicit host and BenchLab wiring so the current saver set can be exercised honestly through the tiered renderer stack without turning the repo into the final meta-gallery

The final suite gallery/meta-product and packaging remain deferred.

## Planned Product Shape

ScreenSave is structured as one shared platform with many releasable products:

- Individual savers under `products/savers/`
- Suite-level applications such as Gallery, Player, and BenchLab under `products/apps/`
- SDK material under `products/sdk/`

`nocturne`, `ricochet`, `deepfield`, `ember`, `oscilloscope_dreams`, `pipeworks`, `lifeforms`, `signal_lab`, `mechanical_dreams`, `ecosystems`, `stormglass`, `night_transit`, `observatory`, `vector_worlds`, `retro_explorer`, `city_nocturne`, `fractal_atlas`, and `gl_gallery` are now the real saver products in the tree.
`benchlab` is now the first real non-saver app in the tree.
Additional saver families and suite-level products are described in `docs/roadmap/products-and-lineup.md`.

## Repository Authority

For normative project law, start with:

- `specs/compatibility.md`
- `specs/saver_api.md`
- `specs/renderer_contract.md`
- `specs/build_targets.md`
- `specs/release_contract.md`
- `specs/config_schema.md`

For explanatory architecture and execution order, start with:

- `docs/architecture/overview.md`
- `docs/architecture/layering.md`
- `docs/architecture/repo-structure.md`
- `docs/roadmap/prompt-program.md`
- `docs/roadmap/series-map.md`

## Directory Summary

- `.codex/` contains project-scoped Codex configuration and custom agent definitions.
- `.github/` contains community-health files, ownership metadata, and stage-appropriate workflows.
- `platform/` is the shared runtime and public API area.
- `products/` is the product tree for savers, applications, and SDK material.
- `assets/` is for shared non-code content and license records.
- `tests/` and `validation/` hold verification logic and evidence.
- `tools/`, `build/`, and `packaging/` hold support material, not baseline runtime dependencies.

## Build Status

The checked-in build scaffold now integrates the real Series 15 multi-family saver-plus-harness path on top of the Series 05 GDI renderer, the Series 08 optional GL11 renderer, the Series 15 optional GL-plus renderer, the Series 04 shared core runtime, and the Series 03 host:

- a concrete MSVC VS2022 solution lane under `build/msvc/vs2022/`
- a concrete MinGW i686 make lane under `build/mingw/i686/`
- documentation-only VS6 and VS2008 lanes that preserve long-term intent without claiming present build completeness

The shared platform target now compiles the reusable non-renderer core runtime, the mandatory GDI backend, the optional GL11 backend, the optional capability-gated GL-plus backend, the Win32 host, the narrow shared visual-buffer helper used by the current framebuffer and vector products, and the narrow shared grid-buffer helper used by the current grid and simulation products.
The `.scr` targets now run Nocturne, Ricochet, Deepfield, Ember, Oscilloscope Dreams, Pipeworks, Lifeforms, Signal Lab, Mechanical Dreams, Ecosystems, Stormglass, Night Transit, Observatory, Vector Worlds, Retro Explorer, City Nocturne, Fractal Atlas, and GL Gallery through the shared saver path with explicit tiered renderer selection and honest fallback.
The app target now runs BenchLab as a real windowed harness for all eighteen current savers through the same shared contracts, with explicit `auto`, `gdi`, `gl11`, and `gl_plus` controls plus requested-versus-active renderer diagnostics.

Read the specs before adding code.
This repository should stay truthful, reconstructable, and conservative about compatibility.
