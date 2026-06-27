# Plasma v2 Release-Candidate Support Matrix

Status: release-candidate support wording.

| Area | Evidence class | Claim |
| --- | --- | --- |
| Current Windows `.scr` artifact | buildable | VS2017 XP-lane output is referenced and hashed for the candidate stage. |
| PE and compatibility audit | binary-audited when validator evidence is present | Binary facts do not certify every target operating system. |
| GDI path | targeted | Mandatory floor inherited from ScreenSave baseline. |
| GL11 path | experimental | First optional accelerated candidate with fallback evidence. |
| Pack data | buildable | Data-only pack examples compile through `packc`; no executable extension ecosystem is admitted. |
| Manager preview | experimental | Shows manifest, provenance, license, proof refs, and rollback notes without install mutation. |
| Workbench release-readiness | experimental | Shows package, proof, review, and performance facts without publication authority. |

Only a later stable-promotion gate may turn release-candidate readiness into a
stable public release claim.

This matrix does not make a compatibility certification claim.
