# SX09 Captures

This directory contains the bounded deterministic BenchLab reports gathered for `SX09`.
They are capture evidence, not general product output.

## Capture Set

| File | Purpose |
| --- | --- |
| `benchlab-gallery-gdi.txt` | forced universal-floor proof on the showcase saver |
| `benchlab-gallery-gl11.txt` | forced compat-lane proof on the showcase saver |
| `benchlab-gallery-gl21.txt` | forced advanced-lane proof on the showcase saver |
| `benchlab-gallery-gl33.txt` | forced modern-lane proof on the showcase saver |
| `benchlab-gallery-gl46.txt` | forced premium-lane proof on the showcase saver |
| `benchlab-gallery-auto.txt` | auto-routing proof showing saver policy prefers `gl33` instead of silently jumping to `gl46` |
| `benchlab-nocturne-auto.txt` | ordinary-saver auto-routing proof through the compat lane |
| `benchlab-nocturne-gl46.txt` | ordinary-saver clamp proof showing requested `gl46` capped to `gl11` by saver policy |
| `benchlab-plasma-auto.txt` | future-`PL` product auto-routing proof through the compat lane |
| `benchlab-plasma-gl46.txt` | future-`PL` product clamp proof showing requested `gl46` capped to `gl11` by saver policy |

## Reproduction Pattern

All checked-in `SX09` reports were generated with BenchLab deterministic report mode:

```powershell
benchlab.exe /deterministic /seed:743 /saver:<product> /frames:4 /report:<path>
```

Forced-tier proofs add:

```powershell
/renderer:gdi
/renderer:gl11
/renderer:gl21
/renderer:gl33
/renderer:gl46
```

## Honest Limits

- These captures are BenchLab windowed-harness reports, not exhaustive fullscreen `.scr` host proof.
- The current validation machine supported the exercised GL tiers, so this capture set does not claim live denial proof for unavailable modern or premium tiers.
- The reports are deterministic and comparable within the harness. They do not claim universal cross-driver pixel identity.
