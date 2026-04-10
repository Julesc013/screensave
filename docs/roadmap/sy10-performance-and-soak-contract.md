# SY10 Performance And Soak Contract

This document freezes the shared vocabulary for performance, startup, and soak evidence in the post-PL program.

It standardizes how later epics describe evidence quality.
It does not create shared hard numeric targets.

## Purpose

Use this contract when a later epic needs to describe:

- startup behavior
- session-compilation behavior
- bounded runtime responsiveness
- repeated restart or soak behavior
- whether the repo has measured numbers or only qualitative evidence

## Shared Vocabulary

Use these shared terms when they are relevant:

| Term | Meaning |
| --- | --- |
| `startup` | process, module, and early session start behavior before steady-state running |
| `session_compile` | config, content, settings, and lane decisions resolving into a runnable session or plan |
| `runtime_envelope` | bounded supportable statement about steady-state behavior during the exercised run scope |
| `restart_soak` | repeated create, run, destroy, and recreate behavior over a bounded iteration count |
| `transition_soak` | repeated transition stepping over a bounded supported subset |
| `long_run_soak` | materially longer unattended or sustained run evidence when it actually exists |

These names describe proof shape.
They do not imply that every epic must produce every class of evidence.

## Evidence Classes

Later product or shared notes should classify performance evidence with these terms:

| Evidence Class | Meaning |
| --- | --- |
| `measured` | concrete numeric measurements were taken and recorded with units and environment context |
| `bounded_qualitative` | current supportable claim is descriptive and scoped, not numeric |
| `unknown` | the repo does not yet have supportable evidence for the named question |

Rules:

- use `measured` only when actual numeric data is captured in the repo
- use `bounded_qualitative` when the repo has truthful run evidence but not hard numbers
- use `unknown` instead of pretending a stronger claim exists

## Naming Conventions For Proof Notes

Proof notes and captures should make it easy to see which support question they answer.

Prefer naming and headings that separate:

- startup
- session compile
- runtime envelope
- restart soak
- transition soak
- long-run soak

Examples only:

- `classic_gl11 startup`
- `transition_subset transition_soak`
- `premium_gl46_heightfield runtime_envelope`

Products may add more specific labels on top of these shared names, but should not replace the shared names with vague local wording.

## Reporting Rules

When using `measured`, record:

- the metric name
- the unit
- the environment or machine context
- the capture or note that contains the measurement

When using `bounded_qualitative`, record:

- the exact run scope
- the bounded claim being made
- the current caveat or uncertainty

When using `unknown`, record:

- what question remains open
- why the repo cannot answer it yet

## Hard Numbers Are Out Of Scope For SY10

`SY10` does not create:

- shared frame-time SLAs
- shared memory ceilings
- shared startup-time guarantees
- product promotion thresholds

Those require product-local evidence and later promotion review.

## What Remains Product-Local

The following stay product-local:

- exact metric thresholds
- exact soak iteration counts required for that product's support posture
- exact transition-cost expectations for one product's content and lane mix
- stable-promotion decisions that depend on those metrics

Products should use the `SY10` evidence classes and envelope names, then add product-local thresholds only when their own proof genuinely supports it.
