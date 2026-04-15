# Plasma U07 Combination Proof

## Purpose

This note records the actual U07 evidence used to prove the stable reachable
combination space and the bounded experimental coverage strategy.

## Commands Actually Run

Stable and experimental report generation:

```powershell
python tools\scripts\plasma_lab.py combination-matrix
python tools\scripts\plasma_lab.py experimental-coverage
```

BenchLab stable and degrade captures:

```powershell
New-Item -ItemType Directory -Force validation\captures\u07 | Out-Null

& .\out\msvc\vs2022\Debug\benchlab\benchlab.exe `
  /deterministic `
  /seed:743 `
  /saver:plasma `
  /frames:4 `
  /report:validation\captures\u07\benchlab-default-gdi.txt `
  /renderer:gdi

& .\out\msvc\vs2022\Debug\benchlab\benchlab.exe `
  /deterministic `
  /seed:743 `
  /saver:plasma `
  /frames:4 `
  /report:validation\captures\u07\benchlab-default-gl11.txt `
  /renderer:gl11

& .\out\msvc\vs2022\Debug\benchlab\benchlab.exe `
  /deterministic `
  /seed:743 `
  /saver:plasma `
  /frames:4 `
  /report:validation\captures\u07\benchlab-heightfield-gl46.txt `
  /renderer:gl46 `
  /plasma-presentation:heightfield

& .\out\msvc\vs2022\Debug\benchlab\benchlab.exe `
  /deterministic `
  /seed:743 `
  /saver:plasma `
  /frames:4 `
  /report:validation\captures\u07\benchlab-heightfield-gdi.txt `
  /renderer:gdi `
  /plasma-presentation:heightfield

& .\out\msvc\vs2022\Debug\benchlab\benchlab.exe `
  /deterministic `
  /seed:424242 `
  /saver:plasma `
  /frames:4 `
  /report:validation\captures\u07\benchlab-classic-cycle-gl11.txt `
  /renderer:gl11 `
  /plasma-preset:plasma_lava `
  /plasma-theme:plasma_lava `
  /plasma-transitions:true `
  /plasma-transition-policy:journey `
  /plasma-journey:classic_cycle
```

Support reports:

```powershell
python tools\scripts\plasma_lab.py capture-diff --left validation\captures\u07\benchlab-default-gdi.txt --right validation\captures\u07\benchlab-default-gl11.txt
python tools\scripts\plasma_lab.py capture-diff --left validation\captures\u07\benchlab-heightfield-gl46.txt --right validation\captures\u07\benchlab-heightfield-gdi.txt
python tools\scripts\plasma_lab.py degrade-report --capture validation\captures\u07\benchlab-heightfield-gdi.txt
```

## Stable Matrix Coverage Exercised

Fresh U07 matrix evidence:

- `validation/captures/u07/stable-combination-matrix.txt` enumerates all 50
  stable rows
- every row is marked `validated`
- the rows reflect the actual first-class stable preset and theme sets, not a
  broader idealized cross-product

The deterministic smoke harness is the executable proof path behind those 50
rows.

## Experimental Coverage Slices Exercised

Fresh U07 experimental-boundary evidence:

- `validation/captures/u07/experimental-coverage.txt` records the current
  bounded slice strategy for experimental preset, theme, lane, output,
  treatment, presentation, and transition surfaces

Fresh execution evidence tied to that boundary:

- the smoke harness now exercises non-raster and richer-lane signature cases
  for `posterized_bands`, `contour_only`, `contour_bands`, `ascii_glyph`,
  `matrix_glyph`, `halftone_stipple`, `emboss_edge`, `ribbon`, and
  `heightfield`
- `validation/captures/u07/benchlab-classic-cycle-gl11.txt` records the bounded
  journey transition slice on the surviving stable core

This is intentionally bounded coverage, not a fake exhaustive claim.

## Evidence That Degrade And Lane Truth Remain Honest

Fresh U07 capture evidence:

- `validation/captures/u07/capture-diff-default-gdi-vs-gl11.txt` shows the
  stable default path preserving preset, theme, and grammar identity while only
  lane and renderer truth change
- `validation/captures/u07/capture-diff-heightfield-gl46-vs-gdi.txt` shows
  `heightfield` remaining premium on `gl46` but clamping back to `flat` on
  `gdi`
- `validation/captures/u07/degrade-report-heightfield-gdi.txt` records the
  explicit `premium -> universal` degrade and the `presentation` clamp

## Passed, Failed, And Blocked

Passed:

- generated stable combination matrix
- generated experimental coverage report
- refreshed U07 BenchLab default-lane captures
- refreshed U07 premium-versus-lower-band presentation captures
- refreshed capture-diff and degrade-report outputs

Failed:

- none in this checkpoint

Blocked:

- U07 does not claim exhaustive proof for the full experimental cross-product
- U07 does not claim screenshot-grade proof for every renderer path

## Actual Evidence Versus Documentation Only

Actual evidence:

- stable combination matrix report
- experimental coverage report
- BenchLab captures
- capture-diff and degrade-report outputs

Documentation-only claims:

- any broader future experimental coverage model beyond the current bounded
  slice
