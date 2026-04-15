# U06 Captures

This directory contains the focused deterministic reports gathered for the
`U06` preset, theme, default-path, and pacing retune.

It is intentionally smaller than the later `U07` proof harness.

## Capture Set

| File | Purpose |
| --- | --- |
| `benchlab-plasma-default-gdi.txt` | proves the preserved `plasma_lava` default path still presents on the truthful `gdi` floor |
| `benchlab-plasma-default-gl11.txt` | proves the same default path still presents on the truthful `gl11` preferred stable lane |
| `benchlab-plasma-classic-cycle-gl11.txt` | proves the retuned stable classic journey path still resolves cleanly with transitions enabled |
| `control-profile-deterministic-classic.txt` | records the stable deterministic classic review profile after the U06 retune |
| `control-profile-dark-room-support.txt` | records the stable dark-room review profile after the U06 retune |
| `capture-diff-default-gdi-vs-gl11.txt` | isolates the routing and renderer delta between the two stable default-lane captures |
| `capture-diff-default-gl11-vs-classic-cycle.txt` | isolates the journey and transition delta between the default `gl11` path and the classic-cycle retune capture |

## Reproduction Pattern

The BenchLab reports were generated with deterministic report mode:

```powershell
benchlab.exe /deterministic /seed:<seed> /saver:plasma /frames:4 /report:<path>
```

The U06 set adds:

```powershell
/renderer:gdi
/renderer:gl11
/plasma-transitions:true
/plasma-transition-policy:journey
/plasma-journey:classic_cycle
```

The control-profile reports were generated with:

```powershell
python tools\scripts\plasma_lab.py control-report --profile <profile_key>
```

## Honest Limits

- These captures are BenchLab and report-first evidence, not fullscreen `.scr`
  host proof by themselves.
- BenchLab still forces preset identity and a bounded forcing subset; it does
  not replay the full preset bundle.
- The directory is a retuning checkpoint, not the later universal proof
  harness.
