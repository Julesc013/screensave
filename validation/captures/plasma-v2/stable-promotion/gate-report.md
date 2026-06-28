# Plasma v2 Stable-Promotion Gate Report

- Status: hold
- Candidate: plasma-v2-rc1
- Recommended state: release-candidate-hold
- Artistic decision: request-changes
- Instrument architecture: promotion-ready
- Instrument stable eligible: True
- Open blocking repairs: 1
- Claim boundary: Stable-promotion gate report only; it does not publish a release, certify compatibility, accept artistic quality automatically, or admit AIDE source mutation.

## Checks

- pass: project-state-before-or-hold - Project state is a release candidate before the decision or a non-stable hold after the decision.
- pass: release-candidate-gate - Command passed.
- pass: stable-promotion-contract - Command passed.
- pass: instrument-architecture-audit - Command passed.
- pass: package-stage - Command passed.
- pass: stable-promotion-evidence - Command passed.
- pass: final-artistic-decision - Command passed.
- pass: release-candidate-support - Command passed.
- pass: stable-support - Command passed.
- pass: release-provenance - Command passed.
- pass: release-security - Command passed.
- pass: stable-review - Command passed.
- pass: aide-evidence-bridge - Command passed.
- pass: aide-evidence-index - Command passed.
- pass: repair-queue - Command passed.
- pass: project-adapter - Command passed.
- pass: fixed-stable-capabilities - Project adapter exposes only fixed stable-promotion capabilities.
- pass: no-generic-or-publication-capability - Project adapter does not expose generic command, publication, unchecked promotion, or agent apply capabilities.
- pass: artistic-decision-outcome - Final artistic decision is either accepted-for-stable or a valid non-promotion outcome.
- pass: repair-queue-outcome - Repair queue outcome matches the artistic decision.
- pass: instrument-architecture-outcome - Instrument audit is either promotion-ready or a valid non-promotion hold.
- pass: proof-bundle-decision-axis - Proof bundle records the stable-promotion decision axis.
- pass: no-publication-packet - No stable publication packet has been created during the stable-promotion decision.
