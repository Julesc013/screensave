# Product Architecture v1

Status: active ScreenSave product architecture contract.

## Purpose

This contract keeps ScreenSave product-led. It captures the durable shape of
the product family, the four-plane architecture, the product-owned semantic
rule, and the canary sequence that must prove shared abstractions before they
become platform contracts.

The short form remains:

```text
Portable meaning. Native delivery. Deterministic proof. Optional automation.
```

## Product Plane

The user-facing product family is:

| Product | Role |
| --- | --- |
| Core | Curated standalone native savers |
| Anthology | Standalone deterministic meta-saver |
| Manager | Optional install, repair, preview, configure, and pack-management tool |
| Workbench | Optional author, inspect, capture, compare, profile, and release-qualification tool |
| SDK | Contributor-facing contracts, schemas, examples, generators, and validation guidance |
| Labs | Explicit experimental holdback channel |
| Preservation | Historical artifacts, old toolchains, VM profiles, and compatibility evidence |

Core is primary. Anthology remains a saver. Manager, Workbench, SDK, Labs, and
Preservation are optional companion or support surfaces.

Saver binaries must remain autonomous native artifacts. They must not require
Manager, Workbench, AIDE, Omnigent, Codex, accounts, services, or the network.

## Semantic And Runtime Plane

The canonical semantic path is:

```text
VisualIntent
  -> product-family resolver
  -> product-specific specification
  -> deterministic runtime plan
  -> reference compute or optional accelerated compute
  -> rendering realization
  -> explicit surface
  -> presenter
  -> native host or headless host
```

VisualIntent is descriptive authoring intent only. It is not directly
executable and must not bypass product schemas, proof, or human promotion.

The shared platform may provide:

- fixed-step time
- deterministic random-number generation
- configuration primitives
- capability facts
- diagnostics and fallback reasons
- surfaces
- presentation contracts
- native host lifecycle
- content loading
- proof hooks

Each product owns:

- visual thesis
- state model
- simulation
- configuration semantics
- deterministic runtime plan
- reference rendering rules
- degradation policy
- visual acceptance criteria

The rule is:

```text
Share mechanics.
Preserve meaning.
```

## Product-Local Specification Rule

ScreenSave must not adopt one universal visual graph. Product-specific
specifications are required.

Examples:

- Plasma owns field, material, output, and treatment semantics.
- Ricochet owns stateful motion, trajectory, collision, and particle
  semantics.
- Nocturne owns minimal ambient raster semantics.
- City may own scene and layout semantics.
- Lifeforms may own cellular simulation semantics.
- Gallery may own asset-selection and composition semantics.

An abstraction may move into the shared platform only after:

1. at least two structurally different products use it naturally, or
2. a platform requirement forces it.

## Proof Plane

ScreenSave proof is product-owned infrastructure:

```text
catalog
  + build profiles
  + artifact audit
  + fixed-step runner
  + RGBA reference surface
  + deterministic capture
  + image comparison
  + lifecycle testing
  + performance and soak testing
  + compatibility evidence
  = proof bundle
```

Proof evidence does not automatically certify compatibility, approve releases,
or accept visual quality. Those remain separate decision classes.

## Canary Sequence

The shared architecture must be proven in this order:

| Canary | What It Proves |
| --- | --- |
| Nocturne | Minimal host, surface, deterministic rendering, capture, and packaging |
| Ricochet or equivalent | Stateful motion, geometry, resize behavior, lifecycle, and performance |
| Plasma | Dense fields, product-specific semantics, compute options, materials, degradation, and advanced routing |

The portable v2 seam must not be frozen after Nocturne alone. At least two
structurally different products must use the shared mechanics before the seam
is treated as stable.

## Development Plane

The development plane is:

```text
Git / worktrees / CI
  -> ScreenSave-owned commands
  -> ScreenSave proof bundles
  -> optional AIDE WorkUnits and EvidencePackets
  -> optional worker harnesses
```

AIDE is optional external development infrastructure. AIDE may coordinate
bounded work and record evidence. AIDE must not own ScreenSave product truth,
visual semantics, compatibility status, proof results, release promotion, or
runtime dependencies.

## Stable Command Surface Direction

The long-lived project command surface should converge toward:

```text
screensave status
screensave catalog
screensave validate
screensave build
screensave render
screensave compare
screensave lifecycle
screensave profile
screensave audit
screensave proof
screensave package
```

The current project adapter is the admitted fixed-command bridge for this
direction. It must not become a generic `screensave run anything` capability.

## Frozen Decisions

- Core is the primary product.
- Standalone savers remain autonomous.
- Anthology remains a saver.
- Manager and Workbench remain optional.
- Product semantics remain product-owned.
- Software/reference rendering is mandatory.
- Acceleration is optional and capability-gated.
- Data packages are the default extension mechanism.
- Runtime executable plugins are not a baseline feature.
- AIDE is external development infrastructure.
- Compatibility is artifact-specific and evidence-backed.
- Workbench uses the same runner as CI.
- Text, image, and video inputs produce bounded specifications, not
  unrestricted generated native code.
- Cross-platform support means shared semantics plus native hosts, not one
  lowest-common-denominator shell.

