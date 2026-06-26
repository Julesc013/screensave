# Truth And Proof Baseline

This document records the active truth-reset milestone introduced after the
Plasma `U09` corrective recut.

## Purpose

ScreenSave already has a strong product doctrine and a deep evidence trail, but
the repository had too many overlapping answers to "what is current."

This baseline makes the current authority explicit:

- the released public artifact remains `c16-core-baseline`
- the development-head Plasma truth is the `U09` corrective recut
- the old MX queue files are lineage records, not the live execution queue
- compatibility language must use evidence classes instead of one broad
  undifferentiated support claim
- build and proof validators must check the current tree rather than stale
  source lists
- AIDE may govern development evidence in bounded operational mode, but
  products must not depend on it and source mutation remains blocked

## Canonical State File

[`../../PROJECT_STATE.toml`](../../PROJECT_STATE.toml) is now the machine-readable
project-state authority.

It records:

- public release
- development head
- active program
- release-candidate state
- queue authority
- current Plasma stable and experimental surfaces
- compatibility evidence policy
- build-profile posture
- achieved Proof Kernel v0 scope
- validator entry points

The product catalog entry point is [`../../catalog/products.toml`](../../catalog/products.toml).
Artifact-profile authority is [`../../catalog/artifact_profiles.toml`](../../catalog/artifact_profiles.toml).
That catalog is the first step toward generated build, packaging, Manager,
Anthology, Workbench, SDK, and documentation surfaces.

The AIDE Lite operational profile is [`../../.aide/pilot.toml`](../../.aide/pilot.toml).
It may coordinate WorkUnits and evidence packets, but it must not become a
runtime dependency, source-mutation authority, or automatic merge authority.

## Evidence Classes

Compatibility claims must use these classes:

| Status | Meaning |
| --- | --- |
| `certified` | Executed successfully on the named OS or hardware profile with stored evidence. |
| `binary-audited` | PE headers, architecture, subsystem, imports, runtime dependencies, and required APIs pass the named profile, but the artifact has not been run there. |
| `buildable` | A reproducible build exists for the named profile, without a binary or runtime support claim. |
| `targeted` | Architectural goal only; not a public support promise. |
| `experimental` | Available with known limitations or incomplete proof, and not silently promoted into Core support. |
| `unsupported` | Known not to work or deliberately excluded. |
| `retired` | Preserved for historical reference, but no longer maintained as current support. |

The broad Windows band remains the preservation target.
It is not automatically certified for every current artifact, and only
`certified` should read as ordinary public support language.

## Queue Reconciliation

The `.codex/queues/wave0.toml` through `.codex/queues/wave5.toml` files are now
marked `historical-superseded`.

They remain useful for lineage, owner boundaries, and promotion doctrine, but
they no longer answer the live execution question after `U09`.

## Immediate Proof Surface

The current captured evidence for this milestone is:

- [`../../validation/captures/truth-proof-baseline/README.md`](../../validation/captures/truth-proof-baseline/README.md)
- [`../../validation/captures/truth-proof-baseline/pe-audit-current.txt`](../../validation/captures/truth-proof-baseline/pe-audit-current.txt)
- [`../../validation/captures/proof-kernel-v0/nocturne/proof.json`](../../validation/captures/proof-kernel-v0/nocturne/proof.json)

The truth-baseline PE audit is intentionally allowed to record violations.
The proof-kernel Nocturne evidence is deterministic visual evidence, not
compatibility certification. It now records the compiled product-session runner,
canonical RGBA8 hash source, clean baseline provenance, implementation source
digests, and lifecycle evidence through the same proof-kernel surface.

## Current Exit Condition

The active exit command is:

```powershell
python tools\scripts\check_project_state.py --summary
python tools\scripts\check_catalog_profiles.py
python tools\scripts\check_aide_pilot.py
python tools\scripts\check_proof_kernel.py
python tools\scripts\check_project_adapter.py
```

That command must be able to report what is released, what is current in
development, what queue is authoritative, and what compatibility evidence policy
governs public claims.

## Scope Boundary

This milestone does not build Workbench, refactor the portable semantic core,
rewrite Plasma, ship a new Core release, or certify broad OS compatibility.

It creates the authority and proof spine those later changes must use.
