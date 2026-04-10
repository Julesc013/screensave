# PX10 Support Hardening

## Purpose

`PX10` is the first post-`PL14` Plasma product-local hardening tranche.
It exists to reduce the current support and proof caveats without widening Plasma's visual language, rewriting the stable cut, or changing the default Classic identity.

## Why PX10 Exists Now

The current repo is already post-`PL14` and the current ship posture remains `GO WITH CAVEATS`.
The biggest open caveats are support and proof caveats rather than missing product architecture:

- cross-hardware proof is limited
- numeric performance posture is still bounded and mostly qualitative
- multi-monitor behavior is real at the shared host layer but still needs a Plasma-local support statement
- environment safety and recovery claims exist in repo language, but they need to be tied back to actual product behavior

`PX10` hardens those four surfaces so later `PX11` review work can judge a stronger support posture instead of inheriting loose caveat wording.

## Relationship To MX00, SY10, And PX11

- `MX00` freezes the control plane and makes `PX10` a Wave 1 Plasma hardening epic.
- `SY10` freezes the shared support vocabulary for proof matrices, soak language, multi-monitor host policy, and environment safety wording.
- `PX10` consumes those shared contracts and turns them into Plasma-local docs, proof notes, validation surfaces, and smoke expectations.
- `PX11` should build on the stronger support surface left here rather than re-litigating the baseline support language.

## The Four PX10 Domains

### Cross-Hardware Proof Expansion

`PX10` makes the actual Plasma hardware matrix explicit, including:

- the current validated single-machine evidence
- the exact blocked breadth that still does not exist
- the difference between validated, partial, blocked, and not-tested states

### Numeric, Performance, And Soak Strengthening

`PX10` splits the current posture into:

- directly measured or counted repo evidence
- bounded qualitative evidence
- still-unknown numeric claims

It does not create hard numeric SLAs.

### Plasma Multi-Monitor Baseline

`PX10` freezes the current Plasma baseline on top of the shared host contract:

- one saver session spanning the virtual desktop in fullscreen
- one preview child-window path
- no independent per-monitor Plasma sessions or per-monitor choreography

### Plasma Environment Safety And Recovery

`PX10` ties the current support-facing terms back to real product behavior:

- `preview_safe`
- `long_run_stable` and `long_run_safe`
- current dark-room-safe descriptive content choices
- truthful clamp, fallback, and config-recovery behavior

## What Was Actually Hardened

This tranche hardens:

- the product-local hardware matrix and environment fingerprint model
- the product-local measured-versus-qualitative-versus-unknown envelope split
- the explicit Plasma multi-monitor baseline statement
- the explicit Plasma environment safety and recovery statement
- the product validation catalog and smoke expectations for those support surfaces

## What Was Only Documented Or Carried Forward

`PX10` does not pretend the repo gained proof it does not have.
This tranche carries forward existing real evidence where that evidence already exists and makes the unsupported or blocked areas explicit.

That means:

- the single-machine proof base is still the main runtime evidence anchor
- the absence of hard numeric SLAs remains explicit
- the current multi-monitor baseline is documented honestly without inventing per-monitor behavior
- reserved safety terms remain unclaimed unless the repo already implements them

## What Remains Intentionally Unchanged

`PX10` does not change:

- product name `Plasma`
- preserved `Plasma Classic`
- default preset `plasma_lava`
- default theme `plasma_lava`
- routing posture `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=safe`
- the stable-only default content filter
- transitions-off-by-default stable posture
- the bounded dimensional and transition scope

## Current Caveats Being Attacked

`PX10` materially attacks:

- vague single-machine proof wording by replacing it with an explicit product matrix
- vague performance wording by splitting measured, qualitative, and unknown evidence
- implicit multi-monitor assumptions by freezing the actual current baseline
- implicit safety wording by tying it back to real preview, long-run, and recovery behavior

## Current Caveats That Still Remain

`PX10` does not eliminate:

- the cross-hardware gap beyond the current validation machine
- the lack of hard numeric performance SLAs
- the bounded transition subset
- the limited dimensional presentation subset
- the bounded BenchLab forcing surface
- the absence of product-modeled OLED, battery, thermal, remote, or safe-mode behavior

## Scope Boundary

`PX10` is support and proof hardening.
It is not feature-language expansion, not release-cut rewriting, and not stable promotion.
