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

## Fixed Capability Bindings

The admitted capability profile is committed at
`tools/project_adapter/capability_bindings.json`. Command-specific receipt
requirements are committed at `tools/project_adapter/receipt_schemas.json`.
Artifact-profile audit roots are committed at
`tools/project_adapter/artifact_profile_audit_roots.json`.

External coordinators must bind to these fixed capabilities, not to an open
`screensave run <anything>` operation:

- `screensave.project.status`
- `screensave.project.capabilities`
- `screensave.catalog.read`
- `screensave.validation.core`
- `screensave.proof.nocturne.render`
- `screensave.proof.capture.compare`
- `screensave.artifact.pe.audit`
- `screensave.proof.nocturne.exact`

Each capability has a declared argv shape, decoder schema, mutation policy,
output policy, timeout, and state probe. Changes to this file are project
contract changes and must be reviewed as such.

## Boundaries

The adapter may:

- invoke repository validators
- invoke `sslab`
- invoke the PE artifact audit
- emit JSON receipts
- write generated proof artifacts only under contained invocation roots in
  `out/aide/screensave-project-adapter/invocations/`
- expose limitations and refusal reasons

The adapter may not:

- edit source files
- merge branches
- publish releases
- certify operating-system compatibility
- accept visual artistic quality
- redefine saver semantics
- make AIDE mandatory for builds or releases
- accept arbitrary output paths from callers
- read compare or audit inputs outside approved ScreenSave evidence or artifact
  roots
- accept arbitrary PE audit filesystem paths instead of artifact-profile audit
  roots

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

Blocked receipts include a typed `refusal` object. Initial refusal codes are:

- `invalid_invocation_id`
- `output_root_escape`
- `input_not_found`
- `input_not_file`
- `input_root_denied`
- `quota_exceeded`
- `command_timeout`
- `command_failed`

Generated-output commands accept a sanitized `--invocation-id`; they do not
accept caller-selected absolute output paths. The adapter owns the output root,
creates an invocation directory, and writes an `artifact-manifest.json` with
paths, sizes, and SHA-256 digests for generated artifacts.

The `audit` command accepts named `--artifact-profile` selections, not
free-form filesystem paths. Those names are resolved through the committed
artifact-profile audit-root map and remain binary-fact evidence only.

## Initial Proof

The initial proof command renders the Nocturne `observatory_night` canary using
`sslab render`, compares it with the committed canary capture using
`sslab compare --class exact`, and writes an adapter receipt.

This is deterministic proof-kernel evidence. It is not a public compatibility
certification and not a visual-artistic acceptance decision.

The `audit` command records binary facts only. It is informational by default
and does not certify compatibility.
