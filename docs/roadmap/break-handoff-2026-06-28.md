# Break Handoff 2026-06-28

Status: publication-ready pause handoff.

Current HEAD at handoff document creation: `93c767fb7e55bac9d95fdb1196d4ca48bddd565d`.

## Current State

- Plasma v2 status: publication-ready.
- Stable: true.
- Release promotion: accepted.
- Publication prep: ready.
- Publication: not-published.
- Opened next: plasma-v2-publication.
- Release candidate: plasma-v2-rc1.
- Active profile: plasma.v2.reference.preview.

## Completed Gates

- Proof Kernel v0 and v1.
- Portable v2 seam.
- Nocturne and Ricochet canaries.
- Plasma v2 reference preview, stable-candidate, release-candidate, final stable artistic acceptance, stable promotion, and publication prep.
- Instrument architecture, legacy authority, VisualIntent-to-spec, package staging, Manager preview evidence, Workbench release-readiness evidence, support/security/provenance checks, and AIDE evidence indexing.
- PAW-JX closeout scan, publication dry run, and pause-handoff gate.

## Open Gates

- PAW-K Plasma v2 publication.
- Public release upload and public release page.
- Compatibility certification broadening.
- Workbench graphical MVP.
- Manager install/apply path.
- Anthology qualification.
- Core saver family migration.
- macOS, Linux, Web, and other native expansion.
- Public SDK freeze.
- Runtime executable plugin ecosystem.

## Explicitly Not Done

- No release was published.
- No release artifact was uploaded.
- No release tag was created.
- No GitHub release was created.
- No compatibility certification was broadened.
- No Workbench MVP was started.
- No Manager install/apply mutation was started.
- No all-saver migration was started.
- No platform expansion was started.
- AIDE did not become product runtime or product authority.

## Resume Commands

```powershell
git status --short
py -3 tools\scripts\check_project_state.py --summary
py -3 tools\scripts\check_plasma_v2_publication_prep.py
py -3 tools\scripts\check_plasma_v2_pause_handoff.py
py -3 tools\project_adapter\screensave_project.py validate --tier T2
py -3 tools\project_adapter\screensave_project.py validate --tier T3
```

## AIDE Commands

```powershell
py -3 tools\aideops\workunit.py inspect --task SS-PLV2-JX0
py -3 tools\aideops\repair_queue.py scan
py -3 tools\aideops\screensave_aide.py postflight --task SS-PLV2-JX0
```

## Next Decision Tree

Option A: PAW-K Plasma v2 Publication.
Use only if the project owner explicitly decides to publish. This can create a final release archive, release notes, public release assets, and a publication receipt.

Option B: PAW-W1 Workbench MVP.
Use only if publication is intentionally deferred. The first MVP should open a WorkbenchProject, edit Plasma v2 direct controls, render live preview through libsslab, save/export pack candidates, run proof, compare captures, and record review decisions.

Option C: PAW-M1 Manager install/apply path.
Use only if publication is deferred and install/apply productization is the chosen next lane. Scope should remain safe user install, repair, rollback, pack install/remove, and provenance display.

Default after break: decide whether to publish.

## Forbidden Actions

- Do not publish without PAW-K authority.
- Do not upload artifacts without PAW-K authority.
- Do not broaden compatibility certification from publication-ready state.
- Do not start Workbench MVP as an implicit continuation of PAW-JX.
- Do not start Manager install/apply mutation as an implicit continuation of PAW-JX.
- Do not start Anthology, all-saver migration, platform expansion, SDK freeze, or runtime plugins from this handoff.
- Do not let AIDE decide product truth, publish, merge, promote, or mutate source automatically.

## Boundary

Publication-ready is not publication. Stable promotion is not compatibility certification broadening. AIDE evidence is not product truth. Workbench shell is not the final graphical Workbench.
