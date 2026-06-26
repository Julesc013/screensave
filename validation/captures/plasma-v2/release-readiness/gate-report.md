# Plasma v2 Release-Readiness Gate

Status: pass

Plasma v2 release-readiness review only; not stable release, release publication, compatibility certification, public SDK stability, or automatic promotion.

| Check | Status | Summary |
| --- | --- | --- |
| portable-v2-accepted | pass | Portable v2 must be accepted before release-readiness review. |
| plasma-status-input | pass | Plasma v2 must enter as stable-candidate and may exit as release-readiness-reviewed. |
| plasma-not-stable | pass | Plasma v2 must remain not stable and release promotion must remain blocked. |
| path-release_readiness_contract | pass | Required release-readiness evidence exists: contracts/plasma_release_readiness_v1.md |
| path-package_stage | pass | Required release-readiness evidence exists: packaging/windows/plasma-v2-preview/manifest.toml |
| path-package_known_limits | pass | Required release-readiness evidence exists: packaging/windows/plasma-v2-preview/known-limits.md |
| path-manager_preview_source | pass | Required release-readiness evidence exists: products/apps/suite/src/manager_pack_preview.c |
| path-workbench_shell | pass | Required release-readiness evidence exists: products/apps/benchlab/src/benchlab_workbench_shell.c |
| path-pack_example | pass | Required release-readiness evidence exists: products/savers/plasma/content/v2/examples/plasma_lava_v2.toml |
| path-performance_envelope | pass | Required release-readiness evidence exists: validation/captures/plasma-v2/performance/envelope.json |
| path-artistic_decision | pass | Required release-readiness evidence exists: validation/captures/plasma-v2/final-artistic-decision/decision.release-candidate.toml |
| path-agentic_policy | pass | Required release-readiness evidence exists: .aide/agentic/policy.toml |
| path-repair_queue | pass | Required release-readiness evidence exists: .aide/repairs/index.toml |
| path-capability_bindings | pass | Required release-readiness evidence exists: tools/project_adapter/capability_bindings.json |
| package-stage-boundary | pass | Package stage must remain unpublished, not stable, and release-blocked. |
| artistic-decision-release-candidate-only | pass | Artistic decision may accept release-candidate preparation only. |
| fixed-release-readiness-capabilities | pass | Fixed release-readiness capabilities must be exposed. |
| no-generic-or-release-capabilities | pass | Generic command, release, promotion, and agent-apply capabilities must remain absent. |
| no-release-overclaims | pass | No stable release, publication, certification, or final acceptance overclaim may exist. |
| no-aide-product-runtime-dependency | pass | Product/runtime tree must not depend on AIDE. |
| validator-check_plasma_v2_stable_candidate | pass | Required release-readiness validator must pass. |
| validator-check_plasma_v2_package_stage | pass | Required release-readiness validator must pass. |
| validator-check_manager_pack_preview | pass | Required release-readiness validator must pass. |
| validator-check_workbench_shell | pass | Required release-readiness validator must pass. |
| validator-check_packc | pass | Required release-readiness validator must pass. |
| validator-check_plasma_v2_performance | pass | Required release-readiness validator must pass. |
| validator-check_plasma_v2_artistic_decision | pass | Required release-readiness validator must pass. |
| validator-check_agentic_policy | pass | Required release-readiness validator must pass. |
| validator-check_repair_queue | pass | Required release-readiness validator must pass. |
| validator-check_project_adapter | pass | Required release-readiness validator must pass. |
