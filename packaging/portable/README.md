# Portable Packaging

Purpose: source-controlled definition and assembly logic for the end-user portable saver bundle.
Belongs here: bundle manifests, layout notes, assembly scripts, and portable release-support docs.
Does not belong here: product implementation logic, installer behavior, or fabricated binary artifacts.
Type: release support.

## Historical Baseline

`C06` makes the first real portable bundle path live for the canonical saver line.

Source-controlled portable inputs now live here:

- `bundle_manifest.ini`: explicit bundle name, inclusion policy, doc sources, and saver-output expectations
- `layout.md`: durable staged-layout and inclusion policy note
- `assemble_portable.py`: stdlib-only assembly script that stages a portable folder and zip from real discovered outputs

Generated bundle output does not belong here.
The assembly script writes generated staging and zip artifacts under `out/portable/`.

## C14 Refresh

`C14` refreshes the portable surface for the final release-candidate pass:

- the bundle manifest now tracks the full canonical saver line, including `anthology`
- staged output now moves to the current `C14` release-candidate bundle root under `out/portable/`
- bundle docs now include final release-readiness, known-issues, and config-integrity notes
- `suite` and `benchlab` remain separate app products and are excluded from the end-user saver payload

## Inclusion Rules

- Include only canonical saver `.scr` files that actually exist in the configured output roots.
- Exclude legacy-named saver outputs from the portable bundle even if they remain on disk from older builds.
- Exclude `benchlab`, `suite`, smoke utilities, and other developer-facing artifacts from the end-user bundle.
- Treat `anthology` as a normal saver product and stage it when its real binary exists.
- Stage file-backed packs only when the owning saver binary is present in the bundle.
- Record missing canonical saver outputs explicitly instead of fabricating placeholders.

## Current Prompt Boundary

This directory defines portable delivery only.
Installed distribution now lives beside it under `packaging/installer/`.
