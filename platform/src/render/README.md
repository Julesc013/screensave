# Renderer Layer

Purpose: backend implementations behind the renderer contract and beneath the private substrate routing layer.
Belongs here: GDI, GL11, explicit higher GL tiers, and the internal null safety fallback.
Does not belong here: product-specific scene logic, public contract definitions, or direct packaging concerns.
Type: runtime implementation.

During `SX01`, backend creation, capability capture, and present-path ownership move upward into `platform/src/core/substrate/`.
The renderer subtrees remain the home for backend-specific state and implementation, not for public-facing routing policy.
During `SX02`, `platform/src/render/gdi/` becomes the hardened universal floor for surface ownership, present behavior, and diagnostics-friendly capture details without widening the public renderer contract.
During `SX03`, `platform/src/render/gl11/` becomes the hardened conservative compat lane with clearer WGL session ownership, present-path detail, and parity-first diagnostics above that floor.
During `SX04`, `platform/src/render/gl21/` becomes the first real hardened advanced lane with explicit private capability-bundle ownership, safer advanced-context lifecycle rules, and clearer degradation signals down to `gl11` and `gdi`.
