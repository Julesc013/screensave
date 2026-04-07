# Internal Render Bands Specification

This document is normative.

## Intent

This specification defines the stable internal capability-band model used during and after `SX`.

The band model exists to let the private substrate grow more cleanly without changing the public renderer ladder.
Bands are internal routing and planning law, not new public renderer-tier names.

## Public Boundary

- The public renderer ladder remains `gdi`, `gl11`, `gl21`, `gl33`, `gl46`, `null`.
- `screensave_renderer_kind` remains the public tier vocabulary exposed by the current renderer contract.
- Release notes, compatibility notes, and user-facing docs continue to describe renderer support through the public ladder unless a later explicit spec changes that rule.
- Internal bands may inform routing, fallback, and product planning, but they do not replace the public tier names.

## Stable Band Set

The stable internal band set is:

| Band | Current Public-Tier Mapping | Meaning |
| --- | --- | --- |
| `universal` | `gdi` | the guaranteed compatibility floor that must remain viable on the broad baseline |
| `compat` | `gl11` | the conservative accelerated band that still preserves broad fallback discipline |
| `advanced` | `gl21` | the first real advanced-capability band in the current public ladder |
| `modern` | `gl33` | a later optional modern-capability band that remains placeholder-only until later `SX` work makes it real |
| `premium` | `gl46` | a later optional highest-capability band that remains placeholder-only until later `SX` work makes it real |

`null` is not a normal band.
It remains an internal safety fallback beneath the normal compatibility walk.

## Band Semantics

- `universal` must preserve the GDI floor and the broad compatibility-first posture.
- `compat` may add conservative acceleration, but it must still behave like a fallback-friendly tier rather than a new minimum requirement.
- `advanced` is the first band where richer later-capability behavior may exist in the current public ladder.
- `modern` and `premium` authorize future later-capability work only after the lower bands are already stable and honest.
- No saver is required to support every band.
- Products should target band semantics rather than backend-private implementation details.

## Routing Rules

The internal routing model is:

1. A request enters through the existing public tier selection path, including `auto`.
2. The substrate derives a requested band ceiling and product allowance from that public request and the product's declared support.
3. The substrate selects the highest available compatible band at or below that ceiling.
4. The substrate then chooses a private backend kind that satisfies that selected band.
5. The public renderer contract still reports the requested and active public tier plus the selection and fallback reasons.

## Fallback Walk

Fallback is normal, not exceptional.

The canonical fallback walk is:

`premium -> modern -> advanced -> compat -> universal -> null`

Rules:

- The substrate must walk downward intentionally rather than failing unpredictably.
- Lower-band operation must remain a normal supported outcome.
- Backend-specific quirks must not leak into saver doctrine or release doctrine as if they were product identity.
- `null` may preserve host stability, but it does not become a normal product capability band.

## Product Rules

- Products may advertise or document higher-band enhancements only when the matching public tier claims remain honest.
- Products in the universal saver set must still define a valid `universal` / `gdi` path.
- Products must not depend directly on backend-private state, backend-private services, or backend-private routing names.
- Products may evolve toward stable internal band targeting, but that remains an internal substrate concern until a later explicit public-spec change says otherwise.

## Future Remapping Rules

Future `SX` work may remap a band to a different private backend or service composition only when all of the following remain true:

- the public renderer ladder and public renderer contract stay intact
- `gdi` remains the guaranteed floor
- lower-band fallback remains available and honest
- product and release notes do not overstate what the remapped band guarantees
- the remapping does not introduce a mandatory baseline dependency on a later service family

This rule allows private substrate evolution without silently changing public doctrine.
