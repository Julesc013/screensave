# Plasma v2 Next Decision

Status: superseded by Plasma v2 publication hold.

This packet originally framed the post-PAW-JX options when Plasma v2 was still
administratively publication-ready. The current product truth is now recorded in
[`plasma-v2-publication-hold.md`](plasma-v2-publication-hold.md): Plasma v2 is
not publishable because the tested artifact failed real-display visual
acceptance.

## Current Truth

- Plasma v2 = publication-hold.
- Stable = false.
- Historical stable promotion = accepted.
- Current product verdict = visual-rejected.
- Release promotion = withdrawn-for-visual-quality.
- Publication prep = superseded.
- Publication = not-published.
- Opened next = plasma-v3-visual-core-spike.
- Release publication has not occurred.
- Compatibility certification has not broadened.
- AIDE remains evidence/control-plane only.

## Recommended Default

Do not publish Plasma v2. Preserve ScreenSave. Continue the Plasma V3
visual-core spike.

PAW-K0 and PAW-K are blocked while
`validation/captures/plasma-v2/visual-rejection/verdict.toml` records
`decision = "reject-publication"`.

## Former Option A0: Hardware Proof Before Publish

PAW-K0 was the right gate before the real-display verdict existed. The verdict
now exists and rejects publication, so PAW-K0 is no longer the current next
step for Plasma v2.

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

## Former Option A: Publish

Do not open PAW-K for Plasma v2 while the visual rejection is active.

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

## Deferred Option B: Workbench MVP

Use this only after the Plasma V3 visual core proves a good image worth editing.

The current repo has a Workbench shell/proof cockpit layer, not a full graphical authoring app. A Workbench MVP should start with WorkbenchProject, live Plasma direct controls, libsslab preview, pack save/export, proof action, candidate compare, and review decision recording.

Do not start with graph editing. Any graph-like authoring must lower into product-owned specs, plans, runtime, and proof.

## Deferred Option C: Manager Install/Apply

Use this only after the product visual core is acceptable.

Scope should stay within safe current-user install, repair, rollback, pack install/remove, and provenance display. Manager remains optional; saver artifacts must continue to operate without it.

## Boundary

Publication-ready is not publication. Administrative packet evidence is not
visual product acceptance. Stable promotion is not compatibility certification
broadening. AIDE evidence is not product truth. Workbench shell is not the final
graphical Workbench. A real-display visual rejection blocks PAW-K.
