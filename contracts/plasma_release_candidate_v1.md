# Plasma v2 Release-Candidate Contract v1

Status: active release-candidate contract.

This contract governs the PAW-H transition from
`release-readiness-reviewed` to `release-candidate`. It is a review and
evidence gate. It is not a stable release contract and it does not authorize
publication.

## Required Input State

- `portable_v2.status = "accepted"`
- `plasma_v2.status = "release-readiness-reviewed"`
- `plasma_v2.stable = false`
- `authority.release_candidate = "none"`
- `plasma_v2.release_promotion = "blocked"`
- `authority.active_program = "plasma-v2-release-candidate"`

## Gate Axes

The release-candidate gate must review these axes as separate claims:

- reference proof
- accelerated comparison
- performance envelope
- packc v1-candidate
- package stage
- Manager preview
- Workbench release-readiness
- support/known-limits wording
- visual review
- final artistic release-candidate decision
- native artifact evidence
- AIDE evidence
- release-candidate state transition

No axis may be flattened into a generic pass bit. Deterministic proof,
performance evidence, package staging, artifact audit, visual review, release
candidate readiness, stable release promotion, and compatibility certification
remain different claim classes.

## Decision States

The checker may permit exactly these PAW-H outcomes:

- `release-candidate`
- `request-changes`
- `defer-to-labs`

The first accepted candidate identifier is `plasma-v2-rc1`.

## Explicit Exclusions

PAW-H must not claim or perform:

- stable release
- public release publication
- compatibility certification broadening
- public SDK stability
- all-saver migration
- automatic AIDE promotion
- automatic AIDE source mutation
- automatic merge
- automatic release publication

## Compatibility Claim Rule

Native artifact evidence may support release-candidate readiness only for the
named artifact profile and evidence class.
PE audit with zero violations is a binary fact; it is not OS certification.
The target Windows band remains a target until runtime evidence exists for
specific systems.

## State Transition Rule

`PROJECT_STATE.toml` may set:

```toml
[authority]
release_candidate = "plasma-v2-rc1"

[plasma_v2]
status = "release-candidate"
stable = false
release_candidate = "plasma-v2-rc1"
release_promotion = "blocked"
```

Only PAW-I may decide whether `stable = true` or
`release_promotion = "accepted"` is appropriate.
