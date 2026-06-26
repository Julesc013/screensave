# Plasma Release Readiness v1

Status: active release-readiness contract.

This contract defines the evidence required to move Plasma v2 from
`stable-candidate` to `release-readiness-reviewed` or later
`release-candidate`. It does not promote Plasma v2 to stable.

## Status Ladder

- `stable-candidate`: proof-backed and reviewed candidate, not release-ready.
- `release-readiness-reviewed`: package, support, preview, review, and
  readiness evidence have been checked.
- `release-candidate`: a later gate may stage a candidate for release decision.
- `stable`: only a separate stable promotion gate may set this state.

## Gate Axes

The release-readiness checker must keep these axes distinct:

- reference proof
- accelerated comparison
- performance envelope
- packc v1 candidate
- Workbench Author/Profile/Review
- Manager preview
- artifact packaging
- support wording
- known limits
- visual review
- final artistic decision
- release promotion

## Boundaries

Release-readiness evidence does not imply:

- stable release,
- compatibility certification,
- final public support expansion,
- public SDK stability,
- all-saver migration,
- automatic promotion,
- automatic source mutation.

Stable promotion remains a PAW-H decision.
