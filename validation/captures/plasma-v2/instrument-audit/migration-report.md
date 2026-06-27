# Plasma v2 Legacy Migration Report

- Status: pass
- Fixtures checked: 5
- All legacy entries reduce to v2 spec: True
- Hidden preset authority detected: False
- Claim boundary: Migration proof only; not stable promotion.

## Fixtures

- plasma_lava / plasma_lava -> field=classic, material=plasma_lava, speed=420, scale=420, treatment=soft
- aurora_plasma / aurora_cool -> field=aurora, material=aurora_cool, speed=700, scale=560, treatment=soft
- ocean_interference / oceanic_blue -> field=interference, material=oceanic_blue, speed=700, scale=360, treatment=none
- museum_phosphor / museum_phosphor -> field=interference, material=museum_phosphor, speed=220, scale=420, treatment=phosphor
- quiet_darkroom / quiet_darkroom -> field=fire, material=quiet_darkroom, speed=180, scale=680, treatment=none

## Rules

- effect_mode -> field_family
- speed_mode -> speed
- resolution_mode -> field-size / internal-resolution policy
- smoothing_mode -> softness or treatment
- preset_key -> starter plasma_v2_spec
- theme_key -> material key
- output_mode -> output kind
- presentation_mode -> presentation kind
- deterministic_seed -> same seed field

## Errors

- none
