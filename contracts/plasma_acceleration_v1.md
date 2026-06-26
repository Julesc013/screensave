# Plasma Acceleration v1

Status: active product contract for the Plasma v2 stable-candidate wave.

This contract admits one narrow accelerated realization candidate for Plasma v2
without changing the software reference oracle, the portable v2 ABI, the
compatibility floor, or release status.

## Principles

- The Plasma v2 software reference is canonical.
- Acceleration is optional and capability-gated.
- If acceleration is unavailable, the product must degrade honestly to the
  software/GDI path.
- Acceleration must not change Plasma v2 product semantics.
- Acceleration evidence does not widen compatibility.
- Visual mismatch must be classified, not hidden.
- Performance benefit must be measured before promotion.
- Human review remains separate from deterministic proof.
- Stable release promotion remains blocked until a later release-readiness
  gate.

## First Candidate

The first admitted candidate is:

```text
plasma_v2_realization_gl11_candidate
```

This candidate is narrow on purpose. It may use GL11 as a presenter or
realization path when a host can provide it, but it must preserve the software
reference as the oracle and must fall back without changing the product claim.

## Comparison Classes

Comparison evidence uses these classes:

- `exact`: byte-for-byte RGBA equivalence.
- `tolerant-pixel`: bounded numeric pixel deltas with explicit thresholds.
- `observational`: classified evidence where exact pixel comparison is not the
  claim.
- `review-only`: human review evidence without deterministic pixel equivalence.

PAW-F starts with `exact` fallback-oracle evidence and tightly bounded
`observational` evidence for candidate review rows. It does not introduce a
general perceptual quality gate.

## Boundaries

Acceleration admission does not imply:

- stable release,
- final artistic acceptance,
- compatibility certification,
- all-renderer-lane stability,
- public SDK stability,
- executable plugin admission,
- AIDE product authority.
