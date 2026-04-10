# MX System Dependency Board

This document records the canonical dependency board for the post-PL program.

## Board Format

Each epic is recorded with this model:

- `owner`: `[P]`, `[S]`, or `[H]`
- `depends_on`: epics that must already be defined or complete before this epic queues
- `blocked_by`: the wave gate that must pass before queue admission
- `promotes_shared_contract`: `yes`, `no`, or `conditional`
- `local_only_ok`: whether a Plasma-local solution is still acceptable at this stage

`local_only_ok=yes` means the work may stay Plasma-local if the shared-promotion rule is not met.
`promotes_shared_contract=conditional` means the epic exists to decide whether promotion is justified, not to assume it.

## Initial Dependency Board

| Epic | Owner | Depends On | Blocked By | Promotes Shared Contract | Local Only OK | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| `MX00` | `[H]` | `none` | `none` | `yes` | `no` | Freezes the control plane and later queue model. |
| `SY10` | `[S]` | `MX00` | `Wave 0 gate` | `yes` | `no` | Shared proof, environment, and dependency-baseline follow-on. |
| `PX10` | `[P]` | `MX00` | `Wave 0 gate` | `no` | `yes` | Plasma proof hardening and caveat burn-down. |
| `PX11` | `[P]` | `MX00`, `PX10` | `Wave 0 gate` | `no` | `yes` | Transition coverage hardening stays product-local until reuse is proven. |
| `SY20` | `[H]` | `SY10`, `PX20` | `Wave 1 gate` | `conditional` | `yes` | Only queues if Wave 1 or `PX20` proves a real shared contract need. |
| `PX20` | `[P]` | `SY10`, `PX10`, `PX11` | `Wave 1 gate` | `no` | `yes` | Freezes promotion-review and authoring-boundary decisions for Plasma. |
| `PX30` | `[P]` | `PX20` | `Wave 2 gate` | `no` | `yes` | Provenance and authorable sets and journeys remain Plasma-local first. |
| `PX31` | `[P]` | `PX20` | `Wave 2 gate` | `no` | `yes` | Minimal Plasma Lab and validator/comparer core remain product-owned first. |
| `PX32` | `[P]` | `PX20` | `Wave 2 gate` | `no` | `yes` | Selection intelligence depends on a frozen Plasma authoring and promotion boundary. |
| `PX40` | `[P]` | `PX30`, `PX31`, `PX32` | `Wave 3 gate` | `no` | `yes` | Major expressive expansion waits for a mature product-local substrate. |
| `SY40` | `[H]` | `PX31`, `PX40`, `PX41` | `Wave 3 gate` | `yes` | `no` | Shared ops and integration only queue after product surfaces are mature enough to expose. |
| `PX41` | `[P]` | `PX30`, `PX40` | `Wave 3 gate` | `no` | `yes` | Curation and provenance operations remain Plasma-owned unless later reuse is proven. |
| `PX50` | `[H]` | `SY40`, `PX40`, `PX41` | `Wave 4 gate` | `conditional` | `no` | Final stable recut crosses product evidence, support posture, and master release truth. |

## Conditional Shared-Promotion Notes

- `SY20` is not automatically admitted.
- `SY20` should queue only if Wave 1 or `PX20` proves a real multi-product or platform-law need.
- If that proof does not exist, keep the work Plasma-local and leave `SY20` deferred.

## Plasma-Local-First Notes

The following areas must remain Plasma-local until evidence says otherwise:

- transition-coverage growth
- authorable set and journey behavior
- Plasma Lab behavior
- selection intelligence
- content provenance and curation operations
- dimensional language growth

## Shared-Now Notes

The following areas are already shared enough to live in `MX` or `SY`:

- queue doctrine
- wave admission
- owner and promotion rules
- proof-surface doctrine
- shared environment and system-proof follow-on
- later shared ops and integration once product surfaces are mature
