# Plasma U08 Visual QA Bar

## Purpose

This note defines the formal product-quality bar applied in `U08`.

It exists so Plasma no longer relies on implied or wishful judgments about
which surviving surfaces are actually good enough to keep stable, which may
stay experimental, and which must leave the claim surface.

## Why U08 Exists Now

`U01` through `U07` made the runtime more coherent, narrowed the reachable
surface, retuned the surviving content, and built a real proof harness.

That still was not enough to answer the product question.

`U08` is the first tranche that binds those proof surfaces into explicit
acceptance decisions.

It defines and applies the bar.
It does not perform the final recut itself.

## Formal Product-Quality Bar

Every materially important surviving surface is judged across the same bounded
dimensions:

- `distinctness`: it must read as its own surface rather than as a weak alias,
  palette nudge, or accidental preset coupling
- `legibility`: structure must stay readable through its admitted treatments
  and renderer paths
- `motion_quality`: motion must look deliberate rather than noisy, jittery, or
  stalled
- `dark_room_behavior`: darker-room use must stay supportable and not depend on
  accidental over-bright output
- `preview_behavior`: the surface still has to read in preview and settings
  preview, not only in fullscreen
- `long_run_behavior`: the surface has to remain supportable beyond a novelty
  first impression
- `lower_band_honesty`: `gdi` and `gl11` truth must remain explicit and richer
  lanes must not be treated as free parity
- `graceful_degradation`: richer requests must clamp or fall back explicitly
  instead of silently collapsing
- `supportability`: the repo must be able to explain, prove, and support the
  surface without hand-waving

## Stable, Experimental, And Demotion Thresholds

### Pass Stable

A surface may remain stable-worthy only when it is:

- evidence-backed through current proof notes, smoke checks, generated reports,
  or captures
- lower-band-honest on the preserved `gdi` floor and the preferred `gl11`
  stable lane
- supportable in preview, fullscreen, and long-run posture for the admitted
  stable use case
- free of hidden premium-only, journey-only, or caveated semantics that would
  widen the stable promise by implication

### Pass Experimental

A surface may remain exposed as experimental when it is:

- implemented and materially real
- explicitly gated and reportable
- backed by bounded proof rather than by vibes
- still outside the narrow stable promise for distinctness, lower-band, or
  supportability reasons

### Keep Experimental With Caveat

A surface belongs here when it is real enough to keep, but still carries a
strong caveat such as:

- only bounded proof coverage
- premium-lane dependence
- narrower subset support than the taxonomy name might imply
- timing, journey, or stateful behavior that still lacks stronger visual proof

### Demote/Hide

A surface belongs here when it still exists for support or diagnostics, but no
longer deserves first-class visibility or product language.

### Remove From Claim Surface

A surface belongs here when it is preserved only for compatibility, migration,
or hidden state continuity and should no longer be spoken about as shipped
variety.

### Blocked/Insufficient Evidence

This result is reserved for surfaces that have not earned even a bounded keep
decision because the repo lacks enough direct evidence.

`U08` did not need to use that class for the current major surviving surfaces,
but the class remains part of the bar for later work.

## How Later Phases Should Use This Bar

Later phases should consume `U08` this way:

- `U09` may treat `pass_stable` as the default source of stable-worthy surfaces
- `pass_experimental` and `keep_experimental_with_caveat` remain opt-in and
  must not silently leak into stable claims
- `demote_hide` and `remove_from_claim_surface` stay out of ship-language
  except where compatibility notes explicitly require them

This is the acceptance bar and acceptance application layer.
It is not the final stable/experimental/deferred recut.
