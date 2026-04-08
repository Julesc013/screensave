# PL13 Transition Validation

## Purpose

This note records the specific PL13 transition validation surface that was actually exercised.

## Commands And Harness Steps

Primary transition proof came from:

- the Plasma smoke harness in [`smoke.c`](./smoke.c)
- the GL11 journey BenchLab capture in [`validation/captures/pl13/benchlab-plasma-journey-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-journey-gl11.txt)

The smoke executable was rebuilt and run with the toolchain-pinned command recorded in [`pl13-soak-and-proof.md`](./pl13-soak-and-proof.md).

The journey capture was generated with:

```powershell
Start-Process -FilePath 'out/mingw/i686/debug/benchlab/benchlab.exe' `
  -ArgumentList @(
    '/deterministic',
    '/seed:743',
    '/saver:plasma',
    '/frames:320',
    '/report:validation\captures\pl13\benchlab-plasma-journey-gl11.txt',
    '/renderer:gl11',
    '/plasma-preset-set:classic_core',
    '/plasma-theme-set:warm_classics',
    '/plasma-journey:classic_cycle',
    '/plasma-transitions:on',
    '/plasma-transition-policy:journey',
    '/plasma-transition-duration:1800',
    '/plasma-transition-interval:9000'
  ) `
  -WorkingDirectory 'd:\Projects\ScreenSave\screensave' `
  -Wait -PassThru
```

## What Was Validated

- real theme morph support for compatible theme-class pairs
- real preset morph support for compatible preset-class pairs
- bounded bridge morph support for compatible bridge-class pairs
- explicit fallback and reject behavior for unsupported pairs
- journey progression across the implemented curated set subset
- requested-versus-resolved transition reporting through BenchLab
- transition capability on non-premium lanes

## Invariants Checked

- supported morphs are not misreported as generic fades
- unsupported pairs use explicit fallback or rejection behavior
- journey-driven transitions remain field-derived and content-identity-preserving
- premium hardware is not required for meaningful transition proof
- default non-transition behavior remains intact when transitions are disabled

## Non-Fatal Observed Differences

- The BenchLab journey capture shows a live `preset_morph` on `gl11`, but it is still one curated journey proof path rather than a universal transition sweep.
- Repeated smoke soak uses the authored journey dwell values and bounded follow-up ticks so bridge transitions can settle honestly.

## Blockers

- PL13 does not provide exhaustive proof for every preset/theme pair.
- PL13 does not provide broader cross-hardware transition captures.

## Evidence Boundary

- Actual evidence: smoke assertions for theme morph, preset morph, bridge morph, fallback, reject, and repeated journey stepping; plus the GL11 journey capture showing `Transition type: preset_morph`.
- Documentation only: unexercised future journeys, unimplemented transition surfaces, and unsupported pairings beyond the currently curated subset.
