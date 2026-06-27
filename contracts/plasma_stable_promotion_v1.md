# Plasma v2 Stable-Promotion Contract v1

Status: active stable-promotion contract.

This contract governs the PAW-I transition from `release-candidate` to one of:

- `stable-promoted`
- `request-changes`
- `defer-to-labs`
- `release-candidate-hold`

It is the first Plasma v2 gate allowed to set `stable = true` and
`release_promotion = "accepted"`. Public release publication remains a separate
act unless a later publication-prep packet explicitly gates it.

PAW-I is also the Plasma v2 instrument-architecture audit. It must ask whether
`plasma-v2-rc1` embodies the active product doctrine:

```text
Plasma is not a preset picker. Plasma is a visual instrument.
```

Stable promotion is allowed only when the candidate proves the direct-control
deterministic path:

```text
direct controls
-> plasma_v2_spec
-> plasma_v2_plan
-> plasma_v2_runtime
-> field/output/material/treatment/presentation
-> proof bundle
```

## Required Input State

- `portable_v2.status = "accepted"`
- `plasma_v2.status = "release-candidate"`
- `plasma_v2.release_candidate = "plasma-v2-rc1"`
- `plasma_v2.stable = false`
- `plasma_v2.release_promotion = "blocked"`
- `authority.active_program = "plasma-v2-stable-promotion"`

## Required Axes

The stable-promotion gate must check these axes separately:

- release-candidate contract passed
- instrument architecture audit passed
- final proof bundle passed
- final visual/artistic acceptance passed
- native artifact package passed
- support/compatibility wording passed
- known limits accepted
- security/provenance/SBOM/checksum review passed
- Manager/Workbench inspection passed
- AIDE evidence and repair queue clean
- release publication still explicitly gated

## Claim Separation

Stable promotion does not collapse these claims:

- validator pass != instrument architecture acceptance
- proof pass != artistic acceptance
- artistic acceptance != compatibility certification
- compatibility evidence != broad support promise
- stable promotion != public publication
- AIDE EvidencePacket != ScreenSave proof truth

## Explicit Exclusions

PAW-I stable promotion must not claim or perform:

- all-saver migration
- public SDK freeze
- broad Windows certification beyond evidence
- macOS/Linux support
- runtime executable plugin ecosystem
- automatic AIDE publication
- automatic AIDE promotion
- publication upload without an explicit publication gate
- hidden preset authority
- hidden GL11 minimum
- arbitrary graph runtime
- heightfield, ribbon, glyph, or GL46 stable claim

## Permitted Stable State

Only after the stable-promotion checker passes may `PROJECT_STATE.toml` set:

```toml
[authority]
active_program = "plasma-v2-publication-prep"
release_candidate = "plasma-v2-rc1"

[plasma_v2]
status = "stable-promoted"
stable = true
release_candidate = "plasma-v2-rc1"
release_promotion = "accepted"
opened_next = "plasma-v2-publication-prep"
```

If the checker blocks, state must remain not stable and must move to
`release-candidate-hold`, `request-changes`, or `defer-to-labs` with repair
WorkUnits. Missing instrument gates must hold with:

```toml
[plasma_v2]
status = "release-candidate-hold"
stable = false
release_promotion = "blocked"
opened_next = "plasma-v2-stable-repair"
```
