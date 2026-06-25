# Project Adapter v0

Status: active ScreenSave development protocol.

## Purpose

The ScreenSave project adapter is the narrow command surface that external
development coordinators can invoke without owning ScreenSave truth.

AIDE, CI, worker harnesses, or human scripts may call the adapter. AIDE may consume receipts, but it must not define ScreenSave truth. The adapter must delegate to ScreenSave-owned authority files, validators, proof tools, and artifact policies.

## Commands

The v0 adapter command set is:

- `status`: report current project state, source revision, dirty state, active
  program, release posture, and proof-kernel location.
- `capabilities`: report admitted adapter operations and their boundaries.
- `catalog`: report the committed generated product inventory.
- `validate`: run a bounded ScreenSave-owned validation ladder.
- `render`: run the Proof Kernel v0 Nocturne canary render.
- `compare`: compare proof-kernel captures using `sslab`.
- `audit`: run the ScreenSave PE artifact audit and report binary facts.
- `proof`: run the Proof Kernel v0 Nocturne canary and exact comparison.

## Boundaries

The adapter may:

- invoke repository validators
- invoke `sslab`
- invoke the PE artifact audit
- emit JSON receipts
- write generated proof artifacts under `out/`
- expose limitations and refusal reasons

The adapter may not:

- edit source files
- merge branches
- publish releases
- certify operating-system compatibility
- accept visual artistic quality
- redefine saver semantics
- make AIDE mandatory for builds or releases

## Result Shape

Every command returns a JSON object with:

- `adapter_schema`
- `project`
- `command`
- `status`
- `source`
- command-specific payload
- `limits`

Status values are `pass`, `fail`, `informational`, or `blocked`.

## Initial Proof

The initial proof command renders the Nocturne `observatory_night` canary using
`sslab render`, compares it with the committed canary capture using
`sslab compare --class exact`, and writes an adapter receipt.

This is deterministic proof-kernel evidence. It is not a public compatibility
certification and not a visual-artistic acceptance decision.

The `audit` command records binary facts only. It is informational by default
and does not certify compatibility.
