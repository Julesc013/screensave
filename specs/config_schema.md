# Configuration Schema Specification

This document is normative.

## Purpose

ScreenSave needs one conceptual configuration model that can serve many savers and suite-level applications without turning every product into a special case.

Series 00 defines the boundary, not the final file format or registry implementation.

## Configuration Layers

Configuration is expected to resolve in layers:

1. Shared platform defaults.
2. Product defaults.
3. Named preset selection.
4. Theme selection.
5. User overrides.
6. Session-only runtime state that is not persisted as stable configuration.

Later implementation series may refine resolution details, but they must preserve the distinction between these layers.

## Presets

Presets are curated behavior bundles.
They are intended to change how a product behaves, not to identify the product itself.

Presets belong to the owning product unless they are later promoted into a shared suite-level concept.

## Themes

Themes are presentation-oriented selections such as palette, mood, or surface treatment.
They should avoid redefining the entire runtime model unless a later spec explicitly broadens their role.

Theme selection should remain separable from preset selection wherever practical.

## Defaults

Every product should define a stable default configuration.
That default must be valid on the universal baseline required for that product class.

Defaults should prefer predictable behavior and conservative resource assumptions.

## Persistence Boundaries

Shared configuration should contain only settings that genuinely belong across products or across multiple saver runs.

Shared configuration may include categories such as:

- preferred language-neutral behavior flags
- default duration or density classes
- shared presentation preferences
- platform-level fallback preferences

Product-local configuration should contain:

- product-specific animation parameters
- product-specific content selections
- product-local preset and theme identifiers

Session-only runtime state should not be persisted as durable user configuration unless a later series deliberately promotes it.

## Shared Versus Product-Local Keys

Promotion into shared config requires evidence of reuse and stable meaning across products.
A key must not move into the shared schema merely because one product needs it early.

The default rule is:

- shared when the concept is cross-product and stable
- product-local when the concept is product-specific or experimental

## Storage Non-Commitments

Series 00 does not commit the repository to one storage backend.
Registry, file-based, or hybrid persistence can be evaluated later as long as the conceptual boundary above remains intact.
