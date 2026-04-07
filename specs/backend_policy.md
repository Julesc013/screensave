# Backend And Service Policy Specification

This document is normative.

## Intent

This specification freezes the public-versus-private renderer boundary for the `SX` series.

The goal is to let the private substrate grow in a disciplined way without turning ScreenSave into a public multi-API middleware platform or weakening the compatibility-first baseline.

## What Remains Public

The following remain public doctrine during `SX`:

- the renderer ladder `gdi`, `gl11`, `gl21`, `gl33`, `gl46`, `null`
- the current narrow renderer contract under `platform/include/screensave/renderer_api.h`
- the current saver-facing contract under `specs/saver_api.md`
- the compatibility-first posture under `specs/compatibility.md`
- the classic Win32 `.scr` host model
- the rule that `gdi` is the guaranteed floor and `null` is internal only

The public surface must remain narrower and more stable than the private substrate.

## What Becomes Or Remains Private

The following are private substrate concerns during `SX`:

- backend kinds
- backend capability descriptors
- backend registries or loaders
- band-to-backend routing logic
- present-path internals
- backend-private state ownership
- future service seams such as image, text, or presentation support layers
- future optional non-GL implementation families

Private implementation detail belongs under `platform/src/` and related internal notes.
It does not belong in the public headers or product doctrine unless a later explicit spec promotes it.

## Backend-Kind Rules

- A backend kind is a private implementation identity, not a public renderer-tier name.
- Backend kinds may map to one public tier or one internal band today and a different private implementation later.
- Products outside the platform layer must not branch on backend-kind internals.
- BenchLab and diagnostics surfaces may expose backend detail for debugging, but diagnostics output does not redefine public doctrine.

## Service-Seam Rules

Future service seams are allowed only under these rules:

- they must remain private first
- they must remain dynamically optional
- they must not become mandatory baseline runtime dependencies
- they must not weaken `gdi` as the compatibility floor
- they must not silently change the public renderer ladder

Examples of future service seams may include image support, text support, presentation support, or other helper families.
Their existence does not make them public contracts automatically.

## Non-GL Family Restraint

`SX00` does not adopt D3D, D2D, or DWrite as public first-class renderer tiers or public first-class service families.

Later `SX` work may explore or scaffold non-GL families privately only when:

- the work stays behind the current public contract
- the work remains optional and dynamically gated
- the work does not weaken the lower-tier fallback walk
- the work does not imply that every saver must migrate

Any future public adoption of a non-GL family would require a later explicit doctrine update beyond `SX00`.

## Anti-Empire Rules

`SX` must not quietly become:

- a scene-graph rewrite
- a universal asset middleware layer
- a mandatory graphics-services stack
- a public backend-agnostic platform reset
- a forced migration campaign across the full saver line

New private abstractions must stay small, reviewable, and justified by real routing or backend pressure.

## Promotion Rule

A private abstraction may move into the public contract only when all of the following are true:

- it is stable across more than one real product need
- it has survived real fallback and compatibility pressure
- it no longer reads as backend-private glue
- a later explicit public spec promotes it intentionally

Until then, private remains private.
