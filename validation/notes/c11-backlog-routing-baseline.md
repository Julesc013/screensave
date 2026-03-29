# C11 Backlog Routing Baseline

This note records the structured backlog and routing surface landed in `C11`.

## Included Planning Artifacts

- `docs/roadmap/backlog-routing.md`
- `docs/roadmap/backlog-inventory.md`
- `docs/roadmap/refinement-threads.md`

## Coverage Baseline

- All current saver products are represented in the active backlog inventory, including `anthology`.
- Key non-saver products are represented where relevant: `suite`, `benchlab`, and `sdk`.
- Working-title future concepts remain explicitly parked instead of competing with the active continuation line.

## Routing And Wave Baseline

- Routing categories are explicit and bounded.
- Priority, effort, risk, dependency, owner area, and target-bucket fields are now standard backlog metadata.
- Cross-cutting polish stays ahead of per-saver waves.
- Later renderer R&D and later release/distribution follow-on work remain deferred buckets, not active implementation promises.

## Operating Model

- Shared and saver-local work now route through the master/orchestration model defined in `refinement-threads.md`.
- Saver-local refinement threads do not own shared platform or renderer law.
- Canonical decisions still return to repo truth through roadmap, specs, validation notes, and changelog updates.

## Next Continuation Step

- `C12` cross-cutting polish pass
