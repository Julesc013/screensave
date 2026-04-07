# Renderer Contract Specification

This document is normative.

## Goals

The renderer layer exists to let saver products target one stable visual contract while the platform selects the concrete backend.

The renderer contract must:

- Preserve a universal path on machines that only have GDI available.
- Permit optional acceleration through OpenGL 1.1 when available.
- Allow later enhanced backends without forcing them into the universal baseline.
- Remain a stable public contract even while richer substrate routing grows privately during `SX`.

## Renderer Tiers

The repository uses an explicit renderer tier ladder:

- `gdi`: always required and always supported by baseline products.
- `gl11`: available only when OpenGL 1.1 initialization succeeds at runtime.
- `gl21`: the first real later-capability OpenGL tier and the current real advanced backend.
- `gl33`: an explicit later-tier placeholder until a real backend exists.
- `gl46`: an explicit later-tier placeholder until a real backend exists.
- `null`: an emergency no-op fallback that keeps host lifecycles alive but does not redefine the product baseline.

Products may advertise use of enhanced tiers, but baseline saver products must always define a valid `gdi` behavior.

## Public Contract Boundary

This ladder is the public compatibility-facing renderer vocabulary.

During `SX`:

- richer backend kinds, internal bands, registries, and service seams may grow privately
- those private abstractions do not replace the public tier names
- requested and active public tier reporting remains part of the stable renderer contract
- `null` remains internal-only even when the private substrate beneath it evolves

See [render_bands.md](./render_bands.md) and [backend_policy.md](./backend_policy.md) for the private substrate rules that sit beneath this public contract.

## Required GDI Backend

The GDI backend is the floor for the entire project.

Products may assume the availability of:

- Window-relative drawing surfaces managed by the host.
- Basic raster composition and clearing.
- Palette-conscious color usage.
- Primitive 2D drawing and bitmap presentation.
- Frame-by-frame animation driven by host timing.

Products may not assume:

- Hardware acceleration.
- Large texture budgets.
- Shader stages.
- Persistent GPU resources.

## Optional GL11 Backend

The GL11 backend is the conservative accelerated tier.

It may expose:

- Double-buffered presentation where the host can provide it.
- Texture-backed drawing models appropriate to OpenGL 1.1.
- Additional blending and transform options that still map cleanly to the saver contract.

It must remain optional.
Failure to initialize GL11 must fall back to a supported lower tier without crashing the host.

## Optional GL21 Backend And Later GL Placeholders

`gl21` is the current real later-capability OpenGL tier.
Later `gl33` and `gl46` tiers may add broader feature coverage, but only when all of the following are true:

- Runtime capability detection succeeds.
- The product explicitly supports the higher tier.
- The product does not thereby weaken the universal baseline required for the same product class.

## Fallback Rules

Fallback behavior is part of the contract:

- If a requested higher tier is unavailable, the platform selects the highest available compatible lower tier.
- Automatic selection may walk `gl46 -> gl33 -> gl21 -> gl11 -> gdi -> null`, but `null` remains an internal safety fallback rather than a normal product capability band or release tier.
- Products must degrade behavior intentionally instead of failing unpredictably.
- If a product cannot offer a valid universal path, it must be classified explicitly later as outside the universal saver set.
- Private routing may use internal bands and backend descriptors, but that private machinery must not obscure the public fallback story.

## Feature Assumptions

Products in the universal saver set may assume:

- Stable frame timing input from the host.
- A drawable surface.
- Basic 2D composition primitives.
- Access to resolved config, theme, and seed data.

Products in the universal saver set may not assume:

- Hardware texturing.
- Multi-pass effects.
- Non-power-of-two textures.
- Shader programs.
- Advanced pixel pipelines.

The renderer contract exists to keep these assumptions explicit and reviewable.
