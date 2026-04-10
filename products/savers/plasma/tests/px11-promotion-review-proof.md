# PX11 Promotion Review Proof

## Purpose

This note records the evidence inputs used for stable-promotion review 1 and the uncertainties that prevented promotion.

## Evidence Inputs Actually Used

- `products/savers/plasma/docs/pl14-stable-vs-experimental.md`
- `products/savers/plasma/docs/pl14-release-cut.md`
- `products/savers/plasma/docs/pl14-ship-readiness.md`
- `products/savers/plasma/docs/px10-support-hardening.md`
- `products/savers/plasma/docs/px10-hardware-proof.md`
- `products/savers/plasma/docs/px10-performance-and-soak.md`
- `products/savers/plasma/docs/px11-transition-coverage.md`
- `products/savers/plasma/docs/px11-transition-compatibility-matrix.md`
- `products/savers/plasma/tests/px11-transition-proof.md`
- `validation/captures/pl13/benchlab-plasma-journey-gl11.txt`

## Validated Versus Partial Versus Blocked Inputs

Validated:

- the stable default identity is still unchanged
- the transition engine still has real direct, bridge, fallback, hard-cut, and reject paths
- the product-local transition compatibility graph is clearer than before

Partial:

- transition proof is stronger but still bounded
- richer-lane bridge paths are clearer but still optional
- lower-band fallback truth is clearer but still not broadly rerun across hardware

Blocked:

- no fresh compiled PX11 rerun in this checkout
- no broader cross-hardware or alternate-driver transition sweep
- no fullscreen `.scr` host transition proof beyond prior bounded evidence

## Remaining Uncertainty

The main uncertainties that affected the review are:

- rerun breadth
- hardware breadth
- host breadth
- the absence of numeric transition-cost evidence

## Effect On The Promotion Decision

These uncertainties mean `PX11` cannot justify a stable promotion.
The review therefore records `promote nothing` as the truthful result.
