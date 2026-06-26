# Proof Bundle Contract v1

Status: active contract for normalized proof receipts.

## Purpose

Proof Bundle v1 separates mechanical evidence from human or release decisions.
It exists so CI, Workbench, AIDE, and release tooling can share one result
shape without implying that a successful command equals compatibility
certification, artistic acceptance, or release promotion.

## Required Top-Level Fields

- `proof_schema`: must be `proof-bundle-v1`.
- `status`: aggregate bundle status, one of `pass`, `fail`, `blocked`, or
  `informational`.
- `source`: source revision and dirty state.
- `subject`: product, profile, artifact, or receipt being proved.
- `inputs`: references to source proof, comparison, audit, build, or adapter
  receipts used to construct the bundle.
- `result_axes`: the independent result axes below.
- `claim_boundary`: explicit statement of what this bundle does not certify.
- `limits`: known limitations and deferred evidence.

## Result Axes

Every bundle must include these axes:

| Axis | Meaning |
| --- | --- |
| `execution` | The product, runner, build, or command executed. |
| `capture` | Visual or binary outputs were produced and hashed. |
| `comparison` | Captures or artifacts were compared under a declared class. |
| `lifecycle` | Create, resize, step, render, destroy, or equivalent lifecycle evidence. |
| `performance` | Frame-time, memory, handle, soak, or resource evidence. |
| `portable_v2_equivalence` | v1/v2 deterministic equivalence evidence for named portable v2 canary profiles. |
| `artifact_audit` | PE or package audit facts for named artifact profiles. |
| `compatibility` | Evidence class supported by the mechanical facts. |
| `artistic_review` | Human visual-quality acceptance or explicit not-reviewed status. |
| `release_promotion` | Whether this evidence promotes a public release claim. |

Each axis has:

- `status`: `pass`, `fail`, `blocked`, or `informational`.
- `evidence_refs`: files, hashes, or receipts supporting the axis.
- `summary`: concise human-readable statement.

Additional axis-specific fields may exist, but the required axes must remain
present so consumers can distinguish mechanical proof from product approval.

## Accepted Evidence Inputs

The normalizer may consume these ScreenSave-owned receipt classes:

- proof receipts from fixed-step render or adapter proof commands;
- comparison receipts from exact, tolerant, or other declared comparison
  classes;
- PE or package audit receipts tied to named artifact profiles;
- build receipts with source, toolchain, artifact, and audit facts;
- adapter receipts that preserve the invoked fixed project capability;
- lifecycle receipts for create, resize, step, render, and destroy evidence;
- performance receipts for frame timing, resource, handle, memory, or soak
  evidence.
- portable v2 equivalence receipts for the fixed Nocturne/Ricochet canary
  matrix.

If an optional receipt is not supplied, its axis remains `informational` or
`blocked` according to the axis semantics. Supplying lifecycle or performance
evidence does not change the artistic-review, compatibility-certification, or
release-promotion boundaries.

## Compatibility Boundary

`artifact_audit.status = pass` or `informational` does not certify an operating
system. In short: a proof bundle does not certify an operating system unless
the compatibility axis records matching runtime evidence for that environment.
A PE audit can support `binary-audited` evidence only when it is tied to a named
artifact profile and exact artifact set. Runtime execution on a named
environment is still required for `certified`.

## Artistic And Release Boundary

`comparison.status = pass` does not mean the saver looks good.
`proof_schema = proof-bundle-v1` does not mean a release is approved.

The `artistic_review` and `release_promotion` axes must remain explicit even
when they are `blocked` or `informational`.

## Relationship To v0

Proof Bundle v0 remains the raw Proof Kernel v0 Nocturne canary output.
Proof Bundle v1 is the normalized evidence envelope that may wrap v0 proof,
comparison receipts, PE audits, build receipts, lifecycle receipts,
performance receipts, and future Workbench/AIDE evidence.

Type: versioned proof contract.
