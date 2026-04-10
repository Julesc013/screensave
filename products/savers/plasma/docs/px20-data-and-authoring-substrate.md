# PX20 Data And Authoring Substrate

## Purpose

`PX20` lands the first real Wave 2 Plasma substrate for authored data, provenance, validation, and bounded authoring support.
It exists so later Wave 3 work can grow from authored files and validation surfaces instead of adding more hard-coded product assumptions.

## Why PX20 Exists Now

- `PX11` broadened and clarified the bounded transition surface, but Plasma still needed a stronger authored substrate underneath that surface.
- `SY20` did not promote a new shared authoring contract, so the richer Wave 2 substrate remains Plasma-local on purpose.
- Wave 3 should consume authored sets, journeys, provenance, and validation surfaces rather than inventing new visual-language breadth on top of compiled-only gaps.

## Relationship To SY20, PX11, And Wave 3

- `SY20` remains effectively `KEEP_LOCAL_FOR_PX20`. `PX20` consumes only the already-existing shared outer shell for presets, themes, and `pack.ini`.
- `PX11` remains the bounded transition and posture hardening tranche that `PX20` builds on.
- `PX30`, `PX31`, and `PX32` may now rely on authored set and journey files, a Plasma-local provenance sidecar, a minimal Lab shell, and the first real selection-foundation hooks.

## The Five PX20 Domains

1. pack schema, provenance, and compatibility hardening
2. authorable preset, theme, set, and journey substrate
3. minimal Plasma Lab shell
4. validator, comparer, and degradation-report core
5. selection-intelligence foundation

## What PX20 Actually Establishes

- `lava_remix` now has a Plasma-local `pack.provenance.ini` sidecar that sits beside the shared `pack.ini` shell.
- Plasma preset sets, theme sets, and journeys are now real checked-in authored files under product-local directories.
- A product-local authoring validator now checks that those authored files still match the compiled Plasma registry and journey inventory.
- `python tools/scripts/plasma_lab.py` provides the first bounded `Plasma Lab` shell for validate, compare, and degrade-report workflows.
- Set weights are now materially real in selection fallback rather than decorative metadata only.

## What PX20 Intentionally Leaves Unchanged

- product name and identity remain `Plasma` with preserved `Plasma Classic`
- stable defaults remain `plasma_lava` plus `plasma_lava`
- `gdi` remains the real floor and `gl11` remains the truthful preferred stable lane
- stable-only default posture remains intact
- no new field, output, treatment, presentation, or dimensional family is added
- no transition-engine redesign, settings-surface redesign, BenchLab redesign, or release-cut rewrite is attempted here

## What Remains Later-Wave Work

- full migration of all built-in preset and theme descriptors into richer authored files
- portable or user discovery roots for authored sets and journeys
- a GUI editor or larger Lab workstation
- broader migration, import or export workflows
- richer anti-repeat memory, similarity reasoning, safety-tag reasoning, and larger selection intelligence

## Scope Boundary

`PX20` is a substrate tranche.
It is not visual-language expansion, not a suite-wide authoring framework, and not a stable-scope widening pass.
