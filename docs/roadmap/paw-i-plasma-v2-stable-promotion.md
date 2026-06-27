# PAW-I Plasma v2 Stable Promotion

PAW-I starts from Plasma v2 release candidate `plasma-v2-rc1` at commit
`c8b5ecf180f3b25adf7fdd2ca67dcd6693b90409`.

This is the first wave allowed to decide whether Plasma v2 becomes stable. The
decision is still gated: stable promotion requires package, proof, support,
security, provenance, Manager, Workbench, final artistic acceptance, and repair
queue evidence.

## Intake State

- Active program: `plasma-v2-stable-promotion`
- Plasma v2 status: `release-candidate`
- Release candidate: `plasma-v2-rc1`
- Stable: `false`
- Release promotion: `blocked`
- Active proof profile: `plasma.v2.reference.preview`

## Allowed Outcomes

- `stable-promoted`
- `request-changes`
- `defer-to-labs`
- `release-candidate-hold`

## Boundary

If stable promotion passes, PAW-I may set `stable = true` and
`release_promotion = "accepted"`. Public release upload or publication remains
separately gated unless an explicit publication-prep packet is added.

## PAW-I Decision

PAW-I ended in `release-candidate-hold`, not stable promotion.

The stable-promotion gate was structurally satisfied, but the final stable
artistic decision remained `request-changes`. That is an authority blocker, not
a mechanical validator failure. The stable-promotion report recommends
`release-candidate-hold`, records one blocking repair WorkUnit, and keeps
`stable = false` with `release_promotion = "blocked"`.

## Repair Handoff

- Next active program: `plasma-v2-stable-repair`
- Blocking WorkUnit: `SS-PLV2-I-REPAIR-001`
- Gate report:
  `validation/captures/plasma-v2/stable-promotion/gate-report.json`
- Repair burndown:
  `validation/captures/plasma-v2/stable-promotion/repair-burndown.json`

No publication packet, broad compatibility certification, public SDK stability
claim, automatic AIDE promotion, or all-saver migration was admitted.
