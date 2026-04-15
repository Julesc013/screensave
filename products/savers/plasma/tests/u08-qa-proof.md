# Plasma U08 QA Proof

## Purpose

This note records the actual proof and report evidence consumed by the `U08`
visual QA gate.

## Commands And Harness Steps Actually Used

Tooling and report checks:

```powershell
python -m py_compile tools\scripts\plasma_lab.py
python tools\scripts\plasma_lab.py qa-bar
python tools\scripts\plasma_lab.py surface-evaluation
python tools\scripts\plasma_lab.py acceptance-decisions
python tools\scripts\plasma_lab.py validate
python tools\scripts\plasma_lab.py control-report --profile deterministic_classic
python tools\scripts\plasma_lab.py control-report --profile dark_room_support
python tools\scripts\plasma_lab.py curation-report
python tools\scripts\plasma_lab.py preset-audit --threshold 3
```

Current smoke check:

```powershell
.\out\msvc\vs2022\Debug\plasma\plasma_smoke_u07.exe
```

Capture refresh:

```powershell
New-Item -ItemType Directory -Force validation\captures\u08 | Out-Null

python tools\scripts\plasma_lab.py qa-bar | Out-File -Encoding ascii validation\captures\u08\qa-bar.txt
python tools\scripts\plasma_lab.py surface-evaluation | Out-File -Encoding ascii validation\captures\u08\surface-evaluation.txt
python tools\scripts\plasma_lab.py acceptance-decisions | Out-File -Encoding ascii validation\captures\u08\acceptance-decisions.txt
python tools\scripts\plasma_lab.py control-report --profile deterministic_classic | Out-File -Encoding ascii validation\captures\u08\control-profile-deterministic-classic.txt
python tools\scripts\plasma_lab.py control-report --profile dark_room_support | Out-File -Encoding ascii validation\captures\u08\control-profile-dark-room-support.txt
python tools\scripts\plasma_lab.py curation-report | Out-File -Encoding ascii validation\captures\u08\curation-report.txt
python tools\scripts\plasma_lab.py preset-audit --threshold 3 | Out-File -Encoding ascii validation\captures\u08\preset-audit-threshold-3.txt
```

## Which Proof Sources Were Consumed

`U08` consumed:

- `u06-retuning-proof.md`
- `u07-settings-influence-proof.md`
- `u07-combination-proof.md`
- `u07-diff-and-duplicate-proof.md`
- the current `plasma_lab.py` U08 reports
- the refreshed stable control-profile and curation reports
- the current smoke executable used in `U07`

## Evidence-Backed Acceptance Decisions

Evidence-backed stable decisions:

- Basic dialog surface
- default `plasma_lava` plus `plasma_lava` path
- `classic_core` preset and theme pools
- `classic_core_featured` and `dark_room_support` stable collections after
  journey-key removal
- `deterministic_classic` and `dark_room_support` stable support profiles after
  journey-key removal
- `raster` / `native_raster`
- `flat`

Evidence-backed experimental decisions:

- Advanced dialog surface
- banded and contour output families
- direct treatment controls
- `wave3_experimental_sampler`

Evidence-backed experimental-with-caveat decisions:

- Author/Lab dialog surface
- glyph output
- `heightfield` and `ribbon`
- transitions, journeys, and timing controls

Evidence-backed demotion or claim-surface removal decisions:

- diagnostics overlay
- hidden `favorites_only` compatibility state
- compatibility-only preset and theme aliases

## What Remained Partial Or Blocked

Partial:

- transitions and journeys remain bounded by partial proof rather than a fully
  visual QA-grade transition lab
- deterministic seed replay identity is still stronger as structural proof than
  as a screenshot-grade visual claim
- premium presentation remains bounded by explicit lane and degrade caveats

Blocked:

- no new cross-hardware screenshot-grade QA lab was added in this tranche
- no universal pixel-perfect acceptance framework exists beyond the bounded
  deterministic smoke and capture substrate already in the repo

## Actual Evidence Versus Documentation-Only Reasoning

Actual evidence:

- generated U08 `qa-bar`, `surface-evaluation`, and `acceptance-decisions`
  reports
- refreshed control-profile and curation reports
- refreshed preset-signature and theme-palette audit
- current stable smoke executable run
- previously generated but still authoritative U06 and U07 proof notes and
  captures

Documentation-only reasoning:

- any future choice U09 makes about final ship-language polish
- any future promotion or cut beyond the explicit U08 result classes
