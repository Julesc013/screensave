# MX Owner Map

This document freezes the initial ownership map for the post-PL program.

## Owner Tags

| Tag | Meaning | Default Use |
| --- | --- | --- |
| `[P]` | Plasma-owned | product behavior, product proof, product curation, product identity |
| `[S]` | System-owned | shared contracts, shared environment/proof surfaces, companion/system integration |
| `[H]` | Hybrid | governance, cross-boundary review, or work that intentionally spans product and shared decisions |

## Owner Assignment Guidance

- Use `[P]` when the meaning is still uniquely Plasma's.
- Use `[S]` when the work is already a real shared ScreenSave concern.
- Use `[H]` only when both sides are materially in scope and the work should not be split yet.
- Do not use `[H]` to hide a vague scope boundary.
- If a hybrid epic splits cleanly into one shared concern and one Plasma-local concern, split it.

## Initial Epic Ownership Map

| Epic | Owner | Title | Why This Owner |
| --- | --- | --- | --- |
| `MX00` | `[H]` | Master control plane freeze | Governance, wave admission, queue doctrine, and proof mapping cross product and system boundaries. |
| `SY10` | `[S]` | Shared proof and environment baseline | Owns shared system-level proof gaps, environment constraints, and non-product-specific follow-on. |
| `PX10` | `[P]` | Plasma proof hardening and caveat baseline | Owns the first Plasma-local caveat burn-down and product-facing proof posture. |
| `PX11` | `[P]` | Plasma transition coverage hardening | Transition breadth is currently a Plasma-local product concern. |
| `SY20` | `[H]` | Conditional shared contract uplift | Only queues if Wave 1 proves a real cross-product or platform-law need. |
| `PX20` | `[P]` | Plasma promotion review and authoring boundary freeze | Promotion review starts from the actual Plasma product and its proof surfaces. |
| `PX30` | `[P]` | Pack provenance and authorable sets and journeys | Authoring and provenance stay Plasma-local until reuse is proven. |
| `PX31` | `[P]` | Minimal Plasma Lab and validator/comparer core | The first lab surface is product-owned, not suite-owned. |
| `PX32` | `[P]` | Plasma selection intelligence | Selection intelligence starts from Plasma's current content and routing model. |
| `PX40` | `[P]` | Output, treatment, field, and dimensional expansion | Product language expansion remains Plasma-owned. |
| `SY40` | `[H]` | Shared ops and integration surfaces | Shared tooling and integration only become relevant after product surfaces mature. |
| `PX41` | `[P]` | Plasma curation and provenance operations | Product curation, provenance, and community discipline remain Plasma-owned first. |
| `PX50` | `[H]` | Final promotion review and stable recut | Final stable-recut decisions cross product posture, proof posture, and master release truth. |

## Promotion Rules From Plasma-Local To Shared

Promote a Plasma-local concern into `SY` only when:

- at least two real products need the same stable abstraction, or
- the work clearly belongs to a general platform, validation, or companion contract

Do not promote solely because:

- Plasma reached the need first
- the implementation looks reusable in theory
- one product would like a cleaner abstraction
- a control or validator sounds suite-like before reuse is proven

## Practical Boundary Examples

- Keep transition-pair coverage, Plasma content provenance, Plasma Lab behavior, and Plasma selection logic in `[P]` unless reuse is proven.
- Keep shared proof-policy updates, environment contracts, and companion integration contracts in `[S]` once they are genuinely shared.
- Use `[H]` for queue doctrine, final promotion review, and any conditional shared uplift that must explicitly decide whether shared promotion is justified.
