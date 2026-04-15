# Plasma U07 Diff And Duplicate Proof

## Purpose

This note records the actual U07 difference and duplicate-detection evidence.

## Commands Actually Run

Preset and palette audit:

```powershell
python tools\scripts\plasma_lab.py preset-audit --threshold 3
```

BenchLab semantic capture comparison:

```powershell
python tools\scripts\plasma_lab.py capture-diff --left validation\captures\u07\benchlab-default-gdi.txt --right validation\captures\u07\benchlab-default-gl11.txt
python tools\scripts\plasma_lab.py capture-diff --left validation\captures\u07\benchlab-heightfield-gl46.txt --right validation\captures\u07\benchlab-heightfield-gdi.txt
python tools\scripts\plasma_lab.py degrade-report --capture validation\captures\u07\benchlab-heightfield-gdi.txt
```

## Diff And Duplicate Methodology

The current U07 methodology is mixed and honest:

- compiled preset signatures detect exact duplicates and near-duplicate crowding
  in the shipped content surface
- compiled theme palette distance reports detect suspicious palette crowding
- BenchLab semantic capture diffs detect when renderer or degrade changes are
  real routing truth rather than silent semantic collapse

This is stronger than prose-only review, but it is still not the final
screenshot-grade QA bar.

## Dead Or Near-Duplicate Findings

Fresh U07 audit findings:

- no exact duplicate compiled preset signatures remain
- the closest stable preset pair is
  `museum_phosphor <-> plasma_lava` at distance `3`
- the closest first-class stable preset pair inside `classic_core` is also
  `museum_phosphor <-> plasma_lava` at distance `3`
- additional first-class stable crowding still exists at distance `3` for
  `museum_phosphor <-> quiet_darkroom` and
  `plasma_lava <-> quiet_darkroom`
- the closest first-class stable theme pair inside `classic_core` is
  `aurora_cool <-> oceanic_blue` at palette distance `188`

Broader near-duplicate pairs still surfaced by the generated threshold-3 audit
include:

- `ascii_reactor <-> matrix_lattice`
- `aurora_bands <-> ribbon_aurora`
- `aurora_plasma <-> cellular_bloom`
- `lava_isolines <-> wire_glow`

These are not automatic cut decisions in U07, but they are real U08 inputs.

## Degrade And Collapse Findings

Fresh U07 capture findings:

- the default `gdi` versus `gl11` diff shows routing and lane truth changing
  while the stable default preset, theme, and grammar identity stay intact
- the `heightfield` premium-versus-`gdi` diff shows a real explicit
  `premium -> universal` degrade and a `presentation` clamp back to `flat`
- no silent presentation collapse was observed in that tested degrade case;
  the degrade is explicit in BenchLab truth

## What Was Found To Remain Weak

Fresh U07 influence reporting still marks these as bounded rather than strongly
render-proved:

- `favorites_only`
- diagnostics overlay state
- transition timing, seed continuity, and policy surfaces

That is now explicit rather than hidden.

## Actual Evidence Versus Documentation Only

Actual evidence:

- `validation/captures/u07/preset-audit-threshold-3.txt`
- `validation/captures/u07/capture-diff-default-gdi-vs-gl11.txt`
- `validation/captures/u07/capture-diff-heightfield-gl46-vs-gdi.txt`
- `validation/captures/u07/degrade-report-heightfield-gdi.txt`

Documentation-only claims:

- final U08 keep, hide, or recut decisions for every near-neighbor pair
