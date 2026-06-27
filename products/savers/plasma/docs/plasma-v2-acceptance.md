# Plasma v2 Acceptance

Plasma v2 is acceptable only when it behaves like a controllable visual
instrument.

## Direct Control Bar

Each first-class Basic control must be one of:

- plan-visible
- render-visible
- intentionally structural
- unsupported and hidden

Decorative sliders are not acceptable.

## Initial Visual Bar

Before stable promotion, Plasma v2 must show that:

- changing field family changes field structure;
- changing scale changes structure size;
- changing complexity changes modulation density;
- changing speed changes temporal motion;
- changing warp bends the field;
- changing feedback changes persistence;
- changing material changes color response;
- changing contrast and brightness is obvious but bounded;
- the migrated `plasma_lava` default looks intentional in preview and
  fullscreen-oriented captures;
- GDI remains attractive enough to be the baseline;
- GL11 is optional and not a hidden minimum.

## Proof Bar

The first proof-bearing implementation must include deterministic checks for:

- spec validation;
- spec token mapping;
- legacy config migration;
- future plan validation;
- seed repeatability;
- control influence;
- resize and lifecycle behavior;
- GDI smoke;
- optional GL11 smoke.

Proof pass, visual acceptance, release promotion, publication, and compatibility
certification remain separate decisions.
