# Plasma v2 Artistic Decision Packet Policy

This packet prepares ScreenSave-owned artistic decisions for Plasma v2. The
release-candidate packet records rc1 readiness. The stable packet records the
separate final stable artistic verdict. Neither packet publishes a release.
This decision packet does not promote a stable release.

Decision states:

- `not-started`
- `preview-only`
- `stable-candidate-review`
- `accepted-for-release-candidate`
- `accepted-for-stable`
- `request-changes`
- `defer-to-labs`
- `rejected`

Required boundaries:

- Release-candidate acceptance is not final stable artistic acceptance.
- Release-candidate acceptance is not release publication.
- Release-candidate acceptance is not compatibility certification.
- Release-candidate acceptance is not a public SDK promise.
- Stable acceptance, if later recorded, is still not release publication.
- Stable acceptance is still not Windows preservation certification.
- Stable acceptance is still not AIDE automatic promotion.

The packet must reference the proof bundle, stable-candidate review summary,
acceleration comparison, performance envelope, package stage, and known limits
used by the reviewer.

The stable packet must also name the decision ID, decision date, reviewer or
project authority, source commit, release candidate, profile, accepted scope,
excluded scope, visual review summary, instrument architecture summary, known
limit acceptance, and claim boundary.
