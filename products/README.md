# Products

Purpose: all releasable product trees built on the shared platform.
Belongs here: saver products, suite applications, and SDK material.
Does not belong here: shared platform internals that belong in `platform/`.
Type: runtime and product-definition space.

## Current Product Roles

- `products/savers/` contains the real standalone saver products plus the real `_template` contributor starter saver. This is the primary product line that feeds `ScreenSave Core`.
- `products/apps/` contains real companion app products such as `benchlab` and `suite`. These remain separate from the primary saver ZIP.
- `products/sdk/` now contains the real contributor surface for saver authoring and data-pack authoring. It is contributor-facing only and not part of the primary end-user product.
