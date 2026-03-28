# Changelog

All notable repository changes are recorded here in series order.

## S11 - 2026-03-28

### Added

- Pipeworks under `products/savers/pipeworks/` as the Grid and Simulation Family network-growth product, including real growth, pulse, rebuild, config, presets, themes, a manifest, and lightweight smoke validation.
- Lifeforms under `products/savers/lifeforms/` as the Grid and Simulation Family cellular product, including real Conway and HighLife evolution, reseed handling, config, presets, themes, a manifest, and lightweight smoke validation.
- A narrow shared byte-grid helper under `platform/src/core/grid/` and `platform/include/screensave/grid_buffer_api.h` so the current stateful grid products can share small occupancy and double-buffered state storage without introducing a broad simulation framework.

### Changed

- Updated the built-in saver wiring so the current `.scr` products can default to Nocturne, Ricochet, Deepfield, Ember, Oscilloscope Dreams, Pipeworks, or Lifeforms while preserving the shared saver/module contract, explicit product identity, and automatic renderer-selection behavior.
- Updated BenchLab so the diagnostics harness can select and inspect Nocturne, Ricochet, Deepfield, Ember, Oscilloscope Dreams, Pipeworks, and Lifeforms through the shared saver and renderer paths while keeping the UI modest and developer-facing.
- Updated the concrete VS2022 and MinGW i686 build lanes, static build checks, product manifests, and repo docs so Series 11 truthfully describes the new grid/simulation family and the widened seven-product harness.

### Validation

- Confirmed Series 00 through Series 10 prerequisites existed before changes.
- Built `build/msvc/vs2022/ScreenSave.sln` for `Debug|Win32` and produced the shared platform library plus `nocturne.scr`, `ricochet.scr`, `deepfield.scr`, `ember.scr`, `oscilloscope_dreams.scr`, `pipeworks.scr`, `lifeforms.scr`, and `benchlab.exe`.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, and `python tools/scripts/check_build_layout.py`; all passed.
- Compiled and ran smoke executables for Nocturne, Ricochet, Deepfield, Ember, Oscilloscope Dreams, Pipeworks, Lifeforms, and BenchLab against the built Win32 debug objects and shared platform library; all returned success.
- Confirmed no GL-plus or modern-context work, next-family work, gallery/player scope, packaging logic, or unsupported compatibility claims were added in this series.

## S10 - 2026-03-28

### Added

- Ember under `products/savers/ember/` as the Framebuffer and Vector Family framebuffer product, including real plasma, fire, and interference modes, product-owned config/presets/themes, a manifest, and lightweight smoke validation.
- Oscilloscope Dreams under `products/savers/oscilloscope_dreams/` as the Framebuffer and Vector Family vector product, including real Lissajous, harmonograph, and dense-trace modes, product-owned config/presets/themes, a manifest, and lightweight smoke validation.
- A narrow shared visual-buffer helper under `platform/src/core/visual/` and `platform/include/screensave/visual_buffer_api.h` so the current framebuffer and persistence-oriented products can share bitmap-state operations without introducing a broad effects or vector framework.

### Changed

- Updated the built-in saver wiring so the current `.scr` products can default to Nocturne, Ricochet, Deepfield, Ember, or Oscilloscope Dreams while preserving the shared saver/module contract, explicit product identity, and automatic renderer-selection behavior.
- Updated BenchLab so the diagnostics harness can select and inspect Nocturne, Ricochet, Deepfield, Ember, and Oscilloscope Dreams through the shared saver and renderer paths while keeping the UI modest and developer-facing.
- Updated the concrete VS2022 and MinGW i686 build lanes, static build checks, product manifests, and repo docs so Series 10 truthfully describes the new framebuffer/vector family and the widened five-product harness.

### Validation

- Confirmed Series 00 through Series 09 prerequisites existed before changes.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, and `python tools/scripts/check_build_layout.py`; all passed.
- Built `build/msvc/vs2022/ScreenSave.sln` for `Debug|Win32` and produced the shared platform library plus `nocturne.scr`, `ricochet.scr`, `deepfield.scr`, `ember.scr`, `oscilloscope_dreams.scr`, and `benchlab.exe`.
- Compiled and ran smoke executables for Nocturne, Ricochet, Deepfield, Ember, Oscilloscope Dreams, and BenchLab against the built Win32 debug objects and shared platform library; all returned success.
- Confirmed no GL-plus or modern-context work, next-family work, gallery/player scope, packaging logic, or unsupported compatibility claims were added in this series.

## S09 - 2026-03-27

### Added

- Ricochet under `products/savers/ricochet/` as the first Motion Family saver, including a real saver module, curated presets/themes, product-owned config dialog, manifest, and lightweight smoke validation.
- Deepfield under `products/savers/deepfield/` as the second Motion Family saver, including a real saver module, curated presets/themes, product-owned config dialog, manifest, and lightweight smoke validation.
- A narrow private built-in saver registry helper so the current stage can resolve multiple real saver products without introducing a plugin loader or broad discovery system.

### Changed

- Updated the Win32 `.scr` host to support modest built-in saver selection, persist the selected product key, and delegate configuration to the selected real saver product while preserving the existing host lifecycle and automatic renderer-selection behavior.
- Updated BenchLab so the diagnostics harness can select and run Nocturne, Ricochet, or Deepfield through the shared saver and renderer contracts while keeping deterministic restart/reseed and renderer diagnostics intact.
- Updated the concrete VS2022 and MinGW i686 build lanes, static build checks, manifests, and repo docs so Series 09 truthfully describes the first multi-product saver family and current multi-product wiring.
- Tightened the Win32 build path with narrow C89/MSVC-safe preset/theme lookup fixes, a small shared RNG header fix, and unique Ricochet resource IDs so the bundled multi-product targets build and link cleanly.

### Validation

- Confirmed Series 00 through Series 08 prerequisites existed before changes.
- Built `build/msvc/vs2022/ScreenSave.sln` for `Debug|Win32` and produced the shared platform library plus `nocturne.scr`, `ricochet.scr`, `deepfield.scr`, and `benchlab.exe`.
- Ran the existing Nocturne, Ricochet, Deepfield, and BenchLab smoke harnesses against the built debug artifacts and confirmed module validity, preset/theme presence, and multi-product selection wiring.
- Confirmed no GL-plus or modern-context work, next-family work, gallery/player scope, packaging logic, or unsupported compatibility claims were added in this series.

## S08 - 2026-03-27

### Added

- A real optional OpenGL 1.1 backend under `platform/src/render/gl11/`, including conservative WGL context creation, capability capture, present handling, and the current baseline primitive/bitmap path required by the shared renderer contract.
- Explicit runtime renderer selection and fallback reporting so sessions can request `auto`, `gdi`, or `gl11` while keeping GDI as the guaranteed floor.

### Changed

- Updated the shared renderer dispatch and renderer-info model narrowly so requested renderer, active renderer, fallback reason, and GL vendor/renderer/version can be reported honestly without introducing GL-plus or modern-context assumptions.
- Updated the Win32 `.scr` host to use automatic shared-renderer selection while keeping the host lifecycle narrow and preserving GDI as the fallback baseline.
- Updated BenchLab so developers can select the requested renderer explicitly, inspect requested-versus-active backend state, and validate Nocturne across both renderer paths without adding player or gallery scope.
- Updated the concrete VS2022 and MinGW i686 build lanes, static build checks, manifests, and repo docs so Series 08 truthfully describes the optional GL11 backend and dual-backend validation state.

### Validation

- Confirmed Series 00 through Series 07 prerequisites existed before changes.
- Performed static validation of the GL11 backend file graph, renderer-selection wiring, build integration, and documentation consistency.
- Confirmed no GL-plus or modern-context work, second-saver work, gallery/player scope, packaging logic, or unsupported compatibility claims were added in this series.

## S07 - 2026-03-27

### Added

- BenchLab as the first real app product under `products/apps/benchlab/`, including a manifest, windowed harness sources, overlay diagnostics, app-local persistence, and lightweight smoke validation.
- A deterministic BenchLab run mode with restart, reseed, pause, and single-step controls for repeatable Nocturne debugging on the shared GDI baseline.

### Changed

- Refined the shared saver and renderer contracts narrowly so both the `.scr` host and BenchLab can allocate saver-owned config state and create window-bound renderers through the public platform surface.
- Updated the concrete VS2022 and MinGW i686 build lanes, static build checks, and repo docs so BenchLab is a first-class checked-in app target alongside `nocturne.scr`.
- Extended shared diagnostics usage modestly so BenchLab can report app-local status while continuing to run Nocturne through the shared saver/module and renderer paths.

### Validation

- Confirmed Series 00 through Series 06 prerequisites existed before changes.
- Performed static validation of the BenchLab file graph, shared-contract wiring, build integration, and documentation consistency.
- Confirmed no OpenGL backend work, second-saver work, gallery/player scope, packaging logic, or release-production claims were added in this series.

## S06 - 2026-03-27

### Added

- Nocturne as the first real saver product under `products/savers/nocturne/`, including a real saver module, curated presets/themes, a manifest, and lightweight smoke validation.
- Product-owned Nocturne configuration persistence and a real Nocturne configuration dialog resource wired through the shared saver/config contract.

### Changed

- Extended the saver/module contract narrowly so the active saver can own its product config size, defaults, clamp/load/save hooks, and configuration dialog without pushing product logic into the host.
- Updated the Win32 host to allocate, load, clamp, save, and edit active-product settings through the shared saver contract while keeping the validation scene as a fallback only when no real saver session exists.
- Replaced the no-op `nocturne` shell with a real restrained dark-room product that renders through the shared GDI backend with fade-in, fade-out, deterministic seed support, and anti-stagnation reseed behavior.
- Updated the concrete VS2022 and MinGW i686 build lanes, static build checks, and repository docs so Series 06 truthfully describes Nocturne as the first real product.

### Validation

- Confirmed Series 00 through Series 05 prerequisites existed before changes.
- Performed static validation of the Nocturne product file graph, shared-contract wiring, host/build integration, and documentation consistency.
- Confirmed no OpenGL backend work, second-product work, gallery/randomizer behavior, packaging logic, or release-production claims were added in this series.

## S05 - 2026-03-27

### Added

- A real reusable GDI backend under `platform/src/render/gdi/` with an offscreen DIB-section backbuffer, present path, baseline rect/line/polyline primitives, and modest bitmap-view blit support.
- A tiny renderer-driven validation scene in the host used only when no real saver session exists yet.

### Changed

- Updated the Win32 host skeleton to create and use the shared GDI renderer for both screen and preview paths instead of the earlier host-local placeholder scene.
- Updated the host lifecycle to recognize a real renderer-backed session path while keeping `nocturne` as a no-op saver-module shell.
- Updated the concrete VS2022 and MinGW i686 build lanes and static build validation to include the GDI backend and validation-scene wiring.
- Updated root, build, architecture, host, renderer, and roadmap documentation so Series 05 now describes the real renderer milestone truthfully.

### Validation

- Confirmed Series 00 through Series 04 prerequisites existed before changes.
- Performed static validation of the GDI backend file graph, host/build integration, and documentation consistency.
- Confirmed no OpenGL backend, real saver-product implementation, packaging logic, or release-production logic was added in this series.

## S04 - 2026-03-27

### Added

- A first narrow public platform header set under `platform/include/screensave/` for versioning, diagnostics, common config, renderer contracts, and saver/module contracts.
- A reusable non-renderer core runtime under `platform/src/core/` covering version identity, deterministic RNG, timing, diagnostics, config helpers, and renderer/saver dispatch support.

### Changed

- Replaced the remaining Series 02 core stub in the concrete VS2022 and MinGW i686 build lanes with the real Series 04 shared core runtime.
- Updated the Win32 host skeleton to consume the shared saver/module, common-config, diagnostics, version, and timing services while keeping the placeholder visual host-local.
- Updated `nocturne_entry.c` from a product-identity shell to a no-op saver-module shell against the new public contract.
- Updated root, architecture, build, public-header, core, host, and roadmap docs to describe the Series 04 runtime/core boundary truthfully.

### Validation

- Confirmed Series 00 through Series 03 prerequisites existed before changes.
- Performed static validation of the new public header set, core runtime file graph, host/build integration, and documentation consistency.
- Confirmed no renderer backend, real saver-product implementation, packaging logic, or release-production logic was added in this series.

## S03 - 2026-03-27

### Added

- A real Win32 `.scr` host skeleton under `platform/src/host/win32_scr/` with screen, preview, and configuration execution paths.
- A small configuration dialog resource, provisional per-user settings persistence scaffold, and lightweight diagnostics overlay/version text support.
- A narrow public saver-to-host entry header in `platform/include/screensave/scr_entry.h`.

### Changed

- Replaced the Series 02 host and product-target stubs with a real `nocturne.scr` host shell wired into the VS2022 and MinGW i686 build lanes.
- Updated build validation to check the real host graph, resource integration, and product-entry wiring instead of the earlier stub-only graph.
- Updated root, build, host, and roadmap documentation so Series 03 now describes the real host milestone truthfully.

### Validation

- Confirmed Series 00 through Series 02 prerequisites existed before changes.
- Performed static validation of the host file layout, build graph wiring, resource integration, and documentation consistency.
- Confirmed no reusable renderer backend, real saver-product implementation, packaging logic, or release-production logic was added in this series.

## S02 - 2026-03-27

### Added

- A documented build philosophy and toolchain lane structure under `build/`.
- Concrete MSVC VS2022 build entry files and a concrete MinGW i686 make lane.
- Tiny Series 02 build-only stub sources to make the initial platform-to-product target graph concrete.
- `tools/scripts/check_build_layout.py` for static build-scaffold validation.

### Changed

- Updated CI to validate the build scaffold honestly at the repository's current stage.
- Updated root and roadmap documentation to reflect the new build-layer reality.

### Validation

- Confirmed Series 00 and Series 01 prerequisites were present before changes.
- Performed static validation of the build files, output conventions, and stub-source relationships.
- Confirmed no real host, renderer, saver, packaging, or release implementation was added in this series.

## S01 - 2026-03-27

### Added

- GitHub community-health files including issue forms, issue-template configuration, a pull request template, and CODEOWNERS.
- Conservative GitHub Actions workflows for repository CI, docs health, and manual release scaffolding.
- Lightweight validation helpers under `tools/scripts/` for repository structure, Codex config, docs basics, and release-scaffold checks.
- `CONTRIBUTING.md` with repository boundaries, series expectations, and verification rules.

### Changed

- Updated repository truth-bearing docs to reflect the governance-focused Series 01 state.
- Updated roadmap material so Series 01 now describes governance and CI foundation work instead of the earlier placeholder plan.

### Validation

- Confirmed Series 00 prerequisites existed before changes.
- Validated the new governance files, Codex config, docs basics, and release scaffold inputs with the new helper scripts.
- Confirmed the series added no runtime implementation, build system logic, packaging logic, or release publishing.

## S00 - 2026-03-27

### Added

- Permanent repository skeleton for platform, products, assets, tests, validation, tools, build, packaging, and third-party records.
- Root and subtree `AGENTS.md` guidance for repository, platform, products, tests, tools, and packaging work.
- Project-scoped Codex control plane in `.codex/config.toml` and `.codex/agents/*.toml`.
- Normative specifications for compatibility, saver API, renderer rules, build targets, release readiness, and config boundaries.
- Explanatory architecture, roadmap, and lineup documentation under `docs/`.

### Changed

- Rewrote the root `README.md` into the permanent Series 00 landing page.
- Replaced the earlier temporary bootstrap layout with the long-term repository structure required by the prompt program.

### Validation

- Reviewed repository structure against the Series 00 directory contract.
- Checked specs, docs, AGENTS guidance, and Codex config for internal consistency.
- Confirmed that no runtime implementation, dependencies, CI workflows, or packaging logic were added in this series.
