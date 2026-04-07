# Private Platform Headers

Purpose: private substrate headers that sit beneath the stable public `screensave/` contract.
Belongs here: backend kinds, internal render bands, backend capability state, loader and registry seams, present-path routing, and future optional service seams.
Does not belong here: product includes, release-facing contracts, or backend-specific declarations that still belong in one implementation subtree.
Type: private runtime implementation contract.

This tree exists so `SX01` can formalize richer renderer-substrate structure without widening the public API surface.

Rules:

- products and public headers stay on the public side
- private substrate detail stays here or under `platform/src/`
- new abstractions may be incomplete scaffolding, but they must stay integrated enough to reduce future churn
- private service seams remain optional and do not authorize new public runtime dependencies
