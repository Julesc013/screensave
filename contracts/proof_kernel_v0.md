# Proof Kernel v0

Status: active implementation milestone.

## Purpose

ScreenSave Proof Kernel v0 is the first ScreenSave-owned execution and proof
spine. It gives the project a deterministic way to produce and validate visual
facts before AIDE, Workbench, CI, or a human reviewer records those facts as
evidence.

AIDE may consume Proof Kernel output. AIDE must not invent ScreenSave product
truth.

## Components

Proof Kernel v0 consists of:

- canonical catalog authority
- generated catalog inventory
- private RGBA8 surface contract
- minimal software/reference renderer
- fixed-step headless canary host
- compiled Nocturne canary runner
- `sslab` command surface
- exact repeatability comparison
- proof-bundle v0 output

## Canary

The first canary is `nocturne`.

This is deliberate. Nocturne exercises a small set of primitives while avoiding
Plasma's larger semantic surface. The private software renderer provides:

- clear
- filled rectangle
- frame rectangle
- line/polyline

The initial `observatory_night` canary exercises:

- clear
- filled rectangle
- frame rectangle
- deterministic seed
- fixed resolution
- fixed timeline

## Exit Gate

Given the same product, preset, seed, resolution, frame count, and fixed delta:

- two headless runs produce the same capture hash
- the repeated captures compare exactly through `sslab compare`
- a proof bundle records source, runtime, seed, renderer, captures, and limits
- the existing Win32 `.scr` path remains unchanged
- no public compatibility claim is widened
- AIDE remains an optional evidence consumer

## Staging

Proof Kernel v0 is intentionally narrow:

- Nocturne
- generated catalog inventory
- fixed-step headless execution
- RGBA8 surface
- deterministic software realization
- compiled Nocturne canary runner
- frame capture
- exact repeatability comparison
- proof-bundle v0

Proof Kernel v1 is the target for:

- Ricochet or another stateful non-field canary
- tolerant and perceptual comparison gates
- lifecycle create, destroy, resize, and repeated-run checks
- frame-time, resource, handle, and soak evidence
- renderer comparison
- CI integration

Proof Kernel v2 is the target for:

- Plasma
- dense field semantics
- multi-renderer equivalence classes
- visual-quality metrics
- pack-generation qualification
- Workbench and compatibility-lab integration

## Non-Goals

Proof Kernel v0 does not:

- remove Win32 types from public headers
- port every saver
- create a public runtime plugin ABI
- certify any operating system
- implement Workbench UI
- generate new saver C code from prompts
- make AIDE mandatory

The compiled Nocturne runner is a proof-kernel tool. It does not create a
public runtime ABI, does not replace the standalone `.scr` host, and does not
certify operating-system compatibility.
