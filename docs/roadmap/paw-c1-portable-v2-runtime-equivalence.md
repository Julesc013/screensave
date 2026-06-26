# PAW-C1 Portable v2 Runtime Equivalence

Status: complete; Gate C accepted on 2026-06-26.

PAW-C1 starts at commit `d57669d`, after the PAW-CX checkpoint added the
ScreenSave-local AIDE operating loop, validation tiers, fixed proof and bundle
capabilities, EvidencePacket export, and the first host-neutral portable v2
public header seam.

## Objective

Make portable v2 executable through the two proof canaries before any Plasma v2
runtime work begins.

By the end of this wave:

- Nocturne and Ricochet execute through portable v2.
- The existing v1 Win32 `.scr` path remains preserved.
- `libsslab` can run both canaries through the v2 path.
- Nocturne keeps its canonical RGBA hash.
- Ricochet keeps exact hashes for frames `0`, `4`, `8`, and `32`.
- Workbench can inspect v2 sessions through the same proof runtime used by CI.
- AIDE records evidence without becoming product runtime truth.
- Gate C was accepted only after the deterministic and native evidence gates passed.

## Closeout

- Nocturne and Ricochet now use extracted portable product cores.
- The v2 adapters call product cores directly rather than wrapping v1 sessions.
- `libsslab` ABI v1 exists as the v2-backed proof path while ABI v0 remains
  available for comparison.
- Current-head native evidence is recorded under
  `validation/captures/portable-v2/native-windows-current/`.
- Independent Gate C acceptance is recorded under
  `validation/captures/portable-v2/gate-c/`.
- The next active program is `plasma-v2-reference-slice`.

## Sequencing

The wave is deliberately serialized:

1. Checkpoint PAW-CX and prepare this runtime-equivalence wave.
2. Add portable v2 descriptor validation and smoke compilation.
3. Bridge v2 draw targets to the existing reference renderer mechanics.
4. Add Nocturne and Ricochet v2 adapters without changing baseline hashes.
5. Add the v1 compatibility adapter over v2 where products provide v2 ops.
6. Add `libsslab` ABI v1 as the v2-backed proof path.
7. Prove v1/v2 exact equivalence for the named canary profiles.
8. Expose only fixed v2 proof and bundle capabilities through the adapter.
9. Extend Workbench Inspect for v2 session evidence.
10. Gate portable and native artifact preservation in CI.
11. Accept Gate C and open Plasma v2 only after check-only acceptance passes.

## Boundaries

Do not implement Plasma v2 runtime in this wave.

This wave must not:

- migrate all savers,
- reorganize the repository,
- introduce a public SDK ABI,
- admit generic AIDE command execution,
- admit a general worker,
- change public compatibility claims,
- turn EvidencePackets into ScreenSave proof truth.

The doctrine remains:

```text
Portable meaning.
Native delivery.
Deterministic proof.
Optional automation.
```

## Normal Validation

Per commit:

```powershell
py -3 tools\scripts\check_portable_v2.py
py -3 tools\scripts\check_sslab_runner.py
py -3 tools\scripts\check_project_state.py --summary
py -3 tools\scripts\check_contracts.py
git diff --check
```

Product-core changes additionally run the fixed Nocturne and Ricochet profile
proofs through the project adapter.
