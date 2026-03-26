# Renderer Contract Specification

This document is normative.

## Goals

The renderer layer exists to let saver products target one stable visual contract while the platform selects the concrete backend.

The renderer contract must:

- Preserve a universal path on machines that only have GDI available.
- Permit optional acceleration through OpenGL 1.1 when available.
- Allow later enhanced backends without forcing them into the universal baseline.

## Capability Classes

The repository uses three conceptual renderer classes:

- `universal_gdi`: always required and always supported by baseline products.
- `optional_gl11`: available only when OpenGL 1.1 initialization succeeds at runtime.
- `optional_gl_plus`: reserved for later capability-gated work.

Products may advertise use of enhanced classes, but baseline saver products must always define a valid `universal_gdi` behavior.

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

## Reserved GL-Plus Backend

`gl_plus` is reserved for later enhanced rendering work.
It may add broader feature coverage, but only when all of the following are true:

- Runtime capability detection succeeds.
- The product explicitly supports the higher tier.
- The product does not thereby weaken the universal baseline required for the same product class.

## Fallback Rules

Fallback behavior is part of the contract:

- If a requested higher tier is unavailable, the platform selects the highest available compatible lower tier.
- Products must degrade behavior intentionally instead of failing unpredictably.
- If a product cannot offer a valid universal path, it must be classified explicitly later as outside the universal saver set.

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
