# Plasma U08 Visual QA Bar

## Purpose

This note defines the acceptance bar used for the salvage recut.

## Stable Surface Bar

A stable Plasma surface must satisfy all of the following:

- distinctness: it must read as a different visual identity, not a weak palette variant
- legibility: structure must survive the intended display treatment and remain readable in motion
- motion quality: motion must feel deliberate rather than noisy, jittery, or stalled
- dark-room behavior: bright-area handling and scanline or accent lift must remain room-safe
- preview behavior: the preset must still read in the smaller preview window
- long-run behavior: the preset must remain watchable beyond the first novelty minute
- lower-band honesty: `gdi` and `gl11` behavior must remain truthful for stable claims
- graceful degradation: requested richer settings must clamp or fall back explicitly instead of lying

## Experimental Surface Bar

Experimental surfaces may be wider, but they still must satisfy:

- a real visual identity
- explicit gating
- explicit requested/resolved/degraded truth
- bounded proof coverage
- known-limit notes when the visual or hardware story is still partial

Experimental does not mean fake.
It means broader, riskier, or less fully validated.

## Removal And Demotion Rules

A surface should be retuned, demoted, hidden, or removed when:

- the visual output is not perceptually distinct enough
- the control does not materially move the rendered result
- the surface only works by accidental preset coupling
- the degrade story is confusing or dishonest
- the product cannot explain or prove the surface without hand-waving

## Current Salvage Interpretation

For this salvage pass, the bar is enforced through:

- deterministic smoke influence checks
- BenchLab reporting checks
- preset-signature and palette audits
- the narrower stable-versus-experimental recut that follows in U09
