# AIDE To ScreenSave Integration Plan

Status: active supporting development plan.

Date: 2026-06-25.

Primary contract: [../../contracts/project_adapter_v0.md](../../contracts/project_adapter_v0.md).

Bridge profile: [../../.aide/project_bridge_profile.toml](../../.aide/project_bridge_profile.toml).

## Verdict

ScreenSave does not need to become an AIDE project. ScreenSave should expose a
narrow, proof-native project interface that AIDE is permitted to consume.

AIDE may coordinate bounded development activity around ScreenSave, record
attempts, index evidence, and present promotion proposals. AIDE must not own
ScreenSave visual semantics, compatibility claims, product identity, release
state, proof rules, native saver artifacts, or final artistic acceptance.

The integration thesis is:

```text
ScreenSave commands produce authoritative results.
ScreenSave proof bundles preserve domain meaning.
AIDE records and coordinates evidence.
Saver binaries never depend on AIDE.
```

## Command Class Split

There are two different integration classes.

| Class | Meaning | ScreenSave posture |
| --- | --- | --- |
| Deterministic fixed command | A registered command with fixed argv shape, schema, timeout, receipt, and no source mutation | Admitted through the ScreenSave project adapter |
| Worker session | A coding-agent or autonomous implementation loop that edits a workspace | Blocked until AIDE worker-host gates are accepted elsewhere |

Running `screensave_project.py proof` is not the same kind of operation as
giving Codex, Claude, Omnigent, or another worker an implementation session.

## Admitted Fixed Capabilities

The ScreenSave-owned fixed capability surface is:

```text
screensave.project.status
screensave.project.capabilities
screensave.catalog.read
screensave.validation.core
screensave.build.windows-current-x86
screensave.build.windows-current-tools
screensave.proof.nocturne.render
screensave.proof.capture.compare
screensave.artifact.pe.audit
screensave.proof.nocturne.exact
```

There must not be a generic capability such as:

```text
screensave.command
screensave.run
screensave.exec
```

The admitted capabilities are declared in
`tools/project_adapter/capability_bindings.json`, decoded through
`tools/project_adapter/receipt_schemas.json`, and constrained by
`tools/project_adapter/artifact_profile_audit_roots.json`.

The build capabilities route through `tools/buildctl/screensave_build.py`.
They accept named profiles only and do not expose arbitrary MSBuild or compiler
arguments. Dry-run build receipts may validate the command plan, but only a
non-dry-run receipt with artifact-set manifests and profile-aware PE audits is
build evidence.

## Hardened Adapter Requirements

Generated-output commands must:

- write only beneath `out/aide/screensave-project-adapter/invocations/`
- require a sanitized invocation id
- reject arbitrary absolute output paths
- limit compare inputs to approved ScreenSave capture/proof roots
- derive audit inputs from named artifact profiles
- emit typed refusal receipts
- include command-specific result schemas
- include source, executable, and argument-plan digests
- record before and after Git-state evidence
- enforce output, input, timeout, and quota limits
- write artifact manifests with paths, sizes, and SHA-256 digests

These are ScreenSave command semantics, not AIDE runtime semantics.

## Result Semantics

AIDE must preserve ScreenSave result semantics. It must not flatten all
successful command execution into product approval.

Examples:

```text
PE audit command returned zero
  != operating-system compatibility certified

Exact image comparison passed
  != artistic quality accepted

Proof bundle generated
  != product approved for release
```

The current proof capability records Nocturne render evidence, exact comparison
evidence, artifact-profile PE audit facts, and the claim boundary that binary
facts are not compatibility certification.

## Readiness Model

| Integration | ScreenSave-side status |
| --- | --- |
| Report-only AIDE pilot | ready-now |
| Pinned AIDE Lite profile | ready-now |
| Read-only deterministic commands | admitted-now |
| Fixed current-Windows build commands | admitted-now-contained |
| Contained generated proof commands | admitted-now-contained |
| Coding-agent data-pack proposal | later |
| Source patch preview/apply | substantially-later |
| Automatic product promotion or release | deliberately-excluded |

External AIDE-side runtime admission remains outside this repository. This
repository can expose a safe ScreenSave-owned bridge; it cannot certify an
external worker host.

## First Worker Canary

A ScreenSave worker canary remains blocked until all of these exist:

- accepted AIDE worker host
- trust and authorization enforcement
- disposable worker workspace
- durable worker evidence
- ScreenSave pack compiler
- VisualIntent or another bounded data schema
- proof-qualified generated output
- human promotion decision

The first worker task should generate a preset or data pack. It must not modify
renderer contracts, compatibility claims, native C source, release state, or
platform seams.

## Immediate Combined Milestone

The combined milestone is:

```text
compiled Nocturne proof path
+ hardened contained project adapter
+ pinned AIDE Lite profile
+ independently checked fixed-command bridge
```

This milestone supports useful automation without making AIDE a dependency,
weakening ScreenSave authority, or pretending the AIDE worker runtime is ready.
