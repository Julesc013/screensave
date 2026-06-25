# Proof Bundle Contract v0

Status: initial contract for future Lab Runner, Workbench, CI, and AIDE
evidence packets.

## Purpose

A proof bundle records enough information to explain what artifact was tested,
how it was selected, what environment ran it, what fallback decisions occurred,
and whether the result supports a compatibility claim.

Proof bundles are evidence. They are not marketing copy and they do not turn
failed or partial runs into passing support claims.

## Required Sections

- source commit and dirty status
- toolchain and build identity
- artifact path and hash
- artifact profile
- binary audit facts
- schema, ABI, pack, and proof versions
- product key and product version
- seed and timeline
- stored, resolved, and degraded configuration
- renderer, presenter, compute, surface, and host selections
- operating system, GPU, driver, monitor, and power/environment facts when available
- captures or capture references
- performance and resource metrics
- fallback and degradation reasons
- pass, fail, blocked, or informational status
- known limitations

## Status Vocabulary

| Status | Meaning |
| --- | --- |
| `pass` | The named proof objective passed with stored evidence. |
| `fail` | The named proof objective ran and failed. |
| `blocked` | The proof objective could not run because a dependency or environment was missing. |
| `informational` | The record captures facts without asserting pass/fail. |

## Claim Boundary

Only a proof bundle tied to a matching artifact profile may upgrade a public
claim. A binary audit without runtime execution may support `binary-audited`;
it must not become `certified`.

Adapter proof receipts may embed artifact-profile PE audit facts alongside
render and comparison evidence. Those facts keep the same boundary: they are
binary evidence, not operating-system certification and not artistic
acceptance.

## Minimal Text Form

The first implementation may be a deterministic text or TOML bundle emitted by
`sslab` or a validator. A later Workbench UI may present the same data, but the
stored proof remains data-first and reviewable.
