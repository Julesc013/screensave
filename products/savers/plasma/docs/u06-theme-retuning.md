# Plasma U06 Theme Retuning

## Purpose

This note records the `U06` retune of Plasma's surviving theme catalogue.

The goal is not to invent new themes.
The goal is to make the surviving themes carry stronger tonal identity and more
supportable dark-room behavior.

## Retuned Themes

| Theme | Primary RGBA | Accent RGBA | U06 identity |
| --- | --- | --- | --- |
| `plasma_lava` | `204, 52, 12, 255` | `255, 174, 48, 255` | hotter ember reds with a brighter orange crest and a darker black floor |
| `aurora_cool` | `18, 124, 166, 255` | `104, 248, 214, 255` | colder cyan foundations with sharper mint energy |
| `oceanic_blue` | `12, 54, 142, 255` | `104, 198, 252, 255` | deeper cobalt body with glacial interference highlights |
| `museum_phosphor` | `20, 132, 44, 255` | `172, 255, 150, 255` | measured green phosphor built for monochrome long-run use |
| `quiet_darkroom` | `68, 30, 22, 255` | `132, 70, 48, 255` | darker cinder warmth with lower-contrast dark-room separation |
| `midnight_interference` | `8, 24, 76, 255` | `92, 170, 246, 255` | blue-black interference tones with colder electric lift |
| `amber_terminal` | `86, 64, 16, 255` | `216, 168, 46, 255` | warmer brass and amber phosphor with bounded terminal crest highlights |

## What Improved

The U06 theme pass makes the remaining themes read more like identities and less
like small palette perturbations.

- the warm stable baseline and the amber compatibility theme now separate more
  clearly instead of clustering around the same orange-red band
- the green phosphor and amber terminal monochrome themes now read as different
  display identities instead of adjacent monochrome swaps
- the quiet dark-room theme now stays darker and lower-contrast without
  collapsing all the way into a muddy warm-black wash
- the two cool-blue themes still remain related, but `midnight_interference`
  now reads colder and darker than `oceanic_blue`

## Current Bounds

The deterministic palette audit is better, but still intentionally bounded:

- the closest theme pair is now `midnight_interference <-> oceanic_blue` at
  distance `146`
- the previous warm clustering is reduced; `amber_terminal <-> plasma_lava`
  moved from a much tighter earlier pairing to distance `181`

This is strong enough for the current retune checkpoint.
It is not the later screenshot-grade quality bar.

## Dark-Room And Long-Run Supportability

The theme pass specifically improves the two most support-sensitive stable
themes:

- `museum_phosphor` now keeps clearer long-run contrast instead of drifting
  toward green washout
- `quiet_darkroom` now supports calmer dark-room presentation without needing a
  brighter accent just to stay legible

## Still Bounded Or Weak

`U06` does not claim that every surviving theme is already perfect.

- the cool-blue family still needs later screenshot-backed review
- the compatibility-only themes still exist for preserved identity first and
  final quality second
- the repo still lacks the later formal QA bar that decides whether every
  surviving theme is good enough to stay first-class
