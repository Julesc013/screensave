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
Implementation currently exists through `S15` plus continuation `C00`, `C01`, `C02`, `C03`, `C04`, `C05`, `C06`, `C07`, `C08`, `C09`, and `C10`.
C00 reset the post-`S15` continuation roadmap, C01 applied the canonical saver/app rename and renderer-taxonomy normalization pass, C02 locked the post-rename migration baseline and recovery checkpoint, C03 converted the eighteen current savers into first-class standalone `.scr` product targets, C04 established the shared settings taxonomy, versioned preset/theme import-export files, within-saver session randomization, pack manifest discovery/validation helpers, and modest host plus BenchLab inspection foundations, C05 hardened the real Win32 `.scr` lifecycle across screen, preview, config, persistence, renderer fallback, and Windows-facing identity paths, C06 added the first portable bundle assembly path under `packaging/portable/` plus a real staged bundle and zip under `out/portable/` from the canonical saver outputs currently available in the local output roots, C07 added a current-user installer package under `packaging/installer/` plus a real staged installer and zip under `out/installer/` with opt-in saver selection assistance and conservative uninstall behavior, C08 added `anthology` as the real suite meta-saver with bounded in-process cross-saver rotation, weighted filtering, family/favorites selection, renderer-aware fallback, and BenchLab inspection support, C09 added `suite` as the real suite-level browser, launcher, embedded preview, detached run, and saver-settings control app with honest manifest-driven discovery and local artifact reporting, and C10 added the real contributor-facing SDK surface under `products/sdk/` plus the canonical `products/savers/_template/` starter saver with pack examples, manifest guidance, and contributor validation helpers.
The older short `S16` / `S17` endpoint is superseded by `docs/roadmap/post-s15-plan.md`.
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

- Plasma as the palette-and-framebuffer saver with restrained plasma, fire, and interference modes
- Phosphor as the phosphor-and-vector saver with curated Lissajous, harmonograph, and dense-trace modes
- a narrow shared visual-buffer helper plus explicit host and BenchLab wiring so all current savers continue to run through the same shared saver and renderer paths

Series 11 adds the Grid and Simulation Family:

- Pipeworks as the procedural network-growth saver with curated density, branch, rebuild, and theme presets
- Lifeforms as the contemplative cellular-automata saver with curated ruleset, reseed, density, and theme presets
- a narrow shared grid-buffer helper plus explicit host and BenchLab wiring so all current savers continue to run through the same shared saver and renderer paths

Series 12 adds the Systems and Ambient Family:

- Signals as the synthetic instrument-and-panel saver with curated scope, terminal, and telemetry presets
- Mechanize as the restrained kinetic-assembly saver with curated workshop, instrument, and machine-room presets
- Ecosystems as the ambient habitat-and-behavior saver with curated aquarium, aviary, and firefly presets
- explicit host and BenchLab wiring so all current savers continue to run through the same shared saver and renderer paths without adding a broad widget, animation, or ecology framework

Series 13 adds the Places and Atmosphere Family:

- Stormglass as the weather-on-glass saver with curated rain-pane, fogged-pane, and winter-pane presets
- Transit as the nocturnal infrastructure-and-route saver with curated motorway, rail-corridor, and harbor-night presets
- Observatory as the celestial exhibit saver with curated orrery, chart-room, and dome-watch presets
- explicit host and BenchLab wiring so all current savers continue to run through the same shared saver and renderer paths without adding a broad weather, route, astronomy, or scenic framework

Series 14 adds the Heavyweight Worlds Family:

- Vector as the abstract software-3D and wireframe flyover saver with curated structure-field, tunnel-flow, and terrain-line presets
- Explorer as the restrained raycast-and-autopilot traversal saver with curated corridor, industrial passage, and canyon-run presets
- City as the urban night-world scenic saver with curated skyline, harbor-edge, and rooftop-flyover presets
- Atlas as the curated fractal-voyage saver with atlas, voyage, and Julia-style presets plus restrained progressive refinement
- explicit host and BenchLab wiring so all current savers continue to run through the same shared saver and renderer paths without adding a broad 3D, map, city, or fractal framework

Series 15 adds the advanced GL capability path and Gallery:

- a real optional GL21 backend under `platform/src/render/gl21/` with explicit advanced-context probing, capability capture, and backend-private state ownership
- explicit versioned renderer selection and fallback among `gdi`, `gl11`, `gl21`, `gl33`, `gl46`, and the internal `null` safety path, with requested-versus-active reporting and explicit fallback reasons
- Gallery as the renderer-showcase saver with deliberate GDI, GL11, and GL21-tier scene variants plus curated compatibility, classic, and advanced presets
- explicit host and BenchLab wiring so the current saver set can be exercised honestly through the tiered renderer stack without turning the repo into the final meta-gallery

The next continuation phase is `C11` for backlog ingestion and routing of new ideas.

## Planned Product Shape

ScreenSave is structured as one shared platform with many releasable products:

- Individual savers under `products/savers/`
- Suite-level applications such as `suite` and `benchlab` under `products/apps/`
- SDK material under `products/sdk/`

`nocturne`, `ricochet`, `deepfield`, `plasma`, `phosphor`, `pipeworks`, `lifeforms`, `signals`, `mechanize`, `ecosystems`, `stormglass`, `transit`, `observatory`, `vector`, `explorer`, `city`, `atlas`, `gallery`, and `anthology` are now the real saver products in the tree.
`anthology` is the real suite meta-saver `.scr` and orchestrates the other eighteen savers in-process without replacing them as first-class products.
`benchlab` and `suite` are now the real non-saver apps in the tree.
`suite` is now the canonical suite-level browser, launcher, preview, and configuration app under `products/apps/suite/`.
`products/apps/player/` remains only as a superseded legacy stub and must not grow into a separate public app.
The saver line now also shares a versioned settings, preset, theme, randomization, and pack contract while keeping product-local meaning and content in the owning saver tree.
`products/sdk/` and `products/savers/_template/` now provide the real contributor-facing authoring surface for new saver products and data-driven packs.
Canonical post-`S15` saver and app naming plus migration aliases are tracked in `docs/roadmap/rename-map.md`.
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

The checked-in build scaffold now integrates the real Series 15 multi-family saver-plus-harness path on top of the Series 05 GDI renderer, the Series 08 optional GL11 renderer, the Series 15 optional GL21 renderer plus explicit GL33 and GL46 placeholders, the Series 04 shared core runtime, and the Series 03 host:

- a concrete MSVC VS2022 solution lane under `build/msvc/vs2022/`
- a concrete MinGW i686 make lane under `build/mingw/i686/`
- documentation-only VS6 and VS2008 lanes that preserve long-term intent without claiming present build completeness

The shared platform target now compiles the reusable non-renderer core runtime, the mandatory GDI backend, the optional GL11 backend, the optional capability-gated GL21 backend, the explicit GL33 and GL46 placeholders, the internal null safety backend, the Win32 host, the narrow shared visual-buffer helper used by the current framebuffer and vector products, and the narrow shared grid-buffer helper used by the current grid and simulation products.
The saver line now builds toward one true `.scr` output per saver across Nocturne, Ricochet, Deepfield, Plasma, Phosphor, Pipeworks, Lifeforms, Signals, Mechanize, Ecosystems, Stormglass, Transit, Observatory, Vector, Explorer, City, Atlas, Gallery, and Anthology, each with product-specific entry binding and version/resource identity on top of the shared host and renderer stack.
`anthology.scr` stays explicit in the build graph as the suite meta-saver target that links the existing eighteen inner saver modules in-process rather than launching external `.scr` processes.
The Win32 host path now also carries the hardened C05 lifecycle baseline: bounded single-saver `Settings...` shell routing, preview-parent revalidation, virtual-desktop fullscreen sizing, display-change resynchronization, and explicit multi-monitor limits that keep one saver window spanning the current virtual desktop.
The app targets now include BenchLab as the real windowed diagnostics harness for the current nineteen saver products and Suite as the bounded suite-level browser, preview, detached-run, and saver-settings surface for the same line. Both stay separate from the distributed `.scr` products and from each other.
Portable bundle sources now live under `packaging/portable/`, and the current `C06` staged portable output lives under `out/portable/screensave-portable-c06/` with a matching zip beside it. Installed-distribution sources now also live under `packaging/installer/`, and the current `C07` staged installer output lives under `out/installer/screensave-installer-c07/` with a matching zip beside it. Both delivery paths remain intentionally truthful and partial: they were staged before `anthology` and `suite` existed, they therefore do not yet include `anthology.scr` or `suite.exe`, they exclude BenchLab, and they keep machine-wide install plus later SDK work deferred beyond `C09`.

Read the specs before adding code.
This repository should stay truthful, reconstructable, and conservative about compatibility.
