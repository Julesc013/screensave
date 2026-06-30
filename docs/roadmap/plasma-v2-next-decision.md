# Plasma v2 Next Decision

Status: decision packet for resuming after PAW-JX.

Plasma v2 is publication-ready. The next step is a project decision, not an automatic continuation.

## Current Truth

- Plasma v2 = publication-ready.
- Stable = true.
- Release promotion = accepted.
- Publication prep = ready.
- Publication = not-published.
- Opened next = plasma-v2-publication.
- Release publication has not occurred.
- Compatibility certification has not broadened.
- AIDE remains evidence/control-plane only.

## Recommended Default

Decide whether to publish Plasma v2 stable.

If yes, run PAW-K0 Plasma v2 Hardware Visual Proof Closeout first, then open
PAW-K Plasma v2 Publication only if the hardware proof and project owner both
support publication. If no, explicitly choose which productization lane to open
next.

## Option A0: Hardware Proof Before Publish

Use PAW-K0 when the project owner is leaning toward publication but wants the
last real-display proof pass before public release work.

Allowed PAW-K0 work:

- Hardware and environment matrix.
- Fullscreen `.scr` run notes.
- Contact sheets or screenshot indices from real displays.
- Renderer-tier comparison captures or explicit unavailable-tier notes.
- Frame-time or pacing summary where available.
- Human visual note with publish, defer, or repair recommendation.

Still forbidden in PAW-K0:

- Public upload.
- GitHub release creation.
- Release page publication.
- Compatibility certification broadening.
- Source redesign or Plasma modular extraction.
- Workbench MVP or Manager install/apply mutation.

## Option A: Publish

Open PAW-K only after an explicit project-owner publication decision and either
a PAW-K0 pass or an explicit owner-recorded deferral of PAW-K0.

Allowed PAW-K work:

- Final release archive.
- Release asset names.
- Release notes.
- Public release or publication-prep receipt.
- Public link verification.
- Post-release known-limits ledger.

Still forbidden unless separately approved:

- Compatibility certification broadening.
- Public SDK freeze.
- All Core saver migration.
- macOS, Linux, or Web support claims.
- Runtime plugin ecosystem.

## Option B: Workbench MVP

Use this only if publication is intentionally deferred.

The current repo has a Workbench shell/proof cockpit layer, not a full graphical authoring app. A Workbench MVP should start with WorkbenchProject, live Plasma direct controls, libsslab preview, pack save/export, proof action, candidate compare, and review decision recording.

Do not start with graph editing. Any graph-like authoring must lower into product-owned specs, plans, runtime, and proof.

## Option C: Manager Install/Apply

Use this only if publication is intentionally deferred and install/apply is the chosen productization lane.

Scope should stay within safe current-user install, repair, rollback, pack install/remove, and provenance display. Manager remains optional; saver artifacts must continue to operate without it.

## Boundary

Publication-ready is not publication. Hardware proof is not compatibility
certification broadening. Stable promotion is not compatibility certification
broadening. AIDE evidence is not product truth. Workbench shell is not the final
graphical Workbench.
