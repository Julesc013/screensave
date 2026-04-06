# Portable Packaging

Purpose: source-controlled definition and assembly logic for the frozen `ScreenSave Core` ZIP.
Belongs here: bundle manifests, layout notes, assembly scripts, and Core-channel release-support docs.
Does not belong here: product implementation logic, installer behavior, or fabricated binary artifacts.
Type: release support.

## Historical Baseline

`C06` made the first real portable bundle path live for the canonical saver line.
`C15` named that portable ZIP path as the Core channel.
`C16` refreshes the actual payload and freezes the real Core baseline.

Source-controlled portable inputs live here:

- `bundle_manifest.ini`: explicit Core bundle identity, inclusion policy, doc sources, and saver-output expectations
- `layout.md`: durable staged-layout and inclusion policy note
- `assemble_portable.py`: stdlib-only assembly script that stages a Core folder and zip from real discovered outputs

Generated bundle output does not belong here.
The assembly script writes generated staging and zip artifacts under `out/portable/`.

## C16 Frozen State

- Frozen Core staging root: `out/portable/screensave-core-c16-baseline/`
- Frozen Core zip: `out/portable/screensave-core-c16-baseline.zip`
- The frozen Core bundle includes the full current canonical saver line, including `anthology`.
- Installer, `suite`, BenchLab, SDK material, and Extras remain outside Core by default.

## Inclusion Rules

- Include only canonical saver `.scr` files that actually exist in configured output roots.
- Exclude legacy-named saver outputs even if they remain on disk from older builds.
- Exclude Installer assets, `suite`, BenchLab, SDK material, and Extras from the Core bundle.
- Treat `anthology` as a normal saver product and stage it only when it satisfies the same real artifact bar as the other savers.
- Stage file-backed packs only when the owning saver binary is present in the bundle.
- Record missing canonical saver outputs explicitly instead of fabricating placeholders.
- Keep `OPTIONAL/` reserved for clearly labeled Extras material.

## Current Prompt Boundary

This directory defines the Core ZIP channel only.
Installed distribution lives beside it under `packaging/installer/` as the companion Installer channel.
