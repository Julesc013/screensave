# MX Program

This document defines the master control plane for post-PL Plasma and later shared ScreenSave follow-on work.

## Purpose

`MX00` freezes the governance substrate that later `SY` and `PX` epics must follow.

It exists to:

- make the post-PL program explicit
- keep owner boundaries clear
- keep later waves gated
- prevent premature promotion of Plasma-local work into shared scope
- preserve the narrow stable Core promise while later work hardens proof, control, and compositional depth

## Current Post-PL Baseline

Current repo reality is the authority baseline for this program.

- Plasma is already post-`PL14`.
- Plasma's current ship posture is `GO WITH CAVEATS`.
- Stable Core Plasma remains Classic-first and lower-band-first.
- The stable default center of gravity remains:
  - `default_preset=plasma_lava`
  - `default_theme=plasma_lava`
  - `minimum_kind=gdi`
  - `preferred_kind=gl11`
  - `quality_class=safe`
- `gdi` remains a real floor.
- `gl11` remains the truthful preferred stable lane unless later evidence justifies change.
- Stable versus experimental remains explicit.
- Requested versus resolved versus degraded truth remains explicit.
- `Plasma Classic` remains preserved.

Current caveats that drive early-wave priorities are also already explicit in the repo:

- cross-hardware proof is limited
- transition coverage is bounded
- dimensional presentation is still limited
- BenchLab forcing is bounded
- hard numeric performance SLAs are not yet established

Repo-truth note:
older roadmap docs still record the earlier `SS` and `SX` handoff framing in which `PL` was later or next.
Those docs remain useful lineage context, but current repo reality and the completed Plasma `PL00` through `PL14` docs take precedence for post-PL execution.

## Namespaces

The post-PL program uses exactly three namespaces:

| Namespace | Meaning | Owns |
| --- | --- | --- |
| `MX` | master governance and control plane | queue doctrine, wave admission, owner rules, dependency arbitration, proof-surface doctrine |
| `SY` | shared ScreenSave system work | shared proof contracts, shared environment/system follow-on, companion integration, platform-level reusable contracts when promotion is justified |
| `PX` | Plasma product work | Plasma-local hardening, expansion, curation, proof, and promotion review |

## Why This Is Post-PL And Not A New PL Series

The Plasma subtree already contains a completed bounded `PL00` through `PL14` line.

That line:

- froze Plasma identity and doctrine
- landed the content, lane, transition, settings, and BenchLab seams
- recorded the current validation matrix and known limits
- made the current release cut and ship decision

Later work is therefore not a fresh `PL15+` planning line.
It is a post-PL hardening and expansion program that starts from the implemented and documented Plasma state already in the repo.

## Canonical Wave Sequence

The canonical initial wave map is frozen as follows.

| Wave | Epics | Role |
| --- | --- | --- |
| `Wave 0` | `MX00` | freeze the control plane before any later queue admission |
| `Wave 1` | `SY10`, `PX10`, `PX11` | harden proof, support, and bounded transition coverage against current caveats |
| `Wave 2` | `SY20` conditional, `PX20` | freeze promotion-review and shared-contract decisions before deeper authoring growth |
| `Wave 3` | `PX30`, `PX31`, `PX32` | land the product-local data, lab, and selection substrate |
| `Wave 4` | `PX40`, `SY40`, `PX41` | expand Plasma's expressive surface and only then connect broader shared ops/integration work |
| `Wave 5` | `PX50` | final promotion review and stable recut decision |

Later waves must not queue until earlier wave gates pass.

## Guiding Rules

- Governance first. No later epic becomes canonical until the control plane is frozen.
- Stable Core protection. Do not broaden the stable promise first.
- Evidence first. Broaden proof, control, and compositional depth before stable widening.
- Plasma identity protection. Plasma must not become the place where the suite gets redesigned accidentally.
- Shared promotion is conditional, not assumed.
- Vertical-slice done is mandatory for later feature work.
- Proof surfaces are part of the deliverable, not cleanup work.

## Vertical-Slice Definition Of Done

Every later feature slice must land all of the following:

- code
- lane and degrade policy
- settings placement
- BenchLab visibility
- docs
- smoke and proof
- known-limit updates
- stable versus experimental classification

If one of those surfaces is missing, the slice is not done.

## Shared-Versus-Local Promotion Rule

Promote work into shared system scope only when at least one of the following is true:

- two or more products genuinely need the same stable abstraction
- the work clearly belongs to a general platform, validation, or companion contract

Otherwise the work stays Plasma-local, even if Plasma discovered the need first.

This rule exists to prevent:

- one product silently redefining shared platform law
- speculative suite-wide abstractions built around one product's unfinished needs
- the stable shared surface growing faster than the evidence that supports it

## Execution Model For Later Epics

Later epics should execute under this control plane as follows:

1. land or refresh the paired `ASK` decision first
2. confirm the wave gate is open
3. keep the write surface within the approved epic scope
4. update the declared proof surfaces in the same pass
5. return the result in a way that makes gate impact explicit

If an epic discovers a shared concern while doing Plasma-local work:

1. record the product-local symptom
2. route the shared question back through `MX`
3. promote it to `SY` only if the promotion rule is satisfied
4. otherwise keep the work in `PX`

## Stable-Widening Rule

Stable widening is evidence-gated.

Later work may:

- strengthen proof
- improve controls
- broaden authoring and curation depth
- expand optional experimental surfaces

Later work may not quietly widen the stable promise until the final promotion review in `PX50` says the evidence is strong enough.
