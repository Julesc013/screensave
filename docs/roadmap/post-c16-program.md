# Post-C16 Program

This document defines the active program after the frozen `C16` baseline.
It supersedes older roadmap wording that referred only to a generic future post-release `S` series.

## Closed Baseline

- `C00` through `C16` is complete and closed.
- `ScreenSave Core` is the primary product.
- Core is the saver ZIP product for the standalone `.scr` line.
- `anthology` is a real saver product, not the `suite` app.
- `Installer`, `Suite`, `BenchLab`, `SDK`, and `Extras` remain separate secondary companion channels.

## Active Sequence

Post-`C16` work now proceeds in this order:

| Program | Role | Scope |
| --- | --- | --- |
| `SS` | short release-closure bridge | normalize doctrine, surface the Core release clearly, refresh release evidence, and optionally lock a maintenance policy |
| `PL` | first flagship expansion program | take `plasma` forward as the first deeper post-release expansion without destabilizing Core |

Do not extend the `C` line further.
Do not revive a vague generic post-release `S` line.

## SS Series

`SS` exists to close release-facing follow-on work quickly and explicitly.
It is not a new endless continuation line.

| Step | Purpose | Notes |
| --- | --- | --- |
| `SS00` | release doctrine normalization and roadmap cleanup | clarify Core-first doctrine and retire stale generic post-release wording |
| `SS01` | Core release publication and public release surfacing | publish the frozen Core release clearly as the primary product |
| `SS02` | release evidence refresh and support matrix | refresh the support-facing evidence without inventing validation |
| optional `SS03` | maintenance/support policy and patch discipline | define how Core is supported after the initial post-release cleanup |

## PL Series

`PL` begins only after the `SS` closure bridge is complete.
`PL` is the first bounded flagship expansion program, with `plasma` as its lead product family.

`PL` is not permission to reopen Core doctrine.
It must preserve the frozen Core baseline while extending the platform carefully.

## Settled Doctrine To Preserve

- `ScreenSave Core` stays primary.
- The saver ZIP stays the product.
- Standalone `.scr` savers stay the main line.
- `anthology` stays a saver product and does not collapse into `suite`.
- `suite` stays the browser, launcher, preview, and configuration companion.
- BenchLab stays diagnostics-only.
- SDK stays contributor-facing only.
- Extras stays the holdback channel for experimental or lower-confidence material.
- Extensibility stays data-driven first, not plugin-first.
- The renderer ladder stays `gdi`, `gl11`, `gl21`, later `gl33`, later `gl46`, and internal `null`.
- `gdi` stays the guaranteed floor.
- `gl33` and `gl46` stay future tiers unless they become real.
- `null` stays internal only.
- Settled naming decisions stay closed, including `suite`, `anthology`, `atlas`, and related post-`C01` naming.

## Source Of Truth

- Release-channel doctrine: [release-channels.md](./release-channels.md)
- Core gate doctrine: [core-zip-doctrine.md](./core-zip-doctrine.md)
- Historical post-`S15` continuation record: [post-s15-plan.md](./post-s15-plan.md)
- Historical prompt and series summaries: [prompt-program.md](./prompt-program.md), [series-map.md](./series-map.md)
