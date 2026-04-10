# PX20 Lab Shell Proof

## Purpose

This note records the actual evidence for the first bounded `Plasma Lab` shell landed by `PX20`.

## Commands And Harness Steps Used

Commands run in this checkout:

- `python tools/scripts/plasma_lab.py validate`
- `python tools/scripts/plasma_lab.py compare --kind preset-set --left classic_core --right dark_room_classics`
- `python tools/scripts/plasma_lab.py compare --kind journey --left warm_bridge_cycle --right cool_bridge_cycle`
- `python tools/scripts/plasma_lab.py degrade-report --pack lava_remix`

## Actual Outputs Observed

Observed compare output:

- `classic_core` versus `dark_room_classics` reported shared members, left-only members, and weight deltas
- `warm_bridge_cycle` versus `cool_bridge_cycle` reported the authored step difference explicitly

Observed degrade-report output:

- `lava_remix` reported `product=plasma`
- `source=built_in`
- `channel=stable`
- `support_tier=bounded_supported`
- routing remained `minimum_kind=gdi`, `preferred_kind=gl11`, `quality_class=balanced`

## What Passed, Failed, Or Was Blocked

Passed in this checkout:

- the Lab shell validates the authored substrate
- the Lab shell compares authored sets and journeys
- the Lab shell prints a bounded pack degrade report

Blocked in this checkout:

- the Lab shell is intentionally textual only
- no live preview or render-diff surface exists yet

## Current Conclusion

`PX20` now has a real minimal Lab foothold.
It is bounded, CLI-first, and honest about that boundary.
