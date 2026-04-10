# SY10 Hardware Proof Contract

This document freezes the shared hardware-proof contract for post-PL work.

It standardizes how later epics describe machine scope, environment scope, and proof status.
It does not define product-specific coverage obligations.

## Purpose

Use this contract when recording multi-machine, multi-environment, or hardware-sensitive evidence for savers, host behavior, and renderer routing.

The goal is honest comparison across proof notes without pretending that all products share the same coverage burden.

## Shared Matrix Vocabulary

Shared matrix entries should use these fields when they are relevant:

| Field | Meaning |
| --- | --- |
| `surface` | the product, host, or contract surface being judged |
| `environment` | the machine and software environment where the evidence was obtained |
| `mode_or_scope` | fullscreen, preview, BenchLab report mode, smoke, soak, or other bounded run scope |
| `request` | requested renderer, presentation, or support condition when the request matters |
| `resolved` | actual renderer, host mode, or support condition exercised |
| `topology` | current monitor layout or topology posture when display layout matters |
| `status` | canonical SY10 proof status bucket |
| `evidence_ref` | primary repo-local note, capture, or proof artifact |
| `notes` | truthful interpretation and current caveat text |

Not every matrix needs every field.
Use the smallest truthful set that still explains the evidence clearly.

## Machine And Environment Fingerprint Guidance

When a proof note or capture claims hardware-sensitive behavior, record the environment with these fields when available:

| Field | Guidance |
| --- | --- |
| `os_name` | human-readable Windows edition or environment name |
| `os_version` | version label such as `2009` when known |
| `os_build` | concrete build number when available |
| `architecture` | runtime or machine architecture |
| `gpu_adapter` | adapter name actually seen during proof |
| `driver` | renderer or driver string when the proof depends on it |
| `host_path` | BenchLab, preview, fullscreen `.scr`, or another real host path |
| `monitor_topology` | single monitor, virtual desktop span, or another truthful topology descriptor |

Guidance:

- record what the run actually used, not what the machine could theoretically use
- omit fields only when the run genuinely did not surface them
- keep environment text human-readable and comparable across notes

## Canonical SY10 Proof Status Buckets

Future post-PL shared and product-local matrices should prefer these status buckets:

| Status | Meaning |
| --- | --- |
| `validated` | direct current evidence exists in the repo for the named surface and scope |
| `partial` | direct evidence exists, but only for a bounded subset or bounded environment slice |
| `blocked` | the repo cannot currently obtain the needed evidence because the required environment is missing |
| `not_tested` | no current direct run exists for this cycle or epic |

Historical note:
older `SX` and `SS` notes use earlier status wording such as `passed` or `Tested / Pass`.
Those notes remain valid historical evidence and do not need to be rewritten.
`SY10` only freezes the preferred post-PL status language going forward.

## Artifact Naming And Reporting Guidance

`SY10` does not create a new shared capture framework.
It freezes simple naming expectations that match the current repo:

- captures live under `validation/captures/<epic-id>/` when later epics create new deterministic evidence
- filenames should preserve producer, product or surface, and scenario clarity
- proof notes should preserve exact commands, run scope, and caveat text beside the evidence

Preferred capture-name shape:

- `<tool>-<surface>-<scenario>.txt`
- `<tool>-<surface>-<scenario>.md` when the artifact is narrative rather than report text

Examples only:

- `benchlab-plasma-gl11.txt`
- `benchlab-gallery-auto.txt`
- `support-matrix.md`

The point is human-readable traceability, not a rigid new schema.

## Shared Matrix Dimensions

When later epics describe hardware breadth, prefer these dimensions:

- operating system and build
- GPU or adapter family
- driver string or driver family when relevant
- monitor topology
- requested lane or backend
- resolved lane or backend
- proof status

Do not add product-local content dimensions to the shared matrix unless they are genuinely shared support concerns.

## What Remains Product-Local

The following stay product-local:

- exact preset, theme, journey, or transition pair coverage
- exact dimensional or presentation coverage obligations
- per-product pass criteria for visual correctness
- promotion decisions based on that product's support posture

Product-local work should consume this contract, then add its own coverage logic on top.

## Non-Normative Examples

Shared example:

- host fullscreen routing on `gdi` versus `gl11`
- multi-monitor virtual-desktop fullscreen behavior
- driver-sensitive proof that a requested lane degraded honestly

Product-local example:

- whether a specific Plasma transition pair is stable-worthy
- whether a specific Plasma journey set has enough soak coverage

Those remain product-local because they are not yet shared platform law.
