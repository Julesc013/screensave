# Plasma U07 Proof Harness

## Purpose

This note records the salvage-phase proof harness added to catch fake controls, dead settings, and near-duplicate preset signatures before they ship.

## Deterministic Smoke Influence Checks

The smoke surface now includes deterministic render-signature checks that:

- lock the seed path
- render the same config on the same lane
- hash the treated and presented buffers
- compare bounded pixel-difference counts between a baseline and a one-setting variant

The smoke harness currently proves material influence for:

- generator family
- visual intensity through `detail_level`
- speed
- resolution
- smoothing
- output family and mode
- filter treatment
- emulation treatment
- accent treatment
- presentation mode on the premium lane

This is not a perceptual gold-standard image-diff system.
It is an honest bounded dead-setting detector that fails if those settings stop moving enough pixels to matter.

## Preset Signature Audit

`plasma_lab.py preset-audit` now audits the compiled preset table directly and reports:

- exact duplicate preset signatures
- near-duplicate pairs at a chosen Hamming-distance threshold
- closest stable-preset pair
- stable treatment-slot coverage
- nearest theme-palette pairs

The audit is intentionally signature-first rather than screenshot-first.
It is meant to catch authoring collapse early, even on machines where a full render-capture workflow is unavailable.

## Existing Invalid-Combination Proof

The salvage harness builds on the existing smoke and BenchLab proof that already checks:

- requested versus resolved versus degraded truth
- unsupported richer-lane treatment degrade
- unsupported presentation degrade back to `flat`
- invalid output-family and output-mode rejection
- stable versus experimental content gating

## Current Boundary

Plasma still does not ship a universal screenshot diff framework or a gallery browser.
The truthful proof harness is therefore:

- deterministic smoke render signatures
- BenchLab text captures
- compiled preset signature audits
- explicit known-limit notes where broader visual QA remains human-reviewed
