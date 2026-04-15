# U04 Captures

This directory contains the focused deterministic BenchLab reports gathered for
`U04`.

They are presentation-truth and degrade-truth evidence for the admitted visual
grammar subset.
They are not a claim that BenchLab can independently force every output or
treatment field in this phase.

## Capture Set

| File | Purpose |
| --- | --- |
| `benchlab-plasma-presentation-ribbon.txt` | shows that a requested premium `ribbon` presentation resolves cleanly to `ribbon` on the premium lane |
| `benchlab-plasma-degrade-ribbon-auto.txt` | shows that a requested premium `ribbon` presentation degrades back to `flat` on the truthful lower-lane auto path while keeping requested-versus-resolved truth visible |
| `benchlab-plasma-degrade-contour-extrusion-raster.txt` | shows that `contour_extrusion` degrades back to `flat` when the resolved output family is `raster`, even on the premium lane |

## Reproduction Pattern

The checked-in `U04` reports were generated with deterministic BenchLab report
mode:

```powershell
benchlab.exe /deterministic /seed:743 /saver:plasma /frames:4 /report:<path>
```

The U04 presentation proofs add:

```powershell
/renderer:gl46
/plasma-presentation:ribbon
/plasma-presentation:contour_extrusion
```

## Honest Limits

- These captures are BenchLab windowed-harness reports, not fullscreen `.scr`
  host proof by themselves.
- The capture set is intentionally narrow because `U04` is a grammar-truth
  checkpoint, not the later universal proof harness.
- BenchLab still does not independently force output-family or treatment-slot
  changes in this phase, so output and treatment distinctness are proved by the
  deterministic smoke harness rather than by standalone BenchLab capture
  forcing.
