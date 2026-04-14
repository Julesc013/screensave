# Plasma U00 Diagnostic Notes

## Purpose

This note records the evidence used to refresh the U00 diagnostic baseline
against the current repository snapshot.

It is not a new proof harness.
It is the grounding note for the corrective-program freeze.

## Surfaces Inspected

Governance and control surfaces:

- `AGENTS.md`
- `products/AGENTS.md`
- `.codex/config.toml`
- `docs/roadmap/mx-program.md`
- `docs/roadmap/mx-wave-gates.md`
- `docs/roadmap/mx-codex-queue-contract.md`
- `docs/roadmap/mx-owner-map.md`

Current Plasma posture and doctrine:

- `products/savers/plasma/README.md`
- `products/savers/plasma/manifest.ini`
- `products/savers/plasma/docs/README.md`
- current `pl*.md`, `px*.md`, and `u*.md` docs relevant to runtime shape,
  settings, BenchLab, proof posture, and release posture
- `products/savers/plasma/tests/README.md`

Implementation and support surfaces inspected directly:

- `products/savers/plasma/src/plasma_plan.c`
- `products/savers/plasma/src/plasma_plan.h`
- `products/savers/plasma/src/plasma_config.c`
- `products/savers/plasma/src/plasma_config.rc`
- `products/savers/plasma/src/plasma_settings.c`
- `products/savers/plasma/src/plasma_output.c`
- `products/savers/plasma/src/plasma_treatment.c`
- `products/savers/plasma/src/plasma_presentation.c`
- `products/savers/plasma/src/plasma_transition.c`
- `products/savers/plasma/src/plasma_benchlab.c`
- `products/savers/plasma/src/plasma_validate.c`
- `tools/scripts/plasma_lab.py`

## Local Commands Run

The diagnostic refresh was grounded with:

```powershell
python tools\scripts\plasma_lab.py validate
python tools\scripts\plasma_lab.py authoring-report
python tools\scripts\plasma_lab.py preset-audit
.\out\msvc\vs2022\Debug\plasma\plasma_smoke.exe
git diff --check
```

## What Was Verified Locally

- authored Plasma substrate validated successfully
- current compiled preset inventory and theme inventory were available to the
  audit tooling
- the preset audit reported no exact duplicate compiled signatures and still
  exposed remaining close pairs
- the local smoke binary exited successfully
- the current baseline still advertises the expected product identity and
  routing posture

## What This Note Does Not Verify

- broad cross-hardware visual acceptance
- universal screenshot or image-diff parity
- qualitative aesthetic judgment beyond the bounded current proof surfaces
- live external consumer behavior for `suite` or `anthology`

## Toolchain Notes

No compiler or runtime blocker prevented the baseline refresh.

The repo still relies on bounded local validation.
That remains a proof limitation, not a U00 blocker.

## What Later Proof Work Still Must Do

- keep dead-setting and near-duplicate detection executable rather than
  narrative-only
- broaden visual acceptance evidence if stable widening is proposed later
- continue distinguishing supported surfaced grammar from unsupported enum or
  import vocabulary
- keep requested, resolved, and degraded truth aligned across runtime,
  BenchLab, docs, and proof notes
