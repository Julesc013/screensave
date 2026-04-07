# Architecture Docs

This directory explains how the platform is intended to fit together.

Belongs here:
- Plain-language overviews.
- Layer boundaries.
- Rationale for stable repository structure.

Does not belong here:
- Normative requirements better placed in `specs/`.
- Product-specific implementation notes.

Type: explanatory architecture.

## Current Entry Points

- [overview.md](./overview.md) explains the shared-platform versus product split and the current post-`C16` architectural posture.
- [layering.md](./layering.md) explains the public-versus-private boundary, including the `SX` rule that the public renderer contract remains narrower than the private substrate and that the first real substrate split now lives under `platform/include/screensave/private/` and `platform/src/core/substrate/`.
