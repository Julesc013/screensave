# Plasma v2 Final Stable Artistic Acceptance

Status: active PAW-I-R2 Turn 3 protocol.

Plasma v2 final stable artistic acceptance is a ScreenSave product decision.
Proof bundles, capture matrices, Workbench facts, Manager/package preview facts,
instrument audit reports, and external evidence references can support the
review, but none of them owns the artistic verdict.

The allowed final decisions are:

- `accepted-for-stable`
- `request-changes`
- `defer-to-labs`
- `rejected`

The decision packet must name:

- decision ID
- decision date
- reviewer or project authority
- source commit
- release candidate
- profile
- accepted scope
- excluded scope
- evidence references
- visual review summary
- instrument architecture summary
- known limit acceptance
- claim boundary

Accepted stable scope is limited to Plasma v2 rc1 for
`plasma.v2.reference.preview`: the direct-control instrument architecture,
software/reference path, GDI lower-band path, optional GL11 candidate evidence,
packc v1-candidate data packs, VisualIntent-to-spec candidate flow, and current
Windows release-candidate evidence.

Excluded scope remains explicit:

- final public release publication
- Windows preservation certification
- macOS or Linux support
- all Core saver migration
- public SDK stability
- runtime executable plugin system
- premium, glyph, ribbon, or heightfield stable claims unless separately proved
- external automation runtime dependency or external automation artistic
  authority

If the decision is not `accepted-for-stable`, Plasma v2 remains on
release-candidate hold and the repair queue must carry the exact blocker reason.

Core rule:

```text
Proof supports the decision.
Review records the decision.
ScreenSave owns the decision.
External evidence indexes support the decision without owning it.
```
