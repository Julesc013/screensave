# Changelog

All notable repository changes are recorded here in series order.

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
