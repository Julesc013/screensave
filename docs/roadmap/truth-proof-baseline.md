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
- validator entry points

The product catalog entry point is [`../../catalog/products.toml`](../../catalog/products.toml).
That catalog is the first step toward generated build, packaging, Suite,
Anthology, Workbench, SDK, and documentation surfaces.

## Evidence Classes

Compatibility claims must use these classes:

| Status | Meaning |
| --- | --- |
| `certified` | Executed successfully on the named OS or hardware profile with stored evidence. |
| `binary-audited` | PE headers, architecture, subsystem, imports, runtime dependencies, and required APIs pass the named profile, but the artifact has not been run there. |
| `targeted` | Architectural goal only; not a public support promise. |
| `unsupported` | Known not to work or deliberately excluded. |

The broad Windows band remains the preservation target.
It is not automatically certified for every current artifact.

## Queue Reconciliation

The `.codex/queues/wave0.toml` through `.codex/queues/wave5.toml` files are now
marked `historical-superseded`.

They remain useful for lineage, owner boundaries, and promotion doctrine, but
they no longer answer the live execution question after `U09`.

## Immediate Proof Surface

The first captured evidence for this milestone is:

- [`../../validation/captures/truth-proof-baseline/README.md`](../../validation/captures/truth-proof-baseline/README.md)
- [`../../validation/captures/truth-proof-baseline/pe-audit-current.txt`](../../validation/captures/truth-proof-baseline/pe-audit-current.txt)

That audit is intentionally allowed to record violations.
The point is to make artifact truth visible before support claims are widened.

## Current Exit Condition

The active exit command is:

```powershell
python tools\scripts\check_project_state.py --summary
```

That command must be able to report what is released, what is current in
development, what queue is authoritative, and what compatibility evidence policy
governs public claims.

## Scope Boundary

This milestone does not build Workbench, refactor the semantic core, or change
runtime behavior.

It creates the authority and proof spine those later changes must use.
