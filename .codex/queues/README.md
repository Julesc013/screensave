# Queue Manifests

This directory contains the historical wave manifests for the post-PL control plane.
After the Plasma `U09` corrective recut and the Truth And Proof Baseline, these files are lineage records rather than the live execution queue.

## Purpose

The queue manifests originally kept later work:

- grouped by wave
- owner-tagged
- dependency-aware
- haltable when a gate is still closed

## How Ask And Code Epics Are Represented

Each epic is represented as one queue entry with a paired mode list:

- `ASK`
- `CODE`

The queue manifest records the pair once.
The paired prompt work still runs as distinct `ASK` and `CODE` tasks.

## How Waves Are Represented

- one TOML file per wave
- one root wave record per file
- one `[[epics]]` table per queued epic in that wave

The wave files define historical planning truth.
They do not by themselves open a gate, and they no longer override [`../../PROJECT_STATE.toml`](../../PROJECT_STATE.toml).

## Halt And Gate Rules

- a later wave stays blocked until the earlier wave gate passes
- conditional epics stay blocked until the control plane records a positive admit decision
- no epic should widen beyond its declared scope just because it is already in the queue

## Naming Conventions

- files use `wave0.toml` through `wave5.toml`
- epic IDs use the frozen namespace form such as `MX00`, `SY10`, or `PX30`
- owner tags use `P`, `S`, or `H`

## Field Conventions

Current queue entries use these fields:

- `id`
- `owner_tag`
- `title`
- `modes`
- `depends_on`
- `conditional`
- `scope`
- `superseded_by`
- `superseded_note`

Wave files use these root fields:

- `wave_id`
- `status`
- `gate_ref`
- `summary`

## Owner Tags And Dependencies

- `owner_tag` records the controlling owner class for the epic
- `depends_on` records the epic-level dependency chain
- `gate_ref` points to the canonical wave gate definition in `docs/roadmap/mx-wave-gates.md`

Keep the schema small and human-readable.
Add new fields only when later queue work proves they are necessary.

## Current Status

`wave0.toml` through `wave5.toml` are all marked `historical-superseded`.
Use `PROJECT_STATE.toml` and `docs/roadmap/truth-proof-baseline.md` for the live current-state answer.
