# MX Wave Gates

This document freezes the pass conditions for each post-PL wave.

## Gate Rules

- A later wave does not queue until the earlier gate passes.
- Passing a gate requires both delivery and proof-surface updates.
- A wave may still defer one of its own conditional epics, but that defer decision must be explicit.
- Stable widening is not a valid reason to skip an earlier hardening gate.

## Wave 0 Gate

Wave 0 passes only when all of the following are true:

- the `MX00` governance docs exist and are internally consistent
- the `PX00` Plasma control docs exist and reflect current repo reality
- the queue manifests for `wave0` through `wave5` exist
- the owner map, dependency board, wave gates, proof-surface map, and shared-promotion rules are all explicit
- the active roadmap and Plasma doc indexes point at the new control plane
- later waves are defined but remain blocked behind their gates

Intent:
governance first, before any later `SY` or `PX` epic becomes canonical.

## Wave 1 Gate

Wave 1 passes only when all of the following are true:

- `SY10`, `PX10`, and `PX11` are complete
- the current caveats have been either reduced or explicitly carried forward with updated evidence
- shared proof and environment follow-on is documented without changing platform law
- Plasma proof notes, validation captures, feature ledger, and risk register are updated
- requested versus resolved versus degraded truth remains explicit in the updated proof surface
- `gdi`, `gl11`, `Plasma Classic`, and the stable default center of gravity remain re-proved
- `MX` records an explicit decision on whether `SY20` is justified or deferred

Intent:
support hardening and proof discipline before promotion review or authoring expansion.

## Wave 2 Gate

Wave 2 passes only when all of the following are true:

- `PX20` is complete
- `SY20` is either complete or explicitly deferred with justification
- the post-Wave-1 shared-versus-local boundary is no longer ambiguous for Wave 3 work
- authoring, provenance, and promotion-review boundaries are frozen tightly enough to support product-local substrate work
- no Wave 3 epic still depends on an unresolved shared promotion decision

Intent:
freeze the product-model and promotion boundary before deeper data and lab work lands.

## Wave 3 Gate

Wave 3 passes only when all of the following are true:

- `PX30`, `PX31`, and `PX32` are complete
- authorable sets and journeys, minimal Lab/validator core, and selection intelligence all have real proof surfaces
- later Wave 4 work can depend on a mature product-local model rather than ad hoc Plasma behavior
- any newly discovered shared concern has either been promoted intentionally or left product-local intentionally
- the Plasma feature ledger and risk register reflect the new substrate truth

Intent:
data and authoring substrate before major language expansion or ecosystem integration.

## Wave 4 Gate

Wave 4 passes only when all of the following are true:

- `PX40`, `SY40`, and `PX41` are complete
- expressive expansion, shared ops/integration, and curation/provenance operations all have truthful degrade policy and proof updates
- shared integration work remains companion and system scope, not a quiet rewrite of Plasma or platform law
- the product is mature enough that a final promotion review can judge stable widening from evidence rather than from aspirational design

Intent:
ecosystem and broader integration only after product and model maturity.

## Wave 5 Gate

Wave 5 passes only when all of the following are true:

- `PX50` is complete
- the final stable-versus-experimental review is explicit
- the risk register records final accept, defer, or holdback decisions
- release-facing posture is updated honestly
- any item that did not make the stable recut is explicitly left experimental or deferred rather than silently absorbed

Intent:
stable recut last, after hardening, proof, authoring maturity, and integration maturity.
