# U02 Captures

This directory contains the focused deterministic BenchLab reports gathered for
`U02`.

They are settings-authority evidence, not a broad visual retune or release-cut
capture set.

## Capture Set

| File | Purpose |
| --- | --- |
| `benchlab-plasma-degrade-heightfield-auto.txt` | shows that a requested premium-only presentation uplift remains visible as requested state while the resolved lower-lane path degrades back to `flat` |

## Reproduction Pattern

The checked-in `U02` report was generated with deterministic BenchLab report
mode:

```powershell
benchlab.exe /deterministic /seed:743 /saver:plasma /frames:4 /report:validation\captures\u02\benchlab-plasma-degrade-heightfield-auto.txt /plasma-presentation:heightfield
```

## Honest Limits

- This is a windowed BenchLab text report, not fullscreen `.scr` host proof by
  itself.
- The capture set is intentionally narrow because `U02` is a settings-authority
  phase, not the later visual-proof expansion.
- Requested-versus-resolved settings truth for other grammar combinations in
  this phase is primarily proved by the updated smoke harness.
