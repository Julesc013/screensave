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
- `profiles`: report fixed admitted proof profile keys.
- `validate`: run a fixed ScreenSave-owned validation tier.
- `build`: run a named fixed ScreenSave build profile.
- `render`: run the Proof Kernel v0 Nocturne canary render.
- `compare`: compare proof-kernel captures using `sslab`.
- `audit`: run the ScreenSave PE artifact audit and report binary facts.
- `proof`: run a fixed Nocturne, Ricochet, or Plasma preview catalog proof profile.
- `bundle`: normalize a fixed Nocturne, Ricochet, or Plasma preview catalog
  proof profile as Proof Bundle v1.
- `equivalence`: run the fixed Nocturne/Ricochet portable v2 equivalence proof.

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
- `screensave.profiles.read`
- `screensave.validation.t0`
- `screensave.validation.t1`
- `screensave.validation.t2`
- `screensave.build.windows-current-x86`
- `screensave.build.windows-current-tools`
- `screensave.proof.nocturne.render`
- `screensave.proof.capture.compare`
- `screensave.artifact.pe.audit`
- `screensave.proof.nocturne.reference-v0.v1`
- `screensave.proof.nocturne.reference-v0.v2`
- `screensave.proof.ricochet.reference-v1.v1`
- `screensave.proof.ricochet.reference-v1.v2`
- `screensave.proof.plasma-v2.reference-preview.v2`
- `screensave.proof.portable-v2.equivalence`
- `screensave.bundle.nocturne.reference-v0.v1`
- `screensave.bundle.nocturne.reference-v0.v2`
- `screensave.bundle.ricochet.reference-v1.v1`
- `screensave.bundle.ricochet.reference-v1.v2`
- `screensave.bundle.plasma-v2.reference-preview.v2`
- `screensave.bundle.portable-v2.equivalence`

Each capability has a declared argv shape, decoder schema, mutation policy,
output policy, timeout, and state probe. Changes to this file are project
contract changes and must be reviewed as such.

## Admission Model

The ScreenSave-side admission classes are:

- read-only fixed commands: `status`, `capabilities`, `catalog`, and fixed
  `validate --tier T0/T1/T2`
- contained generated-output commands: `build`, `render`, `compare`, `audit`,
  `proof`, `bundle`, and `equivalence`
- blocked worker sessions: source patching, data-pack worker proposals,
  autonomous coding sessions, automatic merge, and release promotion

This adapter admits fixed command execution only. It does not admit AIDE
worker-host sessions or generic command execution.

Validation tiers are fixed:

- `T0`: authority, contracts, docs, AIDE pilot, AIDE operational repair, and
  whitespace.
- `T1`: `T0` plus catalog, artifact-set, adapter, `libsslab`, proof-kernel,
  Proof Bundle v1, and Workbench shell checks.
- `T2`: `T0` and `T1` plus the portable v2 header seam and equivalence checks,
  wider local check-script gate, and fixed Nocturne/Ricochet v1/v2 profile
  proofs.

T3 is native and extended evidence. It remains an operator-scheduled promotion
or release gate and is not exposed as an AIDE fixed capability.

## Boundaries

The adapter may:

- invoke repository validators
- invoke fixed build profiles through `tools/buildctl/screensave_build.py`
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
- accept arbitrary compiler, linker, or MSBuild arguments from callers

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
The audit tool emits both a human-readable report and a JSON result. Missing
artifact roots or zero discovered PE artifacts produce `blocked`, not a passing
empty audit.

The `build` command accepts named `--profile` selections only. Initial profiles
are `windows-current-x86` and `windows-current-tools`; both route through the
fixed VS2022 Release Win32 build lane and then produce build receipts,
artifact-set manifests, and profile-aware PE audit facts. A dry-run build
receipt may prove the admitted command plan, but it is not build evidence.

## Initial Proof

The fixed profile proof commands call `tools/sslab/sslab.py proof --profile`
with `nocturne.reference.v0`, `ricochet.reference.v1`, or the v2-only
`plasma.v2.reference.preview`, plus a fixed `--path v1` or `--path v2`, then
write an adapter receipt plus artifact manifest. The bundle commands normalize
that profile proof through `tools/proofbundle/proofbundle.py normalize`. The
equivalence command delegates to `tools/sslab/sslab_equivalence.py` for the
fixed canary profile set only.

This is deterministic proof-kernel evidence. It is not a public compatibility
certification and not a visual-artistic acceptance decision.

The `audit` command and proof-receipt PE audit fields record binary facts only.
They are informational by default and do not certify compatibility.
