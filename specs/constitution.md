# Constitution

This document turns the rollout contract into repository-native rules.

## Project Intent

ScreenSave should evolve as a coherent Windows screensaver project with reconstructable history, truthful documentation, and auditable repair steps.

## Execution

- Execute queued work in order.
- Do not skip forward except for narrow prerequisite repairs.
- Treat failures as repair work, not as reasons to abandon the series.

## Commit Discipline

- Make at least one real commit per series.
- Use explicit series-tagged commit messages such as `S00 constitution and codex control plane`.
- Do not squash or rewrite history during the rollout.

## Verification

- Review changed files after each series.
- Run the strongest meaningful validation available for the current stage.
- Update docs and changelog in the same series as the change.
- Prefer truthful limited validation over unsupported claims.

## Auditability

- Keep `README.md`, `CHANGELOG.md`, `docs/`, and `specs/` aligned with the repository state.
- Prefer explicit deferred-work notes over silent omission.
- Avoid mystery files with no ownership explanation.

## Repair Rules

- Narrowly repair earlier work when later series reveal contradictions or breakage.
- Do not preserve known-bad docs or stale assumptions once they become relevant.
- Prefer the smallest durable fix that restores coherence.
