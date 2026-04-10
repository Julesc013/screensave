# PX41 SDK And Automation

## Purpose

This note records the bounded SDK/reference and automation/control surfaces landed by `PX41`.

## Actual SDK And Reference-Facing Surfaces Landed

`PX41` adds the file-first local SDK/reference surface at:

- `products/savers/plasma/integration/sdk_reference.ini`
- `python tools/scripts/plasma_lab.py integration-report`

The current reference surface records:

- Plasma's role as a bounded complex saver
- the stable default routing posture
- the local collection to treat as the default stable reference surface
- the bounded experimental collection to treat as an opt-in reference surface
- the example built-in pack key
- the stable control profile used for deterministic support and reference review
- the current Plasma Lab entrypoint

The current reference surface also consumes the thin shared vocabulary from:

- [`../../../../docs/roadmap/sy40-sdk-reference-contract.md`](../../../../docs/roadmap/sy40-sdk-reference-contract.md)

## Actual Automation And Control Surfaces Landed

`PX41` adds the bounded local control surface at:

- `products/savers/plasma/integration/control_profiles.ini`
- `python tools/scripts/plasma_lab.py control-report --profile <profile_key>`

The shipped profiles are:

- `deterministic_classic`
- `dark_room_support`
- `experimental_matrix`

The current control surface is deliberately small.
It validates and reports local request shapes for:

- preset and theme keys
- optional preset-set, theme-set, and journey keys
- deterministic seed and detail level
- randomization mode
- favorites and exclusions
- whether experimental content is allowed

## What Is Safe And Bounded Now

The current control bridge is safe and bounded because:

- it is report-first and validation-first
- every requested key is checked against the current compiled or authored Plasma catalog
- stable profiles must stay stable-only
- experimental profiles must opt in explicitly
- there is no runtime bypass of the existing product validation or clamping rules

## What Remains Unsupported Or Intentionally Absent

`PX41` does not add:

- a remote control daemon
- a runtime automation bus
- live external modulation
- a broad session replay framework
- any mechanism that bypasses existing validation or clamping

The current control bridge is a bounded local support and determinism surface only.

## Validation, Clamping, And Degrade Notes

The current control-profile validation rejects:

- unknown preset, theme, set, journey, or pack references
- stable-visible profiles that reference experimental content
- stable-visible profiles that allow experimental content
- profile-to-set mismatches

Warnings are still emitted for alias usage so the local bridge surface stays canonical over time.

## Scope Boundary

`PX41` does not create a broad automation framework.
It creates a bounded local report-first control surface that later work may consume carefully.
