# Portable Packaging

Purpose: source-controlled definition and assembly logic for the `ScreenSave Core` portable ZIP candidate.
Belongs here: bundle manifests, layout notes, assembly scripts, and Core-channel release-support docs.
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

## C15 Doctrine

`C15` names this portable ZIP path as the `ScreenSave Core` channel.

- Core is the primary product for normal users.
- This directory defines the portable ZIP source surface for Core.
- Installer, `suite`, BenchLab, SDK material, and Extras stay outside Core by default.

## C14 Refresh

`C14` refreshes the portable surface for the final release-candidate pass:

- the bundle manifest now tracks the full canonical saver line, including `anthology`
- staged output now moves to the current `C14` release-candidate bundle root under `out/portable/`
- bundle docs now include final release-readiness, known-issues, and config-integrity notes
- `suite` and `benchlab` remain separate app products and are excluded from the end-user saver payload
- the refreshed `C14` stage is now interpreted by `C15` as the pre-freeze Core candidate surface

## Inclusion Rules

- Include only canonical saver `.scr` files that actually exist in the configured output roots.
- Exclude legacy-named saver outputs from the portable bundle even if they remain on disk from older builds.
- Exclude the Installer channel, `suite`, BenchLab, smoke utilities, SDK material, and other companion or developer-facing artifacts from the Core bundle.
- Treat `anthology` as a normal saver product and stage it when its real binary exists.
- Stage file-backed packs only when the owning saver binary is present in the bundle.
- Record missing canonical saver outputs explicitly instead of fabricating placeholders.
- Use `OPTIONAL/` only for clearly labeled Extras material; do not treat it as a dumping ground for companion tools.

## Current Prompt Boundary

This directory defines the Core ZIP channel only.
Installed distribution now lives beside it under `packaging/installer/` as the optional Installer channel.
