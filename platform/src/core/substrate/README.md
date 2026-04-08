# Renderer Substrate

Purpose: private renderer-substrate helpers that sit beneath the stable public renderer contract.
Belongs here: backend selection, backend registry and loader logic, internal band routing, present-path policy, capability capture, and future optional service seam defaults and registries.
Does not belong here: product scene logic, public API definitions, or backend implementation detail that still belongs inside a specific renderer subtree.
Type: private runtime implementation.

`SX01` introduces this directory so the platform can grow a longer-lived substrate without forcing savers or public headers to adopt backend-private concepts directly.

Current contents:

- backend identity and band mapping helpers
- backend capability capture and failure bookkeeping
- backend registry and selection scaffolding
- routing-policy evaluation that turns requested, minimum, preferred, selected, and degraded outcomes into one central flow
- present-path routing helpers
- typed private image, text, present-policy, and backend-family seam helpers
- a private service registry that binds those seams beside the active backend substrate
