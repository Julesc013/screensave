# Queue Manifests

This directory contains the canonical wave manifests for the post-PL control plane.

## Purpose

The queue manifests keep later work:

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

The wave files define planning truth.
They do not by themselves open a gate.

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
