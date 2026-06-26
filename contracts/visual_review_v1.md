# Visual Review v1

Status: active preview-review contract.

Visual Review v1 defines how ScreenSave records human review of deterministic
preview captures without turning that review into release readiness,
compatibility certification, or final artistic acceptance.

## Scope

Visual review may evaluate:

- composition
- motion quality
- palette and material quality
- luminance safety
- dark-room comfort
- control influence
- distinctness
- artifacting
- degradation honesty
- preview and fullscreen suitability

The review input must point to deterministic capture evidence, proof receipts,
and any contact sheets used by the reviewer.

## Decision Classes

Allowed decision classes are:

- `accepted-for-preview`
- `request-changes`
- `rejected`
- `defer-to-labs`

`accepted-for-preview` means the slice can continue as a preview candidate. It
does not mean stable release, Core promotion, compatibility certification, or
final artistic acceptance.

For stable-candidate review rounds, allowed decision classes are:

- `accepted-for-stable-candidate`
- `request-changes`
- `defer-to-labs`
- `reject`

`accepted-for-stable-candidate` means the candidate may proceed to a later
release-readiness wave. It still does not mean stable release, compatibility
certification, Core promotion, or final artistic acceptance.

## Required Boundaries

Visual Review v1 must keep these claims separate:

- reference-preview is not artistic acceptance
- artistic acceptance is not stable release
- stable release is not compatibility certification
- development-plane evidence is not ScreenSave product truth

## Non-Decisions

A Visual Review v1 decision must not decide:

- stable release
- compatibility certification
- Core promotion
- all-platform support
- public SDK stability
- automatic product promotion

## Required Record

A review decision record must include:

- schema version
- product
- profile
- decision class
- reviewer or reviewer role
- reviewed capture references
- review dimensions and notes
- explicit non-decisions
- claim boundary

Review records live under `validation/` and remain evidence for the named slice
only.
