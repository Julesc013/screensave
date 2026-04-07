# Renderer Layer

Purpose: backend implementations behind the renderer contract and beneath the private substrate routing layer.
Belongs here: GDI, GL11, explicit higher GL tiers, and the internal null safety fallback.
Does not belong here: product-specific scene logic, public contract definitions, or direct packaging concerns.
Type: runtime implementation.

During `SX01`, backend creation, capability capture, and present-path ownership move upward into `platform/src/core/substrate/`.
The renderer subtrees remain the home for backend-specific state and implementation, not for public-facing routing policy.
