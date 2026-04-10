# PX40 Authoring And Ops Tools

## Purpose

This note records the actual validator, linter, migration, checker, diff, and degradation-inspection surfaces landed by `PX40`.

## Tool Surface Landed

The current product-local tool surface is `python tools/scripts/plasma_lab.py`.

## What Each Tool Does

| Command | What It Does | What It Does Not Do |
| --- | --- | --- |
| `validate` | validates authored preset sets, theme sets, journeys, pack provenance, and the shared SDK pack shell against current repo truth | does not claim exhaustive runtime validation or broader ecosystem validation |
| `authoring-report` | reports compiled versus authored inventory, channel posture, aliases, and current Lab boundary | does not author files or migrate the compiled preset/theme registry into authored files |
| `compare` | compares authored preset sets, theme sets, and journeys textually | does not compare rendered output |
| `compat-report` | reports scope, member-channel mix, journey references, and pack routing or migration posture | does not define suite-wide compatibility law |
| `migration-report` | reports alias posture, current format versions, read-only migration guidance, and the compiled-versus-authored boundary | does not rewrite files automatically |
| `degrade-report --pack` | reports pack routing and migration posture from the current pack shell | does not inspect runtime captures |
| `degrade-report --capture` | reports requested, resolved, degraded, clamp, output, treatment, and presentation truth from an existing BenchLab text capture | does not claim fresh rerendered proof or pixel analysis |
| `capture-diff` | compares two deterministic BenchLab text captures semantically over the parsed report fields | does not promise exact raw-text identity or pixel-perfect determinism across every lane |

## Compatibility And Degradation Semantics

The current semantics are intentionally report-first:

- authored set and journey scope compatibility is checked against the current stable or experimental channel boundaries
- pack compatibility is taken from the checked-in `pack.ini` and `pack.provenance.ini` shell
- capture degrade reporting is limited to existing BenchLab text captures and the fields already exported there
- deterministic capture diff compares parsed report truth, not rendered pixels

## CLI Expectations

The current toolchain assumes:

- file-first authored content under `products/savers/plasma/`
- existing BenchLab text captures under `validation/captures/`
- read-only usage by default

No command in this tranche performs destructive rewrites.

## Current Limitations And Support Caveats

The current tool surface remains bounded:

- the built-in preset and theme descriptor inventory is still primarily compiled and legacy-INI anchored
- migration help is read-only and advisory
- capture comparison is only honest for existing BenchLab text captures
- no claim is made that every output family, treatment family, or lane is equally diffable
- no claim is made that the Lab is a full editor or a suite-grade workstation

## Scope Boundary

These tools are product-local Plasma support surfaces.
They do not create shared suite, SDK, automation, or community contracts.
