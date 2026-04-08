# PL13 Captures

This directory contains the bounded deterministic BenchLab reports gathered for `PL13`.
They are validation evidence for Plasma's implemented subset, not general release proof.

## Capture Set

| File | Purpose |
| --- | --- |
| `benchlab-plasma-gdi.txt` | forced universal-floor proof on the preserved classic baseline |
| `benchlab-plasma-gl11.txt` | forced compat-lane proof on the preserved classic baseline |
| `benchlab-plasma-gl21.txt` | forced advanced-lane proof for the bounded richer subset |
| `benchlab-plasma-gl33.txt` | forced modern-lane proof for the bounded refined subset |
| `benchlab-plasma-gl46-heightfield.txt` | forced premium-lane proof for the bounded premium subset plus `heightfield` |
| `benchlab-plasma-degrade-heightfield-auto.txt` | requested-premium degrade proof showing `heightfield` forcing clamps back to compat on auto policy |
| `benchlab-plasma-journey-gl11.txt` | non-premium transition proof showing a live journey-driven `preset_morph` on `gl11` |

## Reproduction Pattern

All checked-in `PL13` reports were generated with deterministic BenchLab report mode:

```powershell
benchlab.exe /deterministic /seed:743 /saver:plasma /frames:<count> /report:<path>
```

Forced-lane proofs add:

```powershell
/renderer:gdi
/renderer:gl11
/renderer:gl21
/renderer:gl33
/renderer:gl46
```

Product-local proof additions include:

```powershell
/plasma-presentation:heightfield
/plasma-preset-set:classic_core
/plasma-theme-set:warm_classics
/plasma-journey:classic_cycle
/plasma-transitions:on
/plasma-transition-policy:journey
```

## Honest Limits

- These captures are BenchLab windowed-harness reports, not fullscreen `.scr` host proof by themselves.
- The capture set comes from one capable validation machine.
- The reports prove requested-versus-resolved runtime truth for the implemented subset, not broad numeric performance guarantees.
