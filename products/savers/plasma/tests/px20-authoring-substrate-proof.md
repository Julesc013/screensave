# PX20 Authoring Substrate Proof

## Purpose

This note records the actual evidence for the new authored set, journey, and provenance substrate landed by `PX20`.

## Commands And Harness Steps Used

Commands run in this checkout:

- `python tools/scripts/plasma_lab.py validate`
- `python tools/scripts/plasma_lab.py compare --kind preset-set --left classic_core --right dark_room_classics`
- `python tools/scripts/plasma_lab.py compare --kind journey --left warm_bridge_cycle --right cool_bridge_cycle`

Repo surfaces exercised:

- `products/savers/plasma/preset_sets/`
- `products/savers/plasma/theme_sets/`
- `products/savers/plasma/journeys/`
- `products/savers/plasma/packs/lava_remix/pack.provenance.ini`

## What Passed, Failed, Or Was Blocked

Passed in this checkout:

- authored preset sets validate structurally and reference known preset keys
- authored theme sets validate structurally and reference known theme keys
- authored journeys validate structurally and reference known set keys
- authored compare output demonstrates real weighted-set and journey-step differences

Blocked in this checkout:

- no compiled runtime rerun was available to execute the new smoke-backed authoring hook locally

## What Remains Later-Wave Work

- migrating more built-in content descriptors into authored files
- broader authored discovery roots
- richer migration tooling
- live editing or preview workflows

## Current Conclusion

The authored substrate is now real on disk and usable by tooling, but it remains a partial first substrate rather than a finished authoring system.
