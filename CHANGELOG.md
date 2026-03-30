# Changelog

All notable repository changes are recorded here in prompt history order.

## C14 - 2026-03-30

### Added

- `packaging/release_notes/release-candidate-notes.md` as the final release-candidate summary for the converged saver line, the separate app products, current local output reality, and the canonical checkpoint tag.
- `validation/notes/c14-portable-bundle-matrix.md`, `validation/notes/c14-installer-matrix.md`, `validation/notes/c14-release-readiness-matrix.md`, `validation/notes/c14-config-integrity.md`, and `validation/notes/c14-known-issues.md` as the explicit release-readiness, integrity, distribution, and limitation record for the final rerelease pass.
- `tools/scripts/check_release_candidate_surface.py` as the final static validator for the C14 release-candidate notes, manifest-version alignment, refreshed staged outputs, and closed continuation-status surface.

### Changed

- Refreshed the portable bundle definition under `packaging/portable/` to the `screensave-portable-c14-rc` release-candidate surface, including the new release-readiness, known-issues, integrity, and release-candidate notes staged beside the saver payload.
- Refreshed the installer definition under `packaging/installer/` to the `screensave-installer-c14-rc` release-candidate surface, aligned the staged docs with the current portable payload, and kept the installer truthful about its current-user-only and payload-partial limits.
- Normalized the current real saver and app manifests to `version=0.15.0`, updated the shared version-series identity to `C14 Final Rerelease Hardening Release Candidate`, and synchronized root, roadmap, backlog, architecture, packaging, and tooling docs so the continuation line now closes cleanly at `C14`.
- Updated the release scaffold and stage-specific validators so the final RC checks audit the current package names, the final note set, the closed continuation line, and the current separation between saver-only distribution and the separate `suite` / `benchlab` app products.

### Validation

- Confirmed before editing that the continuation reset, rename normalization, migration checkpoint, saver productization matrix, shared settings architecture, Windows integration baseline, portable bundle, installer baseline, suite meta-saver, suite app, SDK surface, backlog-routing surface, cross-cutting polish baseline, and all three `C13` saver-polish waves already existed in the repo.
- Kept `C14` bounded to release-hardening work only: this pass did not add new saver families, new renderer tiers, new packaging modes, or fresh architecture work beyond what was necessary to make the current release surface coherent and auditable.
- Final `C14` evidence remains truthful and environment-bounded; a fresh full-suite native rebuild was not available here, so release readiness is recorded as static plus packaging plus local-output discovery evidence rather than fabricated runtime success.

## C13 Wave C - 2026-03-30

### Added

- `validation/notes/c13-wave-c-polish.md` as the explicit Wave C scope, product-outcome, shared-surface, validation-level, and next-step note for the final saver-specific polish pass.
- `tools/scripts/check_wave_c_surface.py` as the small static validator for the Wave C saver surface, the anthology long-run safeguards, and the updated continuation-status docs.

### Changed

- Polished `stormglass` with calmer dark-room weather pacing, stronger winter and monochrome cold-pane variants, preview-aware scene refresh, and more deliberate light, condensation, and droplet choreography without turning it into a weather engine.
- Polished `transit` with calmer cinematic reseed cadence, stronger motorway-fog and harbor-midnight variants, preview-safe route sway, and better long-run light refresh without widening it into a driving or map system.
- Polished `observatory` with calmer exhibit pacing, stronger planetarium-blue and brass-eclipse variants, preview-safe scene refresh, and more deliberate body and star reseeding without turning it into an astronomy package.
- Polished `vector` with stronger amber-wire and terrain-glide curation, preview-aware scene reseeding, calmer long-run compositional refresh, and better renderer-sensitive pacing without widening it into a general 3D engine.
- Polished `explorer` with calmer deterministic route refresh, stronger eerie-corridor and amber-service variants, better preview pacing, and less repetitive traversal loops without turning it into an FPS or map framework.
- Polished `city` with stronger harbor and rooftop variants, calmer long-run light and fog choreography, preview-aware scenic reset behavior, and more deliberate night-world composition without widening it into a city simulator.
- Polished `atlas` with stronger nebula-voyage and mathematics-plate curation, calmer preview hold timing, more deliberate route progression, and steadier anthology-style long-run presentation without turning it into a fractal laboratory.
- Polished `gallery` with stronger amber-compatibility and GL21-prism-hall curation, tier-change-aware scenic refresh, and clearer long-run renderer-showcase pacing without turning it into the suite browser.
- Polished `anthology` with calmer preview-safe interval defaults, stronger compatibility and scenic preset coverage, and anti-repeat selection damping that reduces awkward loopbacks while keeping the meta-saver bounded.
- Completed the Wave C saver-only polish pass without broadening `suite`, packaging, installer behavior, renderer tiers, or out-of-wave products, and updated roadmap, backlog, architecture, tooling, validation, and root status docs so `C13` Wave C is recorded as complete and `C14` final rerelease hardening is now next.

### Validation

- Confirmed before editing that the continuation reset, rename normalization, migration checkpoint, saver productization matrix, shared settings architecture, Windows integration baseline, portable bundle, installer baseline, suite meta-saver, suite app, SDK surface, backlog-routing surface, `C12` quality-bar baseline, and `C13` Wave A and Wave B results already existed in the repo.
- Kept `C13` Wave C bounded to `stormglass`, `transit`, `observatory`, `vector`, `explorer`, `city`, `atlas`, `gallery`, and `anthology` plus narrow validation, docs, and settings-hook parity work only; this prompt did not add new renderer tiers, packaging changes, installer changes, suite-app expansion, or out-of-wave saver work.
- Final verification for `C13` Wave C is static and source-level in this environment; no supported MSVC or MinGW toolchain was available for a fresh native build or runtime capture pass.

## C13 Wave B - 2026-03-30

### Added

- `validation/notes/c13-wave-b-polish.md` as the explicit Wave B scope, product-outcome, shared-surface, validation-level, and next-step note for the second saver-specific polish pass.
- `tools/scripts/check_wave_b_surface.py` as the small static validator for the Wave B saver surface, the updated wave-routing docs, and the active continuation-status docs.

### Changed

- Polished `pipeworks` with staged pulse-spawn timing, stronger rebuild cadence, more varied restart origins, calmer preview behavior, clearer density wording, and curated amber/blueprint preset and theme families without turning it into a general tile or flow engine.
- Polished `lifeforms` with calmer preview pacing, richer long-run seed choreography, clearer garden versus archive theme separation, better reseed variety, and real preset/theme/randomization import-export hooks where the product had previously advertised but not implemented them.
- Polished `signals` with stronger panel-family cadence, quieter preview-safe defaults, clearer telemetry versus watch-console preset families, more deliberate pseudo-data rhythm, and real preset/theme/randomization import-export hooks where the product had previously advertised but not implemented them.
- Polished `mechanize` with calmer exhibit pacing, stronger scene-family distinction, periodic layout reconfiguration for long unattended runs, curated copper and ivory material variants, and real preset/theme/randomization import-export hooks where the product had previously advertised but not implemented them.
- Polished `ecosystems` with stronger regroup choreography, calmer preview population sizing, clearer reef and marsh habitat families, better long-run event pacing, and weighted randomization that now favors calmer room-safe defaults.
- Updated roadmap, backlog, architecture, tooling, validation, and root status docs so `C13` Wave B is recorded as complete for the second five polished savers and `C13` Wave C is now the next continuation step instead of leaving the repo anchored on the earlier Wave A baseline.

### Validation

- Confirmed before editing that the continuation reset, rename normalization, migration checkpoint, saver productization matrix, shared settings architecture, Windows integration baseline, portable bundle, installer baseline, suite meta-saver, suite app, SDK surface, backlog-routing surface, `C12` quality-bar baseline, and `C13` Wave A results already existed in the repo.
- Kept `C13` Wave B bounded to `pipeworks`, `lifeforms`, `signals`, `mechanize`, and `ecosystems` plus narrow docs and validation updates only; this prompt did not add new renderer tiers, packaging changes, installer changes, out-of-wave saver work, or suite-app/meta-saver expansion.
- Final verification for `C13` Wave B is static and source-level in this environment; no supported MSVC or MinGW toolchain was available for a fresh native build or runtime capture pass.

## C13 Wave A - 2026-03-30

### Added

- `validation/notes/c13-wave-a-polish.md` as the explicit Wave A scope, product-outcome, shared-surface, validation-level, and next-step note for the first saver-specific polish pass.
- `tools/scripts/check_wave_a_surface.py` as the small static validator for the Wave A saver surface, the updated wave-routing docs, and the active continuation-status docs.

### Changed

- Polished `nocturne` with stronger near-black presets and themes, subtler ghost motion, calmer long-run drift refresh, and better dark-room pacing without widening the product beyond its restrained identity.
- Polished `ricochet` with cleaner deterministic starts, calmer rhythm refresh, stronger trail taper, curated amber and corporate variants, and real preset/theme/randomization import-export parity with its product-owned settings surface.
- Polished `deepfield` with animated twinkle, calmer preview travel, periodic scene refresh for long unattended runs, new deep-space preset and theme coverage, and real preset/theme/randomization import-export hooks where the product had previously advertised but not implemented them.
- Polished `plasma` with clearer effect-family palette behavior, curated midnight and amber dark-room variants, softer long-run composition refresh, and preset import-export wording that now matches the checked-in product preset files more closely.
- Polished `phosphor` with stronger amber and white laboratory variants, calmer long-run ratio refresh, improved preview-safe pacing, more deliberate instrument overlay presentation, and real preset/theme/randomization import-export hooks where the product had previously advertised but not implemented them.
- Updated roadmap, backlog, architecture, tooling, and root status docs so `C13` is now the active saver-polish phase, Wave A is recorded as complete for the first five products, and `C13` Wave B is the next continuation step instead of prematurely advancing to `C14`.

### Validation

- Confirmed before editing that the continuation reset, rename normalization, migration checkpoint, saver productization matrix, shared settings architecture, Windows integration baseline, portable bundle, installer baseline, suite meta-saver, suite app, SDK surface, backlog-routing surface, and `C12` quality-bar baseline already existed in the repo.
- Kept `C13` Wave A bounded to `nocturne`, `ricochet`, `deepfield`, `plasma`, and `phosphor` plus narrow docs and validation updates only; this prompt did not add new renderer tiers, packaging changes, installer changes, out-of-wave saver work, or suite-app/meta-saver expansion.
- Final verification for `C13` Wave A is static and source-level in this environment; no supported MSVC or MinGW toolchain was available for a fresh native build or runtime capture pass.

## C12 - 2026-03-30

### Added

- `docs/roadmap/c12-suite-quality-bar.md` as the durable suite-consistency standard for the later saver-specific polish waves, including the canonical shared outer vocabulary, action semantics, renderer wording, and preservation rules.
- `validation/notes/c12-suite-consistency-audit.md` as the explicit audit note for the bounded cross-cutting normalization pass and the next continuation step.
- `tools/scripts/check_suite_consistency_surface.py` as the small static validator for the C12 quality-bar surface, vocabulary normalization, BenchLab manifest coverage, and active continuation-status docs.

### Changed

- Added the shared internal display-text helper under `platform/src/core/base/` and wired it into the host, Suite, and BenchLab so renderer names, selection paths, fallback causes, renderer status, detail levels, and randomization labels now use one coherent human-readable grammar instead of leaking raw internal tokens.
- Normalized the shared Win32 host shell wording to `Renderer preference` and `Session randomization`, tightened the single-saver and selector dialog wording, and aligned host renderer choice labels with the current `gdi` / `gl11` / `gl21` vocabulary plus the later placeholders.
- Updated Suite so presets and themes now surface display names in end-user combo boxes and the info pane, renderer support and family labels are human-readable, and the app-level actions now distinguish `Apply` from `Revert Changes` instead of overloading `Reset`.
- Updated BenchLab and anthology to keep diagnostics or product-owned specificity while aligning with the suite-wide vocabulary, and corrected BenchLab's manifest so it truthfully describes the full current saver line including `anthology`.
- Updated root, roadmap, architecture, product, tooling, and validation docs so `C12` is recorded as complete, the suite quality bar is explicit, and `C13` first saver-specific polish work is the next continuation step.

### Validation

- Confirmed before editing that the roadmap reset, canonical rename baseline, migration checkpoint, saver productization matrix, shared settings architecture, Windows integration baseline, portable bundle, installer baseline, suite meta-saver, suite app, SDK surface, and backlog-routing surface already existed in the repo.
- Kept `C12` bounded to cross-cutting polish only: this prompt did not add new saver-local deep features, new renderer capabilities, packaging changes, installer changes, or new product families.
- Final verification for `C12` is static and source-level in this environment; no supported MSVC or MinGW toolchain was available for a fresh native build or runtime checkpoint.

## C11 - 2026-03-30

### Added

- `docs/roadmap/backlog-routing.md` as the explicit routing taxonomy, priority model, field set, and wave-bucket definition for post-`C10` work.
- `docs/roadmap/backlog-inventory.md` as the structured backlog inventory covering cross-cutting items, every current saver, the key non-saver products, release/distribution follow-on work, and the parked concept pool.
- `docs/roadmap/refinement-threads.md` as the practical operating model for master coordination, shared-platform work, release/distribution work, suite/meta coordination, and per-saver refinement threads.
- `validation/notes/c11-backlog-routing-baseline.md` as the compact baseline note recording the new planning surface and the next continuation step.
- `tools/scripts/check_backlog_surface.py` as the small stdlib-only validator for backlog coverage, routing docs, refinement-thread notes, and active continuation-status docs.

### Changed

- Updated root and roadmap status docs so `C11` is recorded as complete, the suite is described as entering a structured refinement phase, and `C12` cross-cutting polish is the next continuation step.
- Updated `docs/roadmap/products-and-lineup.md` so future working-title concepts are explicitly treated as parked backlog items rather than near-term implementation promises.
- Updated the tooling index so the new backlog validator is discoverable beside the earlier continuation-status checks.

### Validation

- Confirmed before editing that the roadmap reset, canonical rename baseline, `C02` migration checkpoint, `C03` saver productization matrix, `C04` shared settings architecture, `C05` Windows integration baseline, `C06` portable bundle, `C07` installer baseline, `C08` suite meta-saver, `C09` suite app, and `C10` SDK surface already existed in the repo.
- Kept `C11` planning-only: this prompt did not add saver features, renderer work, packaging expansion, installer changes, or polish-wave implementation.
- Final verification for `C11` is documentation and static-validator based; no new runtime or toolchain evidence is claimed here.

## C10 - 2026-03-30

### Added

- `products/savers/_template/` as the real contributor starter saver tree, including a truthful manifest, starter preset sets, a real entry shim, module hooks, config skeleton, theme and preset descriptors, bounded session and render paths, config resources, version metadata, and lightweight smoke coverage.
- `products/sdk/` contributor docs for saver authoring, pack authoring, manifest and file-format guidance, copy-map notes, and validation checklists that point at the real template surface rather than a placeholder.
- `products/sdk/examples/template_pack/` as the minimal example data pack with a real `pack.ini`, one `.preset.ini`, and one `.theme.ini` aligned with the shared `C04` settings and pack formats.
- `tools/scripts/check_sdk_surface.py` as the stdlib-only contributor validator for the checked-in SDK surface plus contributor-authored saver roots, pack roots, and preset/theme files.
- `validation/notes/c10-sdk-contributor-surface.md` as the explicit baseline note for the new SDK and contributor surface, its boundaries, and the next continuation step.

### Changed

- Updated `products/sdk/README.md`, `products/sdk/examples/README.md`, `products/sdk/template_saver/README.md`, and `products/README.md` so the SDK area now describes the real contributor workflow instead of future placeholder material.
- Updated root, roadmap, architecture, lineup, and tooling docs so `C10` is recorded as complete, the contributor surface is a real first-class part of the repo, and `C11` backlog ingestion and routing is the next continuation step.
- Updated the continuation-status validators and tooling index so active repo truth now expects the real SDK and contributor-surface baseline rather than the earlier `C10 is next` placeholder state.

### Validation

- Confirmed before editing that the roadmap reset, canonical rename baseline, `C02` migration checkpoint, `C03` saver productization matrix, `C04` shared settings architecture, `C05` Windows integration baseline, `C06` portable distribution bundle, `C07` installer baseline, `C08` suite meta-saver, and `C09` suite app already existed in the repo.
- Audited the existing placeholder `products/sdk/` and `products/savers/_template/` trees, representative saver manifests and source layouts, current preset/theme/pack formats, and current validation helpers before defining the real contributor surface.
- Kept `C10` bounded to templates, examples, documentation, and contributor validation helpers only; this prompt did not add a runtime plugin loader, packaging changes, installer changes, broader suite-app work, or new shipping saver products.
- Final verification for `C10` is static and source-level in this environment; no supported MSVC or MinGW toolchain was available for a fresh native compile checkpoint.

## C09 - 2026-03-30

### Added

- `products/apps/suite/` as the real suite app product, including its manifest, bounded browser/preview/launch/config sources, stable role-boundary notes, and lightweight smoke coverage.
- `build/msvc/vs2022/suite.vcxproj` plus `build/msvc/vs2022/suite_target_sources.props` as the explicit VS2022 app target for `suite.exe`, including the checked-in list of linked saver modules and saver-owned config resources the app reuses.
- `validation/notes/c09-suite-app-matrix.md` as the explicit product, discovery, launch/config, role-boundary, and evidence-level record for the `C09` suite app baseline.
- `tools/scripts/check_suite_app_layout.py` as the small static validator for the `C09` suite product tree, build-lane integration, manifest-driven saver coverage, and continuation-status docs.

### Changed

- Updated the MinGW i686 and VS2022 build lanes so `suite` is a first-class app target beside BenchLab while preserving the existing shared-platform plus many-product architecture and keeping standalone `.scr` savers, `anthology`, BenchLab, and Suite in distinct roles.
- Updated root, roadmap, architecture, build, app-boundary, and anthology docs so `C09` is recorded as complete, `suite` is the real suite-level browser and launcher surface, BenchLab remains diagnostics-only, `anthology` remains the meta-saver, and `C10` SDK / contributor-surface work is the next continuation step.
- Updated the continuation-status validators so active repo truth now expects the real `suite` app baseline instead of the earlier `C09 is next` placeholder state.

### Validation

- Confirmed before editing that the roadmap reset, rename normalization, post-rename checkpoint, all-saver productization matrix, shared settings architecture, Windows integration validation note, portable bundle architecture, installer architecture, and `anthology` meta-saver baseline already existed in the repo.
- Audited the saver manifests, shared settings hooks, BenchLab role, anthology role, suite placeholder tree, build-lane conventions, and active roadmap/status docs before promoting `suite` to a real app product.
- Ran `python tools/scripts/check_build_layout.py` and `python tools/scripts/check_suite_app_layout.py` after wiring `suite` into the checked-in VS2022 and MinGW lanes; both passed.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, `python tools/scripts/check_docs_basics.py`, `python tools/scripts/check_build_layout.py`, `python tools/scripts/check_canonical_naming.py`, `python tools/scripts/check_shared_settings_layout.py`, `python tools/scripts/check_windows_integration_layout.py`, `python tools/scripts/check_portable_bundle_layout.py`, `python tools/scripts/check_installer_layout.py`, `python tools/scripts/check_meta_saver_layout.py`, `python tools/scripts/check_suite_app_layout.py`, `python -m py_compile tools/scripts/check_build_layout.py tools/scripts/check_canonical_naming.py tools/scripts/check_shared_settings_layout.py tools/scripts/check_windows_integration_layout.py tools/scripts/check_portable_bundle_layout.py tools/scripts/check_installer_layout.py tools/scripts/check_meta_saver_layout.py tools/scripts/check_suite_app_layout.py`, and `git diff --check` on the final tree.
- Confirmed that supported MSVC and MinGW toolchains were still unavailable in this environment, so `C09` evidence remains static and source-level rather than a fresh native build/run checkpoint.
- Confirmed this prompt did not add SDK/plugin work, packaging refresh, installer expansion, renderer changes, or unrelated saver-feature work.

## C08 - 2026-03-30

### Added

- `products/savers/anthology/` as the real suite meta-saver product, including its manifest, bounded in-process orchestration module, weighted/favorites/family-filter config surface, presets, theme metadata, config dialog resources, and lightweight smoke coverage.
- `build/msvc/vs2022/anthology.vcxproj` plus `build/msvc/vs2022/anthology_target_sources.props` as the explicit VS2022 saver target for `anthology.scr`, including the checked-in list of the eighteen inner saver modules that the meta-saver links in-process.
- `validation/notes/c08-meta-saver-matrix.md` as the explicit product, filtering, renderer, host, BenchLab, and evidence-level record for the `C08` suite meta-saver baseline.

### Changed

- Updated BenchLab so the diagnostics harness can select `anthology` as a first-class saver module and surface the active inner saver, previous saver, filter summary, selection reason, and renderer context in the overlay without becoming the future `suite` app.
- Updated the MinGW i686 and VS2022 build lanes so `anthology` is a first-class `.scr` target while preserving the explicit shared-platform plus many-product architecture and avoiding external `.scr` process orchestration.
- Updated root, roadmap, architecture, build, and BenchLab docs so `C08` is recorded as complete, `anthology` is the canonical suite meta-saver identity, and `C09` `suite` app work is the next continuation step.

### Validation

- Confirmed before editing that the roadmap reset, rename normalization, post-rename checkpoint, all-saver productization matrix, shared settings architecture, Windows integration validation note, portable bundle architecture, and installer architecture already existed in the repo.
- Audited the saver registry shape, shared settings hooks, current renderer-tier behavior, BenchLab diagnostics surface, build-lane conventions, and active roadmap status docs before introducing the suite meta-saver product.
- Ran `python tools/scripts/check_build_layout.py` after wiring `anthology` into the checked-in VS2022 and MinGW lanes; it passed.
- Confirmed that supported MSVC and MinGW toolchains were still unavailable in this environment, so `C08` evidence remains static and source-level rather than a fresh native build/run checkpoint.
- Confirmed this prompt did not add suite-app work, packaging expansion, installer changes, new renderer functionality, or unrelated saver-feature work.

## C07 - 2026-03-30

### Added

- `packaging/installer/installer_manifest.ini`, `packaging/installer/layout.md`, and `packaging/installer/build_installer.py` as the explicit `C07` installer package definition, install-layout note, and stdlib-only installer assembly path.
- `packaging/installer/installer_common.ps1`, `packaging/installer/install_screensave.ps1`, and `packaging/installer/uninstall_screensave.ps1` as the bounded current-user installer, optional saver-selection assistance, uninstall-record, and conservative uninstall implementation.
- `packaging/release_notes/installer-release-notes.md` as the user-facing release-support note for the installed distribution path.
- `validation/notes/c07-installer-matrix.md` as the explicit install, update, uninstall, current payload coverage, and evidence-level note for `C07`.
- `tools/scripts/check_installer_layout.py` as the small static validator for the `C07` installer definition, staged installer package, payload provenance, and active status docs.

### Changed

- Updated `packaging/README.md`, `packaging/installer/README.md`, and `packaging/release_notes/README.md` so the packaging tree now describes the real installed-distribution architecture beside the existing portable bundle path.
- Updated `packaging/portable/README.md`, `packaging/portable/layout.md`, `packaging/release_notes/portable-release-notes.md`, and `packaging/portable/assemble_portable.py` so the portable path now truthfully records installed distribution as a separate `C07` delivery mode instead of a future placeholder.
- Generated a real staged installer package at `out/installer/screensave-installer-c07/` and a matching zip at `out/installer/screensave-installer-c07.zip` from the current staged portable payload under `out/portable/screensave-portable-c06/`.
- Updated `README.md`, roadmap docs, architecture overview, and continuation-status validators so `C07` is recorded as complete and `C08` suite meta-saver work is the next continuation step.

### Validation

- Confirmed before editing that the roadmap reset, canonical rename baseline, `C02` migration checkpoint, `C03` saver productization matrix, `C04` shared settings architecture, `C05` Windows integration validation note, and `C06` portable bundle baseline already existed in the repo.
- Audited the packaging tree, portable payload, release-contract notes, staged `out/portable/` artifacts, and current saver metadata before defining the installer architecture.
- Ran `python packaging/installer/build_installer.py` to stage the installer package and generate the installer zip from the real portable payload only.
- Ran the staged installer in mock mode for first install with `-SetActiveSaver nocturne`, conservative overlay reinstall without changing the active saver request, and uninstall from the installed `INSTALLER\\` copy, leaving logs under `out/installer/validation/` and restoring the mock desktop state to an empty saver path plus no uninstall entries.
- Re-ran `python packaging/portable/assemble_portable.py` after updating the portable release notes so the staged portable bundle now reflects the existence of the separate installed-distribution path.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, `python tools/scripts/check_docs_basics.py`, `python tools/scripts/check_build_layout.py`, `python tools/scripts/check_canonical_naming.py`, `python tools/scripts/check_shared_settings_layout.py`, `python tools/scripts/check_windows_integration_layout.py`, `python tools/scripts/check_portable_bundle_layout.py`, `python tools/scripts/check_installer_layout.py`, `python -m py_compile packaging/portable/assemble_portable.py packaging/installer/build_installer.py tools/scripts/check_canonical_naming.py tools/scripts/check_shared_settings_layout.py tools/scripts/check_windows_integration_layout.py tools/scripts/check_portable_bundle_layout.py tools/scripts/check_installer_layout.py`, and `git diff --check` on the final tree.
- Confirmed that supported C/C++ toolchains were still unavailable in this environment, so the `C07` installer package is an honest current-user scripted installer built from the partial real portable payload instead of a freshly rebuilt full saver line.
- Confirmed this prompt did not add suite-app work, suite meta-saver work, new saver features, or new renderer functionality.

## C06 - 2026-03-30

### Added

- `packaging/portable/bundle_manifest.ini`, `packaging/portable/layout.md`, and `packaging/portable/assemble_portable.py` as the explicit `C06` portable bundle definition, layout note, and stdlib-only assembly path.
- `packaging/release_notes/portable-release-notes.md` as the user-facing release-support note for the portable bundle.
- `validation/notes/c06-portable-bundle-matrix.md` as the explicit inclusion matrix for the current canonical saver line, including the discovered canonical outputs, excluded legacy outputs, BenchLab exclusion, pack policy, and next continuation step.
- `tools/scripts/check_portable_bundle_layout.py` as the small static validator for the portable bundle definition, staged output, and post-`C06` status docs.

### Changed

- Updated `packaging/README.md`, `packaging/portable/README.md`, and `packaging/release_notes/README.md` so the packaging tree now describes the real portable-bundle architecture and the continued deferment of installer work to `C07`.
- Generated a real staged portable bundle at `out/portable/screensave-portable-c06/` and a matching zip at `out/portable/screensave-portable-c06.zip` from the canonical saver outputs currently present in local output roots, while excluding legacy saver outputs, BenchLab, and suite-level products.
- Updated `README.md`, roadmap docs, and the continuation-status validators so `C06` is recorded as complete and `C07` installer, registration, and uninstall flow is the next continuation step.

### Validation

- Confirmed before editing that the roadmap reset, canonical rename baseline, `C02` migration checkpoint, `C03` saver productization matrix, `C04` shared settings architecture, and `C05` Windows integration validation note already existed in the repo.
- Audited `out/` and the checked-in build-output conventions to distinguish currently available canonical saver outputs from missing savers, legacy pre-rename outputs, and developer-only BenchLab binaries before assembling the portable bundle.
- Ran `python packaging/portable/assemble_portable.py` to stage the portable folder and generate the zip from real discovered outputs only.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, `python tools/scripts/check_docs_basics.py`, `python tools/scripts/check_build_layout.py`, `python tools/scripts/check_canonical_naming.py`, `python tools/scripts/check_shared_settings_layout.py`, `python tools/scripts/check_windows_integration_layout.py`, `python tools/scripts/check_portable_bundle_layout.py`, `python -m py_compile packaging/portable/assemble_portable.py tools/scripts/check_canonical_naming.py tools/scripts/check_shared_settings_layout.py tools/scripts/check_windows_integration_layout.py tools/scripts/check_portable_bundle_layout.py`, and `git diff --check` on the final tree.
- Confirmed that supported C/C++ toolchains were unavailable in this environment, so the `C06` portable bundle is an honest staged partial bundle assembled from pre-existing canonical outputs instead of a freshly rebuilt full saver line.
- Confirmed this prompt did not add installer logic, suite-app work, suite-meta work, or unrelated saver/renderer feature work.

## C05 - 2026-03-30

### Added

- `validation/notes/c05-windows-integration-matrix.md` as the explicit Windows lifecycle audit record for the current saver line, including per-saver notes for screen, preview, config, persistence, renderer, and multi-monitor status plus the remaining limitations and next continuation step.
- `tools/scripts/check_windows_integration_layout.py` as the small static validator for the C05 Win32 host hardening surface, saver metadata consistency, and active repo status notes.
- `IDD_SCR_SAVER_SHELL` in the shared host resources so standalone saver products can expose a bounded host-owned settings shell before opening their saver-specific dialog.

### Changed

- Hardened the Win32 `.scr` host so standalone saver `/c` mode now routes through a bounded single-saver settings shell that stages shared common settings, renderer preference, and saver-specific dialog edits before saving on `OK`.
- Hardened preview and fullscreen behavior so preview remains a true child-window path with live parent revalidation and resize sync while fullscreen mode now sizes against the Windows virtual desktop and resynchronizes on `WM_DISPLAYCHANGE`.
- Normalized the remaining stale saver metadata by updating `products/savers/lifeforms/manifest.ini` to the canonical product/identity/notes layout used by the rest of the current saver line.
- Updated `README.md`, roadmap docs, the Win32 host README, and the naming validator so `C05` is recorded as complete and `C06` portable distribution bundle is the next continuation step.

### Validation

- Confirmed before editing that the roadmap reset, canonical rename baseline, `C02` migration checkpoint, `C03` saver productization matrix, and `C04` shared settings architecture already existed in the repo.
- Performed a narrow audit of the real Win32 host lifecycle, single-saver config routing, preview-parent handling, fullscreen sizing, saver manifests, and active roadmap/status docs to distinguish concrete hardening work from still-deferred packaged runtime evidence.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, `python tools/scripts/check_docs_basics.py`, `python tools/scripts/check_build_layout.py`, `python tools/scripts/check_canonical_naming.py`, `python tools/scripts/check_shared_settings_layout.py`, `python tools/scripts/check_windows_integration_layout.py`, `python -m py_compile tools/scripts/check_canonical_naming.py tools/scripts/check_shared_settings_layout.py tools/scripts/check_windows_integration_layout.py`, and `git diff --check` on the final tree.
- Confirmed that native Windows Control Panel / packaged `.scr` smoke execution and toolchain builds could not be rerun in this environment, so the C05 matrix records honest static validation plus code hardening rather than fake runtime evidence.
- Confirmed this prompt did not add portable distribution, installer flow, suite-meta behavior, suite-app work, or a broad saver polish wave.

## C04 - 2026-03-30

### Added

- `platform/include/screensave/settings_api.h` plus `platform/src/core/config/settings.c` as the shared settings, preset/theme import-export, pack manifest, and pack discovery surface for the renamed eighteen-saver product line.
- `validation/notes/c04-shared-settings-architecture.md` as the explicit baseline note for the shared settings taxonomy, file formats, pack rules, migration/version boundaries, representative saver adoption, and the next continuation step.
- Sample built-in packs under `products/savers/plasma/packs/`, `products/savers/transit/packs/`, and `products/savers/atlas/packs/` so the new pack format is exercised by real repository data instead of documentation-only examples.

### Changed

- Extended the shared config and saver-module contracts with schema-versioned common state, settings capability flags, import/export hooks, and product-local randomization hooks while preserving product-local ownership of actual saver semantics.
- Updated the Win32 host to carry both stored and resolved per-session saver config, added a modest shared randomization control to the fallback settings dialog, and kept the configuration path bounded to the normal saver dialog flow instead of building a new suite UI framework.
- Updated BenchLab to inspect the resolved per-session config state and surface active preset, theme, detail, and randomization information without turning the diagnostics harness into the future `suite` app.
- Adopted the shared settings surface broadly across the saver line and added deeper import/export/randomization support in representative savers `nocturne`, `plasma`, `vector`, `ecosystems`, `transit`, and `atlas`.
- Updated `README.md`, roadmap docs, `specs/config_schema.md`, BenchLab docs, and the validation helpers so the repository now records `C04` as complete and names `C05` Windows picker, config, preview, and fullscreen validation as the next continuation step.

### Validation

- Confirmed before editing that the roadmap reset, rename/taxonomy normalization, post-rename checkpoint, and all-saver `.scr` productization baselines already existed in the repo.
- Performed a narrow audit of shared config state, host dialog wiring, BenchLab diagnostics, representative saver hooks, pack sample layout, roadmap status notes, and the config schema spec to remove the remaining `C04 is next` drift from active repo truth.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, `python tools/scripts/check_docs_basics.py`, `python tools/scripts/check_build_layout.py`, `python tools/scripts/check_canonical_naming.py`, `python tools/scripts/check_shared_settings_layout.py`, and `git diff --check` on the final tree.
- Confirmed this prompt did not add packaging, installer, suite-meta, suite-app, or plugin-SDK work.

## C03 - 2026-03-30

### Added

- `platform/src/host/win32_scr/scr_product_version.rc` as the shared version-resource template for saver product binaries.
- Product-local `*_version.rc` files for every current saver so the standalone saver line now carries explicit per-product version and identity metadata.
- `build/msvc/vs2022/saver_target_common.props` as the shared VS2022 saver-project property sheet for the one-saver-per-target productization matrix.
- `validation/notes/c03-productization-matrix.md` as the explicit all-saver `.scr` productization record.

### Changed

- Updated every saver entry shim so `nocturne` through `gallery` now bind directly to their owning saver module through `screensave_scr_main()` instead of embedding the full multi-saver registry into every distributed `.scr`.
- Updated the VS2022 saver projects and the MinGW i686 make lane so each saver target now compiles only its owning saver sources, its product config dialog resource, its product version resource, the shared host resource, and the shared platform library.
- Preserved BenchLab as a separate diagnostics harness that still links the saver modules directly rather than depending on the distributed `.scr` product line.
- Narrowed remaining host wording away from the earlier built-in-saver distribution assumption and updated repo truth docs so `C03` is complete and `C04` shared settings/presets/randomization/pack architecture is the next continuation step.
- Updated `tools/scripts/check_build_layout.py` so the checked-in build validator now enforces the single-saver entry/binding contract, the per-saver resource matrix, and the continued separation between saver products and BenchLab.

### Validation

- Confirmed before editing that the `C00` roadmap reset, `C01` rename/taxonomy normalization, and `C02` migration-baseline checkpoint already existed in the repo.
- Performed static audit and cleanup of the saver entry shims, saver project files, MinGW lane, host diagnostics wording, and current build/docs language to remove the old shared multi-saver distribution assumption from active repo truth.
- Ran `python tools/scripts/check_build_layout.py` after the build/docs reconciliation; it passed on the final tree.
- Confirmed this prompt did not add packaging, installer, suite-meta, suite-app, shared settings/randomization framework, or new saver-feature work.

## C02 - 2026-03-30

### Added

- `validation/notes/c02-migration-baseline.md` as the explicit post-rename migration-baseline record, including the canonical saver/app/renderer naming regime, the legacy-to-canonical mapping summary, the intentional historical exceptions, the remaining migration aliases, and the next continuation step.
- `tools/scripts/check_canonical_naming.py` as a small checkpoint-integrity helper for the locked canonical saver/app naming baseline and its key status docs.

### Changed

- Updated `README.md`, `docs/roadmap/prompt-program.md`, `docs/roadmap/series-map.md`, `docs/roadmap/post-s15-plan.md`, and `docs/roadmap/rename-map.md` so the repo now records `C02` as complete and names `C03` all-saver `.scr` productization as the next continuation prompt.
- Updated the narrow build-status READMEs under `build/` so they describe the current repository as the locked `C02` post-rename baseline instead of the earlier `C01` checkpoint-in-progress state.
- Audited active docs, manifests, build files, host strings, BenchLab labels, and migration paths to confirm that canonical saver/app naming is coherent and that the remaining old-name references are either explicit migration aliases or historical evidence.
- Created the annotated recovery tag `c02-post-rename-baseline` for the locked post-rename checkpoint.

### Validation

- Confirmed before editing that the repo already contained the `C00` roadmap reset, the explicit rename map, the renderer-tier taxonomy note, the completed `C01` rename/taxonomy refactor, the canonical `suite` placeholder, the canonical `atlas` identity, and the implemented `S00` through `S15` history.
- Ran targeted stale-name and stale-taxonomy audits to distinguish acceptable historical references in changelog and older validation notes from the active canonical repo truth.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, `python tools/scripts/check_docs_basics.py`, `python tools/scripts/check_build_layout.py`, `python tools/scripts/check_canonical_naming.py`, and `git diff --check`; all passed.
- Confirmed this prompt remained a stabilization/checkpoint step and did not add new saver features, new renderer behavior, all-saver `.scr` productization, packaging logic, installer work, or suite/meta-product implementation.

## C01 - 2026-03-30

### Changed

- Renamed the implemented saver products, directories, manifests, build targets, and project references to the canonical post-`S15` slugs: `plasma`, `phosphor`, `signals`, `mechanize`, `transit`, `explorer`, `vector`, `atlas`, `city`, and `gallery`.
- Renamed the old app-level `gallery` placeholder to `products/apps/suite/`, kept `products/apps/player/` only as a superseded legacy stub, and updated the repo docs so `gallery` is now unambiguously the saver while `suite` is the future app identity.
- Normalized active renderer-tier naming across code, manifests, docs, and diagnostics to the explicit `gdi -> gl11 -> gl21 -> gl33 -> gl46 -> null` ladder without claiming new renderer functionality beyond the existing baseline.
- Added narrow old-to-new migration handling so legacy saver selection keys resolve to canonical slugs, renamed savers can load legacy per-product registry roots before resaving under canonical roots, `plasma` accepts the legacy `ember_lava` preset/theme key, and BenchLab retains `gl_plus` / `glplus` only as legacy aliases for canonical `gl21`.
- Updated roadmap, architecture, build, saver, and app docs so `C01` is recorded as complete and the next continuation path is `C02` checkpointing followed by `C03` all-saver `.scr` productization.

### Validation

- Confirmed the `C00` continuation roadmap reset, rename map, renderer-tier taxonomy note, `S00` through `S15` changelog history, saver/app placeholder tree, and legacy saver/build identifiers existed before applying the rename/taxonomy refactor.
- Ran targeted stale-name and stale-taxonomy searches across active repo truth to confirm the canonical saver slugs, the `suite` app identity, the `atlas` merge, and the explicit renderer ladder are now the active naming baseline.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, `python tools/scripts/check_docs_basics.py`, `python tools/scripts/check_build_layout.py`, and `git diff --check`; all passed.
- Confirmed this prompt did not add new saver features, new renderer tiers, all-saver `.scr` productization, packaging logic, installer work, or suite/meta-product behavior.

## C00 - 2026-03-30

### Added

- `docs/roadmap/post-s15-plan.md` as the explicit post-`S15` continuation program, starting from the real `S15` baseline, retiring the old short `S16` / `S17` endpoint, and naming `C01` as the next implementation prompt.
- `docs/roadmap/rename-map.md` as the canonical saver/app normalization guide, including the one-word saver slug targets, the `suite` app decision, and the `Infinity Atlas -> atlas` merge rule.

### Changed

- Updated `docs/roadmap/prompt-program.md` and `docs/roadmap/series-map.md` so the active continuation line now runs `C00` through `C14` instead of stopping at the older short post-`S15` endpoint.
- Updated `docs/roadmap/products-and-lineup.md`, `README.md`, `docs/architecture/overview.md`, and the `products/apps/` placeholder READMEs so the public planning layer no longer implies the old saver/app naming model.
- Normalized `specs/renderer_contract.md` and the post-`S15` planning docs around the explicit `gdi -> gl11 -> gl21 -> gl33 -> gl46 -> null` ladder without claiming future tiers as implemented facts.

### Validation

- Confirmed before editing that the repo already contained the implemented `S00` through `S15` history in `CHANGELOG.md`, roadmap docs under `docs/roadmap/`, saver/app placeholders, renderer directories, subtree `AGENTS.md` files, and the project-scoped `.codex` config.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, `python tools/scripts/check_docs_basics.py`, and `python tools/scripts/check_build_layout.py`; all passed.
- Confirmed this prompt remained planning-only and did not rename directories, move code, change build targets, change manifests, add runtime features, or implement packaging/suite/product logic.

## S15 - 2026-03-29

### Added

- A real optional advanced GL backend under `platform/src/render/gl21/`, including capability-gated context creation, capability capture, backend-private state management, and the narrow primitive surface required by GL Gallery.
- GL Gallery under `products/savers/gl_gallery/` as the renderer-showcase saver, including real compatibility, GL11-classic, and advanced-showcase scene families, product-owned config, curated presets/themes, a manifest, and lightweight smoke coverage.
- Explicit GL33 and GL46 placeholder backend directories plus an internal null safety backend so the renderer ladder can be named honestly without pretending higher tiers already exist.

### Changed

- Updated the shared renderer dispatch so the runtime now supports explicit `auto`, `gdi`, `gl11`, `gl21`, `gl33`, and `gl46` request paths, records requested-versus-active renderer state, and reports explicit fallback reasons across the versioned renderer ladder down to the internal null safety backend.
- Updated the Win32 `.scr` host so the current built-in saver selection dialog also persists the requested renderer tier and surfaces tier state through the existing host diagnostics without turning the host into the final gallery or a renderer lab.
- Updated BenchLab so the diagnostics harness can select and inspect all eighteen current savers plus the explicit `gdi`, `gl11`, `gl21`, `gl33`, and `gl46` request paths, including requested renderer, active renderer, fallback reason, and useful GL capability identity.
- Updated the concrete VS2022 and MinGW i686 build lanes, static build checks, product manifests, and repo docs so Series 15 truthfully describes the versioned GL tier ladder, the GL Gallery product, and the widened eighteen-product harness.

### Validation

- Confirmed Series 00 through Series 14 prerequisites existed before changes.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, and `python tools/scripts/check_build_layout.py`; all passed.
- Confirmed the checked-in VS2022 solution, saver projects, BenchLab project, platform project, and MinGW i686 make lane now include the GL21 backend, GL33 and GL46 placeholders, the null safety backend, and GL Gallery alongside the existing saver set.
- Confirmed that full Win32 build and smoke execution could not be completed in this environment because `MSBuild.exe` / Visual C++ targets were not installed and `mingw32-make` was unavailable.
- Confirmed no Series 16 meta-product work, packaging logic, or unsupported compatibility claims were added in this series.

## S14 - 2026-03-29

### Added

- Vector Worlds under `products/savers/vector_worlds/` as the Heavyweight Worlds Family software-3D product, including real wireframe-field, tunnel-flow, and terrain-line scene modes, product-owned config, curated presets/themes, a manifest, and lightweight smoke coverage.
- Retro Explorer under `products/savers/retro_explorer/` as the Heavyweight Worlds Family traversal product, including real corridor, industrial-passage, and canyon-run scene modes, product-owned config, curated presets/themes, a manifest, and lightweight smoke coverage.
- City Nocturne under `products/savers/city_nocturne/` as the Heavyweight Worlds Family urban scenic product, including real skyline, harbor-edge, and rooftop-flyover scene modes, product-owned config, curated presets/themes, a manifest, and lightweight smoke coverage.
- Fractal Atlas under `products/savers/fractal_atlas/` as the Heavyweight Worlds Family fractal-voyage product, including real atlas, voyage, and Julia-style presentation modes, product-owned config, curated presets/themes, a manifest, and lightweight smoke coverage.

### Changed

- Updated the built-in saver wiring so the current `.scr` products can default to Nocturne, Ricochet, Deepfield, Ember, Oscilloscope Dreams, Pipeworks, Lifeforms, Signal Lab, Mechanical Dreams, Ecosystems, Stormglass, Night Transit, Observatory, Vector Worlds, Retro Explorer, City Nocturne, or Fractal Atlas while preserving the shared saver/module contract, explicit product identity, and automatic renderer-selection behavior.
- Updated BenchLab so the diagnostics harness can select and inspect all seventeen current savers through the shared saver and renderer paths while keeping the UI modest and developer-facing.
- Updated the concrete VS2022 and MinGW i686 build lanes, static build checks, product manifests, and repo docs so Series 14 truthfully describes the heavyweight worlds family and the widened seventeen-product harness.

### Validation

- Confirmed Series 00 through Series 13 prerequisites existed before changes.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, and `python tools/scripts/check_build_layout.py`; all passed.
- Confirmed the checked-in VS2022 solution, saver projects, BenchLab project, and MinGW i686 make lane now include Vector Worlds, Retro Explorer, City Nocturne, and Fractal Atlas alongside the existing saver set.
- Confirmed that full Win32 build and smoke execution could not be completed in this environment because `MSBuild.exe` / Visual C++ targets were not installed and `mingw32-make` was unavailable.
- Confirmed no higher-capability GL tier or modern-context work, next-series work, gallery/player scope, packaging logic, or unsupported compatibility claims were added in this series.

## S13 - 2026-03-29

### Added

- Stormglass under `products/savers/stormglass/` as the Places and Atmosphere Family weather-on-glass product, including real pane-weather modes, product-owned config, curated presets/themes, a manifest, and lightweight smoke coverage.
- Night Transit under `products/savers/night_transit/` as the Places and Atmosphere Family nocturnal infrastructure product, including real motorway, rail, and harbor scene modes, product-owned config, curated presets/themes, a manifest, and lightweight smoke coverage.
- Observatory under `products/savers/observatory/` as the Places and Atmosphere Family celestial exhibit product, including real orrery, chart-room, and dome-watch modes, product-owned config, curated presets/themes, a manifest, and lightweight smoke coverage.

### Changed

- Updated the built-in saver wiring so the current `.scr` products can default to Nocturne, Ricochet, Deepfield, Ember, Oscilloscope Dreams, Pipeworks, Lifeforms, Signal Lab, Mechanical Dreams, Ecosystems, Stormglass, Night Transit, or Observatory while preserving the shared saver/module contract, explicit product identity, and automatic renderer-selection behavior.
- Updated BenchLab so the diagnostics harness can select and inspect all thirteen current savers through the shared saver and renderer paths while keeping the UI modest and developer-facing.
- Updated the concrete VS2022 and MinGW i686 build lanes, static build checks, product manifests, and repo docs so Series 13 truthfully describes the new places/atmosphere family and the widened thirteen-product harness.

### Validation

- Confirmed Series 00 through Series 12 prerequisites existed before changes.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, and `python tools/scripts/check_build_layout.py`; all passed.
- Confirmed the checked-in VS2022 solution, saver projects, BenchLab project, and MinGW i686 make lane now include Stormglass, Night Transit, and Observatory alongside the existing saver set.
- Confirmed that full Win32 build and smoke execution could not be completed in this environment because `MSBuild.exe` / Visual C++ targets were not installed and `mingw32-make` was unavailable.
- Confirmed no higher-capability GL tier or modern-context work, next-family work, gallery/player scope, packaging logic, or unsupported compatibility claims were added in this series.

## S12 - 2026-03-29

### Added

- Signal Lab under `products/savers/signal_lab/` as the Systems and Ambient Family synthetic instrument product, including real panel-layout modes, product-owned config, curated presets/themes, a manifest, and lightweight smoke coverage.
- Mechanical Dreams under `products/savers/mechanical_dreams/` as the Systems and Ambient Family kinetic assembly product, including real coupled-motion scenes, product-owned config, curated presets/themes, a manifest, and lightweight smoke coverage.
- Ecosystems under `products/savers/ecosystems/` as the Systems and Ambient Family ambient habitat product, including real habitat modes, restrained behavior-driven motion, product-owned config, curated presets/themes, a manifest, and lightweight smoke coverage.

### Changed

- Updated the built-in saver wiring so the current `.scr` products can default to Nocturne, Ricochet, Deepfield, Ember, Oscilloscope Dreams, Pipeworks, Lifeforms, Signal Lab, Mechanical Dreams, or Ecosystems while preserving the shared saver/module contract, explicit product identity, and automatic renderer-selection behavior.
- Updated BenchLab so the diagnostics harness can select and inspect all ten current savers through the shared saver and renderer paths while keeping the UI modest and developer-facing.
- Updated the concrete VS2022 and MinGW i686 build lanes, static build checks, product manifests, and repo docs so Series 12 truthfully describes the new systems/ambient family and the widened ten-product harness.

### Validation

- Confirmed Series 00 through Series 11 prerequisites existed before changes.
- Ran `python tools/scripts/check_repo_structure.py`, `python tools/scripts/check_codex_config.py`, and `python tools/scripts/check_build_layout.py`; all passed.
- Confirmed the checked-in VS2022 solution, saver projects, BenchLab project, and MinGW i686 make lane now include Signal Lab, Mechanical Dreams, and Ecosystems alongside the existing saver set.
- Confirmed that full Win32 build and smoke execution could not be completed in this environment because `MSBuild.exe` / Visual C++ targets were not installed and `mingw32-make` was unavailable.
- Confirmed no higher-capability GL tier or modern-context work, next-family work, gallery/player scope, packaging logic, or unsupported compatibility claims were added in this series.

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
- Confirmed no higher-capability GL tier or modern-context work, next-family work, gallery/player scope, packaging logic, or unsupported compatibility claims were added in this series.

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
- Confirmed no higher-capability GL tier or modern-context work, next-family work, gallery/player scope, packaging logic, or unsupported compatibility claims were added in this series.

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
- Confirmed no higher-capability GL tier or modern-context work, next-family work, gallery/player scope, packaging logic, or unsupported compatibility claims were added in this series.

## S08 - 2026-03-27

### Added

- A real optional OpenGL 1.1 backend under `platform/src/render/gl11/`, including conservative WGL context creation, capability capture, present handling, and the current baseline primitive/bitmap path required by the shared renderer contract.
- Explicit runtime renderer selection and fallback reporting so sessions can request `auto`, `gdi`, or `gl11` while keeping GDI as the guaranteed floor.

### Changed

- Updated the shared renderer dispatch and renderer-info model narrowly so requested renderer, active renderer, fallback reason, and GL vendor/renderer/version can be reported honestly without introducing higher-capability GL-tier or modern-context assumptions.
- Updated the Win32 `.scr` host to use automatic shared-renderer selection while keeping the host lifecycle narrow and preserving GDI as the fallback baseline.
- Updated BenchLab so developers can select the requested renderer explicitly, inspect requested-versus-active backend state, and validate Nocturne across both renderer paths without adding player or gallery scope.
- Updated the concrete VS2022 and MinGW i686 build lanes, static build checks, manifests, and repo docs so Series 08 truthfully describes the optional GL11 backend and dual-backend validation state.

### Validation

- Confirmed Series 00 through Series 07 prerequisites existed before changes.
- Performed static validation of the GL11 backend file graph, renderer-selection wiring, build integration, and documentation consistency.
- Confirmed no higher-capability GL tier or modern-context work, second-saver work, gallery/player scope, packaging logic, or unsupported compatibility claims were added in this series.

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
