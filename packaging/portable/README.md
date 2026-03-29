# Portable Packaging

Purpose: source-controlled definition and assembly logic for the end-user portable saver bundle.
Belongs here: bundle manifests, layout notes, assembly scripts, and portable release-support docs.
Does not belong here: product implementation logic, installer behavior, or fabricated binary artifacts.
Type: release support.

## C06 Baseline

`C06` makes the first real portable bundle path live for the canonical saver line.

Source-controlled portable inputs now live here:

- `bundle_manifest.ini`: explicit bundle name, inclusion policy, doc sources, and saver-output expectations
- `layout.md`: durable staged-layout and inclusion policy note
- `assemble_portable.py`: stdlib-only assembly script that stages a portable folder and zip from real discovered outputs

Generated bundle output does not belong here.
The assembly script writes generated staging and zip artifacts under `out/portable/`.

## Inclusion Rules

- Include only canonical saver `.scr` files that actually exist in the configured output roots.
- Exclude legacy-named saver outputs from the portable bundle even if they remain on disk from older builds.
- Exclude `benchlab`, `suite`, smoke utilities, and other developer-facing artifacts from the end-user bundle.
- Stage file-backed packs only when the owning saver binary is present in the bundle.
- Record missing canonical saver outputs explicitly instead of fabricating placeholders.

## Current Prompt Boundary

This directory defines portable delivery only.
Installed distribution now lives beside it under `packaging/installer/`.
