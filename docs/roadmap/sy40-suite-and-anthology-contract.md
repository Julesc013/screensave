# SY40 Suite And Anthology Contract

## Purpose

This document freezes the minimum shared Wave 4 contract that is genuinely shared now between standalone savers, the `anthology` meta-saver, and the `suite` companion app.

It is intentionally thin.
It defines role boundaries and exposure vocabulary only.
It does not define cross-product runtime behavior.

## Why This Contract Is Shared Now

This contract is shared now because more than one real product already needs the same stable outer meaning:

- standalone savers expose identity, defaults, and stable-versus-experimental posture
- `anthology` consumes saver identity and decides whether a saver belongs in its bounded cross-saver surface
- `suite` consumes saver identity, metadata, presets, themes, and settings handoff posture

Without a thin shared contract here, the repo would drift into duplicated or incompatible vocabulary across saver docs, `anthology`, `suite`, and the SDK.

## Exact Domains Included In Shared Scope

The shared scope in this document is limited to:

- role boundaries among standalone saver, `anthology`, and `suite`
- shared exposure vocabulary for product-owned pools and the read-only projection of those pools into higher-level consumers
- shared visibility vocabulary for `stable` versus `experimental`
- shared outer wording for `featured` and `curated` when those labels are surfaced by a product

This document deliberately defines words and boundaries.
It does not define product behavior.

## Exact Domains Excluded From Shared Scope

The following areas remain product-local and are not promoted by this contract:

- cross-product selection algorithms
- cross-product favorites, exclusions, or weighting semantics
- saver-local presets, themes, sets, journeys, and compatibility rules
- anthology runtime orchestration policy beyond its current product-owned behavior
- suite browsing, filtering, or ranking policy beyond its current app-owned behavior
- automation or control bridge behavior
- provenance, trust, vetting, rating, or community metadata

## Role Boundaries

| Role | Owns | Must Not Claim |
| --- | --- | --- |
| Standalone saver | product identity, presets, themes, local pools, local curation, stable-versus-experimental product truth, local projection rules | suite-wide favorites law, cross-product curation law, shared automation law |
| `anthology` | cross-saver playback as a product in its own right, saver-level inclusion, saver-level favorites and weights, meta-saver selection policy | saver-local preset or theme law, product-local curation law, shared suite governance |
| `suite` | browsing, preview, settings handoff, saver metadata presentation, bounded configuration workflow | saver-local playback law, cross-saver weighting law, product-local selection law |

`anthology` and `suite` consume product exposure.
They do not replace the product-owned meaning of that exposure.

## Shared Vocabulary For Projection And Exposure

| Term | Shared Meaning | Explicitly Not Implied |
| --- | --- | --- |
| Product-owned pool | A product-local collection such as presets, themes, packs, sets, or journeys that the owning product defines | shared selection law or shared persistence |
| Exposed pool | The subset of a product-owned pool that a product intentionally makes visible to `anthology`, `suite`, the SDK, or later consumers | automatic inclusion in every consumer |
| Projection | A read-only mapping from product-owned meaning into consumer-visible metadata | a shared runtime algorithm or shared storage format |
| Stable visibility | The exposed subset that is truthful to the product's current stable posture and may be shown by default | promotion of product-local semantics into shared behavior |
| Experimental visibility | The exposed subset that exists but must remain opt-in and clearly marked experimental | silent inclusion in default stable browsing or playback |
| Featured exposure | A product-owned recommendation marker that a higher-level consumer may display as a label | shared ranking, weighting, or favorites law |
| Curated exposure | A product-owned editorial grouping or recommendation label | shared curation behavior, shared moderation, or suite-wide collection law |

## How Products Such As Plasma Should Consume It

Products such as Plasma should:

- keep presets, themes, sets, journeys, favorites, exclusions, and local curation semantics product-owned
- use the shared terms in this document when exposing product-owned pools upward to `anthology`, `suite`, or the SDK
- keep `stable` and `experimental` visibility aligned with the product's real release posture
- treat `featured` and `curated` as labels on exposed product-owned meaning, not as shared cross-product behavior

## Explicit Statement Of What Remains Product-Local

This document does not create shared favorites behavior.
It does not create shared projection files.
It does not create shared cross-saver weighting or ranking behavior.
It does not create shared curation, provenance, or community law.

Those concerns remain product-local for `PX41` unless a later review proves a broader shared need.
