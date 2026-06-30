# Plasma v2 Publication-Prep Gate Report

Status: publication-hold
Candidate: plasma-v2-rc1
Recommended state: publication-hold

## Checks

- publication-packet-paths: pass - Publication-prep packet paths exist.
- project-state: pass - Project state is stable-promoted/publication-ready or an explicit visual-rejection publication hold.
- visual-rejection-blocker: fail - Active real-display visual rejection blocks Plasma v2 publication.
- release-manifest: pass - Release manifest records publication-prep only.
- release-manifest-artifacts-refs: pass - Release manifest artifacts refs exist.
- release-manifest-evidence-refs: pass - Release manifest evidence refs exist.
- release-manifest-documents-refs: pass - Release manifest documents refs exist.
- artifact-manifest: pass - Artifact manifest records unpublished publication-prep artifacts.
- artifact-manifest-refs: pass - Artifact manifest paths exist.
- checksum-file-format: pass - Publication-prep checksum file is parseable.
- checksums-match: fail - Publication-prep checksums match referenced files.
- doc-known-limits: pass - releases\plasma-v2-stable\known-limits.md records required publication-prep wording.
- doc-support-matrix: pass - releases\plasma-v2-stable\support-matrix.md records required publication-prep wording.
- doc-install-notes: pass - releases\plasma-v2-stable\install-notes.md records required publication-prep wording.
- doc-rollback-notes: pass - releases\plasma-v2-stable\rollback-notes.md records required publication-prep wording.
- doc-third-party-notices: pass - releases\plasma-v2-stable\third-party-notices.md records required publication-prep wording.
- doc-publication-checklist: pass - releases\plasma-v2-stable\publication-checklist.md records required publication-prep wording.
- publication-provenance: fail - Publication-prep provenance records artifact and boundary facts.
- publication-provenance-refs: pass - Publication-prep provenance refs exist.
- stable-promotion-accepted: fail - Stable-promotion gate remains accepted before publication prep.
- aide-publication-summary: pass - AIDE publication summary records evidence-only posture.
- no-publication-receipts: pass - No publication/upload receipt exists in PAW-J.
- publication-prep-contract: pass - Command passed.
- visual-rejection-hold: pass - Command passed.
- release-provenance: fail - Command failed.
- release-security: pass - Command passed.
- package-stage: fail - Command failed.
- aide-evidence-index: pass - Command passed.

## Boundary

Publication-prep gate only; no public release publication, compatibility certification broadening, SDK stability, all-saver migration, Manager install mutation, Workbench MVP, platform expansion, AIDE runtime authority, or visual acceptance without a real-display human verdict is admitted.
