# Plasma v2 Release-Candidate Support Matrix

Status: release-candidate support matrix, not stable release.

Plasma v2 release-candidate support is evidence-classed:

| Surface | Evidence class | Release-candidate wording |
| --- | --- | --- |
| Current Windows `.scr` artifact | buildable | The candidate package references the VS2017 XP-lane `plasma.scr` output and records its hash. |
| Baseline renderer | targeted | GDI remains the universal floor. |
| First acceleration candidate | experimental | GL11 candidate evidence is compared against the software reference and may fall back. |
| Pack data | buildable | `packc` v1-candidate compiles bounded data-only Plasma packs. |
| Manager | experimental | Manager can preview pack facts and refusal reasons without installation mutation. |
| Workbench | experimental | Workbench can inspect release-readiness facts without publication authority. |

This support matrix does not make a compatibility certification claim.
It does not turn release-candidate readiness into stable release promotion.
