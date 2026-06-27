# Scripts

Purpose: general repository support scripts.
Belongs here: helper scripts for validation, maintenance, and local workflow support.
Does not belong here: generated outputs or runtime code shipped with savers.
Current stage:
- `check_repo_structure.py` validates required governance files and workflow anchors.
- `check_codex_config.py` validates the project-scoped Codex configuration.
- `check_docs_basics.py` checks markdown links and roadmap/doc basics.
- `check_build_layout.py` validates the checked-in build scaffold and target relationships.
- `check_build_controller.py` validates the fixed build-profile controller and dry-run receipt boundary.
- `check_project_state.py` validates `PROJECT_STATE.toml`, `VERSION.toml`, linked authority files, and prints the current released/development/queue/compatibility state with `--summary`.
- `check_catalog_profiles.py` validates `catalog/products.toml`, `catalog/artifact_profiles.toml`, product artifact-profile references, and evidence-status vocabulary.
- `check_artifact_sets.py` validates `catalog/artifact_sets.toml`, `catalog/proof_profiles.toml`, and the artifact manifest tool boundary.
- `check_catalog_generated.py` verifies that committed `catalog/generated/` inventory and proof-registry outputs match `tools/cataloggen/cataloggen.py`.
- `check_aide_pilot.py` validates the bounded AIDE Lite operational control plane, fixed-command bridge admission plan, ignored local state, linked work-unit/evidence paths, and the no-product-runtime-dependency boundary.
- `check_aide_operational.py` validates the PAW-CX ScreenSave-local AIDE bootstrap repair files, controller failure taxonomy, token baselines, validation-tier map, deterministic ScreenSave golden task definitions, and operation-receipt wrapper boundary.
- `check_aide_evidence_bridge.py` validates the ScreenSave AIDE EvidencePacket exporter by generating a fixed Ricochet v2 Proof Bundle v1 projection with portable v2 equivalence and checking that compatibility certification, artistic review, release promotion, and AIDE runtime dependency remain separate claims.
- `check_aide_evidence_index.py` validates `.aide/evidence/index.toml` so AIDE can index ScreenSave proof references without becoming proof authority or a runtime dependency.
- `check_portable_v2.py` validates the public portable v2 header seam: fixed-width C89-compatible aliases, no native platform terms, no public `long`, and size/version-prefixed public structures.
- `check_product_core_boundaries.py` validates that Nocturne and Ricochet portable cores stay free of Win32, v1 saver host APIs, renderer APIs, and public or proof-observable `long` usage.
- `check_contracts.py` validates the ScreenSave doctrine, portable-semantics, renderer-alias, proof-bundle, proof-kernel, and project-adapter contract scaffolding.
- `check_visual_intent_contract.py` validates the draft VisualIntent v1 contract and example authoring intent.
- `check_product_architecture.py` validates the active product architecture contract, product-family boundaries, canary sequence, and catalog fit.
- `check_plasma_instrument_architecture.py` audits whether Plasma v2 stable promotion satisfies the direct-control visual-instrument architecture, emits the PAW-I instrument audit report, and cleanly distinguishes promotion-ready from release-candidate hold.
- `check_plasma_product_center.py` audits Plasma v2 product-center file boundaries and records whether legacy Plasma files are still blocking instrument promotion.
- `check_plasma_legacy_boundary.py` audits that old Plasma files are compatibility shims or migration inputs rather than hidden product truth.
- `check_plasma_v2_plan.py` validates the direct Plasma v2 plan compiler, requested-versus-resolved spec facts, renderer fallback, explicit degradation flags, seeds, and software/reference requirements.
- `check_plasma_v2_runtime.py` validates the direct Plasma v2 runtime buffer contract, deterministic frame state, phase streams, RNG state, and C89 smoke coverage.
- `check_plasma_v2_no_hot_loop_hazards.py` rejects heap allocation, file I/O, renderer APIs, Win32 handles, and runtime trig/math hazards from the direct v2 runtime/field/output island.
- `check_plasma_v2_field_pipeline.py` validates the direct Plasma v2 sources, generators, modifiers, output transform, material mapping, treatment, and flat presentation stages.
- `check_plasma_v2_influence.py` validates the direct Plasma v2 Basic-control influence matrix for Field, Scale, Complexity, Motion, Speed, Warp, Feedback, Material, Contrast, Brightness, Softness, Treatment, and Seed.
- `check_plasma_v2_material_response.py` validates direct Plasma v2 material response, contrast/brightness bounds, softness/treatment post-material behavior, and dark-room comfort.
- `check_plasma_v2_migration.py` validates legacy Plasma effect/speed/resolution/smoothing/preset/theme migration into direct Plasma v2 specs.
- `check_plasma_v2_workbench_inspection.py` validates Workbench inspection of requested spec, resolved plan, degradation, pipeline, material response, treatment stack, software reference, GL11 optionality, captures, control influence, review, and promotion status without creating a second runtime.
- `check_plasma_v2_aide_repair_evidence.py` validates AIDE repair scan, plan, proposal-only receipt, and remaining-blocker evidence for the Plasma v2 instrument repair wave.
- `check_plasma_v2_instrument_reaudit.py` reruns the Plasma v2 instrument architecture audit and records the PAW-I-R1 re-audit decision as either promotion-ready or a clean release-candidate hold.
- `check_proof_bundle_v1.py` validates the normalized Proof Bundle v1 contract, lifecycle/performance receipt inputs, and normalizer output axes.
- `check_workbench_shell.py` validates the minimal PAW-B Workbench shell binding to `sslab` and generated proof profiles.
- `check_sslab_runner.py` builds one private static `libsslab` artifact and validates the generic `sslab_runner` proof path for Nocturne and Ricochet.
- `check_sslab_abi_v1.py` validates the fixed-width private `libsslab` ABI v1 header and compiles a portable-v2 smoke path for Nocturne and Ricochet.
- `check_proof_kernel.py` validates the Proof Kernel v0 private surface, software-renderer scaffolding, `sslab` canary command, and deterministic proof output.
- `check_compiled_nocturne_runner.py` builds the compiled Proof Kernel v0 Nocturne canary and exact-compares it with committed evidence.
- `check_project_adapter.py` validates the ScreenSave project adapter status, capabilities, catalog, validation, render, compare, audit, and proof command surface.
- `audit_pe_artifacts.py` records PE architecture, subsystem, import, identity, and structured JSON facts for checked-out `.scr` and `.exe` artifacts; missing roots or zero discovered PE artifacts are blocked, and exact proof lanes should pass `--artifact-manifest` plus `--artifact-profile`.
- `check_shared_settings_layout.py` validates the C04 shared settings, preset, randomization, and pack scaffold.
- `check_windows_integration_layout.py` validates the C05 Win32 screen, preview, config, persistence, and metadata hardening scaffold.
- `check_portable_bundle_layout.py` validates the frozen C16 Core bundle definition, staged bundle, and frozen-status docs.
- `check_installer_layout.py` validates the frozen C16 installer companion definition, staged package, payload provenance, and frozen-status docs.
- `check_meta_saver_layout.py` validates the C08 anthology meta-saver product tree, orchestration hooks, BenchLab inspection hooks, and continuation-status docs.
- `check_suite_app_layout.py` validates the C09 suite app product tree, build-lane integration, manifest-driven saver coverage, role boundaries, and continuation-status docs.
- `check_sdk_surface.py` validates the C10 contributor-facing SDK surface, the real saver template, and contributor-authored saver or pack roots.
- `check_backlog_surface.py` validates the C11 backlog, routing, wave-planning, and refinement-thread surface plus the active continuation-status docs.
- `check_suite_consistency_surface.py` validates the C12 cross-cutting consistency pass, the quality-bar note, and the active continuation-status docs.
- `check_wave_a_surface.py` validates the C13 Wave A saver-polish note, the in-wave saver preset/theme/config-hook surface, the updated wave routing docs, and the active continuation-status docs.
- `check_wave_b_surface.py` validates the C13 Wave B saver-polish note, the in-wave saver preset/theme/config-hook surface, the updated wave routing docs, and the active continuation-status docs.
- `check_wave_c_surface.py` validates the C13 Wave C saver-polish note, the in-wave saver preset/theme/config-hook surface, the anthology long-run safeguards, and the active continuation-status docs.
- `check_release_candidate_surface.py` validates the preserved C14 release-candidate notes, the C15 doctrine handoff, manifest/version alignment, and historical staged outputs.
- `check_release_baseline_surface.py` validates the frozen C16 Core baseline notes, manifest/version alignment, companion-artifact references, and refreshed staged outputs.
- `check_release_scaffold.py` generates a manual release-scaffold report without publishing anything.
- `plasma_lab.py` provides the bounded Plasma Lab shell for authored-substrate validation, authoring inventory, authored-object comparison, compatibility and migration reporting, local integration and control reporting, local curation and provenance reporting, pack or capture degrade inspection, and deterministic BenchLab text-capture comparison.
Type: tooling.
