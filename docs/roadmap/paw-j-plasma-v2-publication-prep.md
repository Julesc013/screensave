# PAW-J Plasma v2 Publication Prep

PAW-J starts from the stable-promoted Plasma v2 rc1 state and prepares a local publication packet. It does not publish a release, upload artifacts, broaden compatibility certification, start Workbench MVP, mutate Manager install state, migrate all savers, or expand platform support.

## Source State

- Source commit: `9182f348a4a306ee3499758824da1af7f13432e1`
- Plasma v2 status: stable-promoted
- Stable flag: true
- Release promotion: accepted
- Opened next: plasma-v2-publication-prep
- Publication: not started
- Compatibility certification broadening: false

## WorkUnits

- `SS-PLV2-J0`: checkpoint stable promotion before publication prep.
- `SS-PLV2-J1`: define the publication-prep contract and validator.
- `SS-PLV2-J2`: assemble the local publication manifest packet.
- `SS-PLV2-J3`: prepare stable release notes and compatibility wording.
- `SS-PLV2-J4`: index publication-prep evidence.
- `SS-PLV2-J5`: add the publication-prep checker and reports.
- `SS-PLV2-J6`: record publication-ready or publication-hold.

## Boundary

Stable promotion is a product state decision that has already been accepted for the bounded Plasma v2 rc1 profile. Publication prep is a release packet decision. Actual public publication, compatibility certification broadening, public SDK stability, all-saver migration, Workbench graphical authoring, Manager install/apply mutation, and platform expansion remain separate gates.

## Required Validation

```powershell
py -3 tools\scripts\check_project_state.py --summary
py -3 tools\scripts\check_plasma_v2_stable_promotion.py
py -3 tools\scripts\check_plasma_v2_publication_prep.py
py -3 tools\project_adapter\screensave_project.py validate --tier T2
py -3 tools\project_adapter\screensave_project.py validate --tier T3
py -3 tools\scripts\check_docs_basics.py
py -3 tools\scripts\check_contracts.py
py -3 tools\scripts\check_aide_evidence_index.py
git diff --check
```
