# MX Codex Queue Contract

This document defines the queue contract for post-PL `ASK` and `CODE` epics.

## Purpose

The queue exists to keep later work:

- wave-gated
- owner-scoped
- proof-bearing
- haltable when the repo truth does not support execution

## Ask And Code Pairing Rule

Every queued epic is represented as a paired `ASK` and `CODE` mode.

- `ASK` decides whether the proposed `CODE` epic is safe, scoped, and gate-valid.
- `CODE` executes only the approved write surface.
- `CODE` must not broaden scope beyond what the corresponding `ASK` decision approved.
- A prompt-local instruction may require a more specific `ASK` or `CODE` output format, but it does not remove the pairing rule.

## Required Queue Fields

Each queued epic must declare at least:

- epic `id`
- short `title`
- `owner_tag`
- `modes`
- `depends_on`
- `conditional`
- `gate_ref`
- high-level `scope`

Optional notes may be added later, but the queue contract should stay small and human-readable.

## Ask Output Expectations

An `ASK` task must:

- restate the repo-grounded baseline that matters for the epic
- identify the exact intended write surface
- confirm owner and dependency assumptions
- state gate implications and proof-surface obligations
- return one of:
  - `PROCEED`
  - `PROCEED_WITH_SPLIT`
  - `BLOCKED`

Default `ASK` final format:

1. repo-grounded baseline
2. recommended write surface
3. owner and dependency implications
4. gate and proof implications
5. decision

Prompt-local exact section requirements override this default when present.

## Code Output Expectations

A `CODE` task must:

- implement only the approved scope
- keep the write set tight
- update the declared docs, queue, and proof surfaces in the same pass
- verify the changed surface before finalizing
- report any unresolved blocker explicitly instead of silently widening scope

Default `CODE` final format:

1. files changed
2. summary of frozen or implemented decisions
3. validation run
4. unresolved blockers, if any
5. confirmation that scope stayed within the approved epic

Prompt-local exact section requirements override this default when present.

## Decision Rules

Use `PROCEED` when:

- one `CODE` epic can land the scoped result safely

Use `PROCEED_WITH_SPLIT` when:

- the requested control or feature surface is real, but one `CODE` epic would be too broad or ambiguous

Use `BLOCKED` when:

- prerequisites are missing
- the gate is not open
- the write surface cannot be identified safely
- the requested work conflicts with current repo truth or governing doctrine

## Automatic Halt Conditions

Stop and report instead of continuing when:

- the wave gate is closed
- the repo reality contradicts the assumed baseline materially
- a shared promotion is not justified but the task depends on it
- the required proof surface cannot be updated honestly
- the write surface would spill into runtime or spec work outside the approved epic
- live repo changes appear in the target files and create a scope conflict

## Write-Surface Discipline

- `ASK` decides the write surface.
- `CODE` should not exceed it without a new decision pass.
- Keep edits local and operational.
- Do not use queue work as a backdoor for runtime feature implementation.
- Do not silently edit specs or stable product doctrine unless the epic explicitly owns that contract.

## Parallelism Guidance

- Parallel work is acceptable only when write scopes are disjoint and the gate is already open.
- Do not queue a dependent epic in parallel with the epic that defines its contract.
- Shared and Plasma-local follow-ons should split rather than race each other when they write the same truth surfaces.

## Gate Discipline

- A queued epic is admissible only when its `depends_on` chain is satisfied and its `gate_ref` is open.
- Conditional epics remain blocked until the control plane records a positive admit decision.
- Later-wave planning may exist on disk before admission, but planning presence does not equal gate passage.

## Proof Discipline

No feature or contract slice is done until its declared proof surfaces are updated.

That includes, where relevant:

- docs
- product tests and proof notes
- validation captures
- BenchLab reporting surfaces
- known-limit updates
- risk-register or feature-ledger updates

## Final Response Rule

Queued tasks must end with a concrete close-out that makes these visible:

- what changed
- what decisions were frozen or implemented
- what was verified
- whether any blocker remains
- whether the scope stayed within the approved epic
