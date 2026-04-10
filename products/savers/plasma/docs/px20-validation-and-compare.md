# PX20 Validation And Compare

## Purpose

This note records the validator, comparer, and degradation-report substrate added by `PX20`.

## Landed Validation Surface

`PX20` adds two complementary validation paths:

1. a product-local C authoring validator
2. the minimal `plasma_lab.py` CLI shell

The C validator checks that:

- authored preset sets still match the compiled Plasma content registry
- authored theme sets still match the compiled Plasma content registry
- authored journeys still match the compiled transition surface
- `lava_remix` provenance still aligns with the shared `pack.ini`

The Python Lab shell checks that:

- authored files are structurally valid
- authored members reference known preset and theme keys
- journey steps reference known set keys
- pack provenance fields remain aligned with the existing shared pack shell

## Compare Surface

The current compare surface is intentionally textual.
It can compare:

- preset sets
- theme sets
- journeys

Current output is aimed at authoring and support review, not visual approval workflows.

## Degradation-Report Surface

`plasma_lab.py degrade-report --pack lava_remix` prints the current bounded routing and degrade story for the shipped pack shell:

- source kind
- channel
- support tier
- minimum and preferred lane
- quality class
- degraded behavior
- migration policy
- classic-identity preservation

## Current Limitations

- no rendered-image comparison
- no automatic migration rewrite pass
- no broad broken-content recovery framework
- no cross-product shared validation framework

That larger scope remains out of bounds for `PX20`.
