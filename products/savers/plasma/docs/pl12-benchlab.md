# Plasma PL12 BenchLab Integration

## Purpose

`PL12` makes Plasma's BenchLab surface real as a product-local reporting and forcing layer.
It does not change Plasma's default runtime identity.
It makes the already-implemented content, lane, transition, presentation, and settings truth exportable and selectively forceable for proof work.

## Relationship To PL00-PL11

- `PL00` through `PL02` fixed Plasma's preserved identity and classic obligations.
- `PL03` through `PL05` created and stabilized the engine core and truthful lower-band baseline.
- `PL06` through `PL10` made content, richer lanes, dimensional presentation, and transitions real engine concepts.
- `PL11` made layered settings surfaces and settings resolution real.
- `PL12` sits on top of those systems and exposes their resolved truth to BenchLab without redefining the normal saver path.

## What BenchLab Integration Means For Plasma

For Plasma, BenchLab integration means:

- the product can export a truthful resolved runtime snapshot instead of only generic saver facts
- BenchLab can request a bounded supported subset of Plasma-specific overrides
- requested state, resolved state, degraded state, and clamp results are reported distinctly
- textual overlay and report surfaces can describe what Plasma actually ran

BenchLab is still not the normal end-user settings surface.
`PL12` does not claim full validation sign-off, release readiness, or unrestricted forcing.

## Reporting Model

Plasma now reports BenchLab truth in three layers:

1. requested input
2. resolved runtime truth
3. degraded or clamped outcome when they differ

The product-local report and overlay are assembled from:

- compiled plan state
- resolved content selection state
- resolved transition state
- resolved settings state
- active BenchLab forcing state

This keeps the report grounded in the runtime session rather than in raw requested configuration alone.

## Forcing Model

`PL12` adds a bounded Plasma-owned forcing subset.
It is inactive by default and does not persist into normal saver runs.

Implemented forcing subset:

- preset override by key
- theme override by key
- preset-set override by key
- theme-set override by key
- journey override by key
- content filter override
- favorites-only override
- transitions enable/disable override
- transition policy override
- transition fallback override
- transition seed continuity override
- transition interval override
- transition duration override
- bounded presentation request override for `heightfield`

BenchLab still uses the shared generic `/renderer:` and `/seed:` controls for renderer request and deterministic seed request.
Plasma does not replace those shared controls with a separate private lane-forcing law.

Unsupported requests are not silently accepted.
They are clamped, downgraded, or ignored through the same validation path used by normal product config.

## Capture And Proof Hooks

The product now exposes:

- a product-local BenchLab forcing parser
- a product-local forcing-to-config bridge
- a product-local plan forcing pass
- a product-local runtime snapshot builder
- a concise overlay summary builder
- a detailed report section builder

BenchLab app integration uses those hooks only when the active saver is `Plasma`.
Other products do not inherit Plasma-specific reporting.

## Preserved Default And Classic Invariants

When BenchLab is inactive:

- product identity remains `Plasma`
- `Plasma Classic` remains preserved
- default preset remains `plasma_lava`
- default theme remains `plasma_lava`
- manifest posture remains `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`
- the default classic path remains `raster` + `native_raster` + `flat`

`PL12` does not make BenchLab-only overrides part of the default saver path.

## Implemented Subset Versus Future Surface

Implemented now:

- real product-local overlay and report export
- real requested versus resolved versus degraded reporting
- real bounded product-local forcing
- real clamp reporting
- real content, lane, transition, presentation, seed, and settings reporting for the implemented Plasma engine

Still later work:

- deeper inspection tooling
- full interactive forcing coverage
- broader validation/performance envelope proof
- release-cut evidence and release curation

## Current Known Limitations

- Plasma's BenchLab forcing surface is intentionally bounded; not every future product control is forceable yet.
- Reporting depends on concepts Plasma actually models; unsupported future notions are not fabricated.
- Full BenchLab inspection UI depth remains later work even though the product-local data surface now exists.

## What Remained Intentionally Unchanged

- shared renderer and routing law
- default saver routing posture
- default classic content identity
- normal non-BenchLab config and session flow
- release and performance claims

## What PL13 May Build On Next

`PL13` can build on this product-local BenchLab seam by:

- broadening truthful validation captures
- expanding performance and degradation evidence
- adding deeper automated proof coverage over the now-exportable resolved truth

## Scope Boundary

`PL12` is the BenchLab-integration phase.
It is not the final validation sign-off phase and it is not release curation work.
