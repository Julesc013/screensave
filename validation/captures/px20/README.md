# PX20 Capture Surface

`PX20` is primarily a data, authoring, and validation substrate tranche.
Its most truthful reproducible evidence in this checkout is textual rather than rendered-image capture output.

## Commands Used

- `python tools/scripts/plasma_lab.py validate`
- `python tools/scripts/plasma_lab.py compare --kind preset-set --left classic_core --right dark_room_classics`
- `python tools/scripts/plasma_lab.py compare --kind journey --left warm_bridge_cycle --right cool_bridge_cycle`
- `python tools/scripts/plasma_lab.py degrade-report --pack lava_remix`
- `python tools/scripts/check_sdk_surface.py products/savers/plasma/packs/lava_remix`

## Current Evidence Shape

Current `PX20` evidence is:

- authored file validation output
- authored compare output
- bounded pack degrade-report output
- updated smoke assertions and validation catalog entries in source

## Why No Generated Output Files Are Checked In

This checkout does not have a local compiler toolchain or prebuilt Plasma smoke binary, so no fresh compiled runtime capture can be generated honestly here.
Checking in fabricated or hand-written output files would violate repo rules.

The checked-in proof for `PX20` therefore stays in:

- the authored data files themselves
- the Lab shell
- the proof notes
- the validation catalog and known-limit surfaces
