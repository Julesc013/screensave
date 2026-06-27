# Plasma v2 Release-Candidate Proof Matrix

Status: pass

This matrix projects existing Plasma v2 reference-preview, stable-candidate,
release-readiness, package, Manager, Workbench, and review evidence into the
PAW-H release-candidate review lane.

It does not publish a release, set `stable = true`, certify compatibility,
freeze a public SDK, or migrate all savers.

| Row | Status | Evidence |
| --- | --- | --- |
| Reference software path | pass | `validation/captures/plasma-v2/reference-preview/profile-proof.json` |
| GL11 candidate path | pass | `validation/captures/plasma-v2/acceleration/matrix.json` |
| Default `plasma_lava` | pass | `validation/captures/plasma-v2/reference-preview/proof-bundle-v1.json` |
| Five U09 materials | pass | `validation/captures/plasma-v2/materials/material-treatment-summary.json` |
| Restrained dither | pass | `validation/captures/plasma-v2/acceleration/matrix.json` |
| Restrained CRT | pass | `validation/captures/plasma-v2/acceleration/matrix.json` |
| VisualIntent candidates | pass | `validation/captures/plasma-v2/matrix/visualintent-candidates.json` |
| Packc example packs | pass | `products/savers/plasma/content/v2/examples/plasma_lava_v2.toml` |
| Manager preview | pass | `products/apps/suite/src/manager_pack_preview.c` |
| Workbench release-readiness | pass | `products/apps/benchlab/src/benchlab_workbench_shell.c` |

Claim axes remain distinct: deterministic proof, visual review, performance
envelope, artifact build/audit, release-candidate readiness, stable release
promotion, and compatibility certification are not interchangeable.
