# Plasma U00 Salvage Baseline

## Purpose

This document freezes the factual starting point for the unification and salvage pass.
It records what the current repo actually does before the corrective rewiring begins.

## Commands Run

The baseline was frozen against the current clean `main` worktree with:

```powershell
& .\out\mingw\i686\debug\plasma\plasma_smoke.exe
python tools\scripts\plasma_lab.py validate
python tools\scripts\plasma_lab.py authoring-report
```

Observed results:

- `plasma_smoke.exe` exited `0`
- `plasma_lab.py validate` reported `OK`
- `plasma_lab.py authoring-report` reported `23` compiled presets, `7` compiled themes, `7` stable preset sets, `4` stable theme sets, and `4` stable journeys

These results prove that the existing bounded implementation is internally consistent.
They do not prove that the product surface is coherent or truthful.

## Runtime Truth

The current runtime is structurally broader than the old classic saver, but the resolved visual grammar is still dominated by preset-owned tables.

Current plan compilation truth:

1. selection resolves preset/theme
2. settings resolution contributes `effect_mode`, `speed_mode`, `resolution_mode`, `smoothing_mode`, detail, seed, and transition fields
3. the plan then looks up `plasma_find_preset_values(plan->preset_key)`
4. `output_family`, `output_mode`, all treatment slots, and `presentation_mode` come from that preset table rather than from user-facing settings

That means the product currently has one real runtime, but not one coherent settings-owned runtime grammar.

## Main Gaps

- `classic_execution` is still carried as a validation gate even though it is always initialized to `1`
- preset selection still hides the real output/treatment/presentation authority
- the dialog exposes effect, speed, resolution, and smoothing, but not the rest of the visual grammar the docs discuss
- BenchLab can report output/treatment/presentation truth, but ordinary settings cannot ask for most of that truth directly
- several enum values exist only on paper or remain unreachable from the product dialog

## Baseline Findings

- The current product is not literally split into two binaries or two engines, but it is still conceptually centered on a preserved classic baseline plus preset-driven sidecar breadth.
- The claimed output/treatment/presentation taxonomy is partly real in code, but mostly not settings-owned.
- The settings docs overstate how much of the visual grammar is actually controlled by the settings surface.
- The stable-versus-experimental split is explicit for content, but much less explicit for direct control surfaces.

## Companion Audits

- [U00 Settings Influence Matrix](./u00-settings-influence-matrix.md)
- [U00 Visual Distinctness Audit](./u00-visual-distinctness-audit.md)
- [U00 UI Gap Audit](./u00-ui-gap-audit.md)
