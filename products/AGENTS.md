# AGENTS.md

This file applies to `products/`.

- Keep each product subtree self-owned and identifiable.
- Product-local manifests, presets, themes, and defaults belong with the owning product once introduced.
- Shared code stays out of `platform/` until at least two products need the same stable abstraction.
- Do not let one product's needs silently redefine the platform contract.
- When a product gains new compatibility or validation requirements, update its docs and related validation notes in the same series.
