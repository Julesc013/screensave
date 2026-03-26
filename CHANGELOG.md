# Changelog

All notable repository changes are recorded here in series order.

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
