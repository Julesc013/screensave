# Plasma v2 Legacy Authority Boundary

Status: active PAW-I-R legacy authority repair note.

Legacy content is migration input only; Plasma v2 spec/plan/runtime own execution
truth.

This note classifies the older Plasma preset, theme, config, plan, render, and
treatment surfaces so they can remain useful without silently becoming the
runtime master for the Plasma v2 instrument.

## Product Center

The new product center is:

```text
plasma_v2_spec
-> plasma_v2_plan
-> plasma_v2_runtime
-> field
-> output
-> material
-> treatment
-> presentation
-> proof
```

`plasma_v2_spec` is the semantic instrument description.
`plasma_v2_plan` is the resolved executable truth after capability, size,
renderer, and degradation decisions.
`plasma_v2_runtime` owns deterministic state and buffers.

## Compatibility And Migration Inputs

These inputs may remain as compatibility shims, migration anchors, examples,
bookmarks, and curated starting points:

- `products/savers/plasma/src/plasma_config.c`
- `products/savers/plasma/src/plasma_presets.c`
- `products/savers/plasma/src/plasma_themes.c`
- `products/savers/plasma/src/plasma_content.c`
- `products/savers/plasma/src/plasma_selection.c`
- `products/savers/plasma/src/plasma_benchlab.c`
- `products/savers/plasma/presets/**`
- `products/savers/plasma/themes/**` when present

They are not hidden runtime masters, not uninspected plan mutators, not direct
render authorities, and not separate semantic engines for the Plasma v2
instrument. The required audit phrase is: not direct render authorities.

## Legacy Bypass Candidates

These older files may remain in the tree while legacy compatibility still
exists, but they are bypass candidates for the Plasma v2 instrument audit:

- `products/savers/plasma/src/plasma_sim.c`
- `products/savers/plasma/src/plasma_render.c`
- `products/savers/plasma/src/plasma_plan.c`
- `products/savers/plasma/src/plasma_advanced.c`
- `products/savers/plasma/src/plasma_modern.c`
- `products/savers/plasma/src/plasma_premium.c`
- `products/savers/plasma/src/plasma_output.c`
- `products/savers/plasma/src/plasma_treatment.c`
- `products/savers/plasma/src/plasma_presentation.c`

Their presence is not itself a release blocker. The blocker is any evidence
that they are the default semantic authority after a Plasma v2 spec has been
resolved.

## Gate Rule

`legacy_preset_authority_removed` may pass only when:

- legacy presets, themes, config, and packs lower through migration into
  `plasma_v2_spec`;
- legacy keys are not read as hidden runtime authority after plan compilation;
- runtime execution uses resolved `plasma_v2_plan`;
- Workbench inspection exposes requested, resolved, degraded, and legacy
  migration facts;
- packc examples compile into v2 specs;
- proof profiles refer to v2 spec/plan evidence.

This is a repair boundary only. It does not promote stable, publish a release,
certify compatibility, or accept final artistic quality.
