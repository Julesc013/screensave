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
- private RGBA8 surface contract
- minimal software/reference renderer
- fixed-step headless canary host
- `sslab` command surface
- exact, tolerant, perceptual, and observational comparison classes
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

## Non-Goals

Proof Kernel v0 does not:

- remove Win32 types from public headers
- port every saver
- create a public runtime plugin ABI
- certify any operating system
- implement Workbench UI
- generate new saver C code from prompts
- make AIDE mandatory
