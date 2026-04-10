# PX41 Curation Proof

## Purpose

This note records the actual commands used to exercise the bounded `PX41` local curation and provenance surfaces.

## Commands Used

```text
python tools/scripts/plasma_lab.py validate
python tools/scripts/plasma_lab.py curation-report
git diff --check
```

## Curation, Provenance, And Community-Facing Surfaces Exercised

- `curated_collections.ini`
- `provenance_index.ini`
- validation of collection-to-set, collection-to-pack, and provenance-index-to-pack alignment

## What Passed, Failed, Or Was Blocked

- Passed: stable collections stayed stable-only
- Passed: experimental sampler remained explicitly experimental
- Passed: the provenance index matched the current `lava_remix` pack provenance sidecar on the admitted shared fields
- Blocked: no broader user-pack or online provenance source exists in the repo for wider proof

## Evidence Versus Documentation Only

Actual evidence in this tranche is:

- the checked-in collection and provenance metadata files
- the expanded validation logic in `plasma_lab.py`
- the `curation-report` output

Documentation-only claims avoided:

- ratings or dislikes
- online index behavior
- shared provenance or trust law
