# Plasma v2 Stable-Promotion Support

Status: stable-promotion support packet, release-candidate hold.

Plasma v2 stable-promotion support is evidence-classed:

| Surface | Evidence class | Stable-promotion wording |
| --- | --- | --- |
| Current Windows `.scr` artifact | buildable | The staging packet references the VS2017 XP-lane `plasma.scr` output and records its hash. |
| Baseline renderer | targeted | GDI remains the universal floor. |
| First acceleration candidate | experimental | GL11 candidate evidence is compared against the software reference and may fall back. |
| Pack data | buildable | `packc` v1-candidate compiles bounded data-only Plasma packs. |
| Manager | experimental | Manager can preview pack facts and refusal reasons without installation mutation. |
| Workbench | experimental | Workbench can inspect release-readiness facts without publication authority. |
| Final stable artistic decision | blocked | The stable-promotion review is held until an explicit operator verdict is supplied. |

This support packet does not make a compatibility certification claim.
It does not turn release-candidate readiness into stable release promotion.
It does not broaden Plasma v2 beyond the current Windows candidate scope.
