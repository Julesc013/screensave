# PAW-H Plasma v2 Release Candidate

PAW-H starts from the release-readiness-reviewed Plasma v2 state at commit
`06161c9111dff2d06550c237b944fb5bbfb042ed`.

The purpose of this wave is to decide whether Plasma v2 becomes a release
candidate. It does not publish a release, set `stable = true`, broaden
compatibility certification, freeze a public SDK, migrate all savers, or admit
autonomous source mutation by AIDE.

## Intake State

- Active program: `plasma-v2-release-candidate`
- Plasma v2 status: `release-readiness-reviewed`
- Release candidate: `none`
- Stable: `false`
- Release promotion: `blocked`
- Active proof profile: `plasma.v2.reference.preview`
- Candidate target if the gate passes: `plasma-v2-rc1`

## Work Sequence

1. Checkpoint release-readiness-reviewed evidence.
2. Define the release-candidate contract.
3. Stage non-published release-candidate package artifacts.
4. Finalize proof, package, review, and evidence matrices.
5. Record release-candidate artistic decision.
6. Finalize support wording and compatibility class.
7. Harden proposal-only agent receipts.
8. Burn down blocking release-candidate repairs.
9. Record Manager and Workbench release-candidate review.
10. Run the release-candidate checker.
11. Transition state only if the checker passes.

## Claim Boundary

PAW-H may record `release_candidate = "plasma-v2-rc1"` with
`stable = false`. PAW-I remains responsible for any stable promotion or
publication decision.
