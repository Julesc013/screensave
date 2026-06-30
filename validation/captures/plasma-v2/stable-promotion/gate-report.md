# Plasma v2 Stable-Promotion Gate Report

- Status: fail
- Candidate: plasma-v2-rc1
- Recommended state: release-candidate-hold
- Artistic decision: accepted-for-stable
- Instrument architecture: promotion-ready
- Instrument stable eligible: True
- Open blocking repairs: 0
- Claim boundary: Stable-promotion gate report only; it does not publish a release, certify compatibility, accept artistic quality automatically, admit AIDE source mutation, or override a real-display human visual verdict.

## Checks

- pass: project-state-transition - Project state is a release candidate, a valid hold, or an accepted post-stable transition.
- fail: visual-rejection-blocker - Active real-display visual rejection blocks Plasma v2 stable promotion and publication lineage.
- fail: release-candidate-gate - Command failed.
- pass: stable-promotion-contract - Command passed.
- pass: instrument-architecture-audit - Command passed.
- fail: package-stage - Command failed.
- pass: stable-promotion-evidence - Command passed.
- pass: final-artistic-decision - Command passed.
- pass: release-candidate-support - Command passed.
- pass: stable-support - Command passed.
- fail: release-provenance - Command failed.
- pass: release-security - Command passed.
- pass: stable-review - Command passed.
- pass: aide-evidence-bridge - Command passed.
- pass: aide-evidence-index - Command passed.
- pass: repair-queue - Command passed.
- fail: project-adapter - Command failed.
- pass: fixed-stable-capabilities - Project adapter exposes only fixed stable-promotion capabilities.
- pass: no-generic-or-publication-capability - Project adapter does not expose generic command, publication, unchecked promotion, or agent apply capabilities.
- pass: artistic-decision-outcome - Final artistic decision is either accepted-for-stable or a valid non-promotion outcome.
- pass: repair-queue-outcome - Repair queue outcome matches the artistic decision.
- pass: instrument-architecture-outcome - Instrument audit is either promotion-ready or a valid non-promotion hold.
- pass: proof-bundle-decision-axis - Proof bundle records the stable-promotion decision axis.
- pass: publication-boundary - Any stable publication packet remains local, non-published, and upload-free.
