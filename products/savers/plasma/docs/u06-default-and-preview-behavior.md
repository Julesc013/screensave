# Plasma U06 Default And Preview Behavior

## Purpose

This note records the default-path, preview, long-run, dark-room, and bounded
transition retunes that landed with `U06`.

It freezes the current retuned behavior before the later proof-harness and
visual-QA phases.

## Resulting Stable Default Path

The stable out-of-box path stays compatibility-safe:

- preset: `plasma_lava`
- theme: `plasma_lava`
- minimum kind: `gdi`
- preferred kind: `gl11`
- quality class: `safe`
- resolved stable grammar: `fire` + `gentle` + `standard` + `soft` +
  `raster/native_raster` + no treatments + `flat`
- transitions remain disabled by default

What changed in `U06` is not the identity.
What changed is the tuning around that identity.

## Preview And Settings-Preview Retune

Preview behavior is now closer to the fullscreen intent without pretending the
two contexts are identical.

- preview resolution penalty was reduced from `+2` divisors to `+1`
- preview variation cadence moved from `7000ms` to `9000ms`
- preview fire-floor reduction softened from `20` to `16`
- preview smoothing cap rose from `64` to `80`
- `preview_safe` now uses `resolution=standard` and `smoothing=soft`

This makes the preview path less blurry and less frantic while keeping it cheap
enough for the config dialog and windowed harness.

## Fullscreen And Long-Run Retune

Long-run behavior now holds shapes longer and reduces the sense that every
preset is trying to mutate too quickly.

| Effect family | U06 variation interval |
| --- | --- |
| preview mode | `9000ms` |
| interference | `12500ms` |
| lattice | `12000ms` |
| chemical | `14000ms` |
| caustic | `16500ms` |
| aurora | `13500ms` |
| substrate | `17500ms` |
| arc | `10500ms` |
| fire | `18000ms` |
| fallback/default | `14500ms` |

The fire floor was also retuned:

- gentle: `150`
- standard: `172`
- lively: `198`

That change keeps the warm presets alive without leaving them as a constantly
blown-out lower-band fire wash.

## Dark-Room Support

The stable dark-room support path is now more intentional:

- `dark_room_support` uses `quiet_darkroom`
- it stays deterministic at seed `8080`
- it now uses `detail_level=low`
- it routes through `dark_room_classics` and `dark_room_cycle`

This is a support and preview-review profile, not a promise that BenchLab can
force the full preset bundle on demand.

## Transition Default Retune

The bounded supported transition subset now defaults to calmer timing:

- transition interval default: `14000ms`
- transition duration default: `1800ms`

The authored stable journeys were retuned alongside those defaults:

| Journey | U06 dwell notes |
| --- | --- |
| `classic_cycle` | main fire step `12000ms`; dark-room dwell `16000ms` |
| `dark_room_cycle` | `18000ms` dwell |
| `warm_bridge_cycle` | `12000ms` dwell |
| `cool_bridge_cycle` | `12000ms` dwell |

## Honest Limits

`U06` improves the supported subset only.

- it does not claim fullscreen `.scr` host captures yet
- it does not claim universal parity across every lane and renderer
- it does not claim BenchLab can force the entire preset bundle; BenchLab still
  forces preset identity and a bounded forcing subset, not a full preset replay

Those remain later proof and QA work.
