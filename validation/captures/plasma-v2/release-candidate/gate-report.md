# Plasma v2 Release-Candidate Gate

Status: fail

Release-candidate gate only; not stable release, release publication, compatibility certification, public SDK stability, all-saver migration, automatic promotion, or visual acceptance without a real-display human verdict.

| Check | Status | Summary |
| --- | --- | --- |
| portable-v2-accepted | pass | Portable v2 must be accepted before Plasma release-candidate work. |
| plasma-status | pass | Plasma v2 must be at release-readiness-reviewed, release-candidate, release-candidate-hold, stable-promoted, publication-ready, or publication-hold. |
| plasma-stability-state | pass | Plasma v2 stability fields must match the current lifecycle state. |
| post-transition-release-candidate | pass | After transition, rc1 must be recorded by authority and Plasma state. |
| post-transition-active-program | pass | After transition, the active program must match the current Plasma v2 decision lane. |
| path-contract | pass | Required release-candidate input exists: contracts/plasma_release_candidate_v1.md |
| path-candidate_package | pass | Required release-candidate input exists: packaging/windows/plasma-v2-release-candidate/manifest.toml |
| path-candidate_checksums | pass | Required release-candidate input exists: packaging/windows/plasma-v2-release-candidate/checksums.sha256 |
| path-proof_matrix | pass | Required release-candidate input exists: validation/captures/plasma-v2/release-candidate/proof-matrix.json |
| path-proof_bundle | pass | Required release-candidate input exists: validation/captures/plasma-v2/release-candidate/proof-bundle-v1.json |
| path-evidence_index | pass | Required release-candidate input exists: validation/captures/plasma-v2/release-candidate/evidence-index.json |
| path-support_claims | pass | Required release-candidate input exists: validation/captures/plasma-v2/release-candidate/support-claims.json |
| path-artistic_decision | pass | Required release-candidate input exists: validation/captures/plasma-v2/final-artistic-decision/decision.release-candidate.toml |
| path-manager_review | pass | Required release-candidate input exists: validation/captures/plasma-v2/release-candidate/manager-review.json |
| path-workbench_review | pass | Required release-candidate input exists: validation/captures/plasma-v2/release-candidate/workbench-review.json |
| path-repair_burndown | pass | Required release-candidate input exists: validation/captures/plasma-v2/release-candidate/repair-burndown.json |
| path-agentic_policy | pass | Required release-candidate input exists: .aide/agentic/release-candidate-policy.toml |
| artistic-decision | pass | Artistic decision must accept release-candidate only and block stable/certification claims. |
| support-claims | pass | Support claims must remain not-stable, release-blocked, and uncertified. |
| repair-burndown | pass | Repair queue must have no open blocking release-candidate repairs. |
| package-not-published | pass | Release-candidate package must remain staged only. |
| visual-rejection-blocker | fail | Active real-display visual rejection blocks Plasma v2 release-candidate publication lineage. |
| fixed-release-candidate-capabilities | pass | Fixed release-candidate capabilities must be exposed. |
| no-generic-or-promotion-capabilities | pass | Generic command, release, stable-promotion, and agent-apply capabilities must remain absent. |
| no-release-overclaims | pass | No publication, stable, certification, or promotion overclaim may exist. |
| validator-deep-release-candidate-subchecks | fail | Deep release-candidate validators skipped because active real-display visual rejection already blocks the release lineage. |
