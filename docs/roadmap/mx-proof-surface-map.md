# MX Proof Surface Map

This document records the current proof surfaces and the mandatory proof-update rules for later post-PL epics.

## Principle

No feature, contract slice, or promotion decision is done without proof-surface updates.

Code alone is not enough.
Docs alone are not enough.
BenchLab captures alone are not enough.

## Current Proof Surfaces Already Present

| Surface | Current Anchors | Role |
| --- | --- | --- |
| Shared substrate handoff | `docs/roadmap/pl00-substrate-handoff.md`, `validation/notes/sx09-substrate-validation-summary.md`, `validation/notes/sx09-tier-proof-matrix.md`, `validation/captures/sx09/` | Records the finished shared substrate contract that Plasma already inherited. |
| Plasma product doctrine | `products/savers/plasma/docs/pl00-constitution.md` through `pl14-ship-readiness.md` | Records implemented product law, release posture, and known limits. |
| Plasma product proof notes | `products/savers/plasma/tests/README.md` and per-phase proof notes under `products/savers/plasma/tests/` | Records the exact rebuild and proof commands that were actually run. |
| Plasma validation captures | `validation/captures/pl12/`, `validation/captures/pl13/` | Holds deterministic BenchLab reports for the implemented Plasma subset. |
| BenchLab runtime truth | Plasma-specific sections in the checked-in BenchLab captures | Records requested, resolved, degraded, clamp, content, transition, and settings truth. |
| Known limits and release cut | `products/savers/plasma/docs/pl13-known-limits.md`, `pl14-release-cut.md`, `pl14-stable-vs-experimental.md`, `pl14-ship-readiness.md` | Keeps current caveats, release posture, and stable-versus-experimental truth honest. |

## How The Proof Surfaces Relate

| Surface Type | What It Answers | Update Expectation |
| --- | --- | --- |
| docs | what the product or contract means | update when behavior, scope, or posture changes |
| tests and proof notes | what was run | update when commands, harness scope, or asserted invariants change |
| validation captures | what was observed in deterministic runs | update when new runtime truth is being claimed |
| BenchLab and reporting | what the running product reports | update when lane, degrade, selection, transition, presentation, or forcing truth changes |
| known limits | what is still bounded, unsupported, or blocked | update whenever a caveat changes state |
| release-cut docs | what is stable, experimental, or deferred | update when ship posture or promotion posture changes |

## Mandatory Later-Epic Update Surfaces

Every later epic must update:

- its controlling roadmap or product doc
- the relevant queue manifest state
- the relevant proof note or proof notes
- the relevant known-limit or risk surface

Feature-bearing or proof-bearing epics must also update:

- `validation/captures/<epic-id>/README.md`
- one or more deterministic captures or reports under `validation/captures/<epic-id>/`

Plasma-local epics must also update:

- `products/savers/plasma/docs/px00-feature-ledger.md`
- `products/savers/plasma/docs/px00-risk-register.md`

## BenchLab Reporting Rule

If a later epic changes any of the following, the BenchLab proof surface must be updated:

- lane selection
- degrade behavior
- selection state
- transition state
- presentation state
- forcing or clamp behavior

Requested versus resolved versus degraded truth must remain explicit.

## Release-Cut Rule

If a later epic changes anything that affects stable versus experimental posture or support claims, it must update the release-cut surfaces, not just the implementation notes.

That means:

- stable-versus-experimental docs
- known limits
- risk register
- any release-facing summary touched by the decision

## Done Rule

A later epic is not done if any of these are true:

- the code changed but the proof note did not
- the proof note changed but no actual capture or smoke evidence exists
- the capture changed but known limits were left stale
- the feature ledger or risk register still describes the old reality
- a stable or experimental claim changed without the release-cut surfaces being updated
