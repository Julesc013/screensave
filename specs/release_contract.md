# Release Contract Specification

This document is normative.

## Releasable Saver Criteria

A saver is releasable only when all of the following are true:

- It builds into the intended product artifact for the series scope.
- It supports the documented host modes required for that product class.
- It has default metadata, default config, and at least one validated presentation path.
- It has smoke-test evidence appropriate to the current stage.
- Its compatibility claims are backed by recorded validation notes or captures.

## Versioning Expectations

Versioning details may be refined later, but a releasable product must have:

- A stable product identity.
- A visible version string or equivalent release identifier.
- Changelog coverage for the work that changed the released behavior.

Shared platform changes that materially affect products must also be traceable.

## Smoke-Test Expectations

At minimum, release readiness requires smoke validation for:

- Startup in the intended mode.
- Clean shutdown.
- Basic rendering on the required compatibility band or public renderer tier.
- Configuration handling where the product supports configuration.
- Preview behavior where the product claims preview support.

The exact test harness may change, but the obligation to produce evidence does not.

## Product Metadata Expectations

Each releasable product is expected to define:

- Stable product name and identifier.
- Product classification such as saver, app, or SDK asset.
- Compatibility notes relevant to the product.
- Default preset or default operating profile.

Later series may formalize metadata files, but the conceptual requirement exists now.

## Compatibility Evidence Expectations

Compatibility claims must be supported by evidence in `validation/` and `tests/` as later series establish those flows.

Evidence may include:

- Smoke test notes.
- OS matrix notes.
- Captures and screenshots from real runs.
- Known limitation notes for unsupported or degraded paths.

Fake evidence is never acceptable.

## Definition Of Done For A Product Series

A product-focused series is done only when:

- Product behavior matches the series scope.
- Platform and product docs are updated.
- Validation evidence exists for the behavior claimed.
- Remaining limitations are recorded honestly.
- The product can be handed to the next series without hidden contract gaps.
