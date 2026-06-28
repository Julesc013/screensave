# Plasma v2 Publication-Prep Contract v1

Status: active publication-prep contract.

This contract governs PAW-J, the transition from `stable-promoted` to one of:

- `publication-ready`
- `publication-hold`

Publication prep is not stable promotion. Stable promotion records that
`plasma-v2-rc1` has been accepted as the bounded stable Plasma v2 slice.
Publication prep records whether the release packet is complete enough for a
later publication gate. Actual publication remains a separate act.

Publication prep is also not compatibility certification. Evidence may describe
the current Windows artifact class, package stage, proof bundle, and known
limits, but it must not broaden public support claims by implication.

## Required Input State

- `portable_v2.status = "accepted"`
- `plasma_v2.status = "stable-promoted"`
- `plasma_v2.stable = true`
- `plasma_v2.release_candidate = "plasma-v2-rc1"`
- `plasma_v2.release_promotion = "accepted"`
- `plasma_v2.opened_next = "plasma-v2-publication-prep"`
- `authority.active_program = "plasma-v2-publication-prep"`

## Required Axes

The publication-prep gate must check these axes separately:

- stable-promotion accepted
- release manifest
- artifact manifest
- checksums
- provenance
- SBOM or third-party notice
- known limits
- support matrix
- install notes
- rollback notes
- package-stage evidence
- Manager preview evidence
- Workbench release facts
- AIDE EvidencePacket
- no public upload yet
- no release page publication yet
- no compatibility certification broadening
- no all-saver migration claim

## Claim Separation

Publication prep does not collapse these claims:

- stable promotion != public publication
- publication prep != actual publication
- release manifest != uploaded release asset
- checksum record != external download verification
- package-stage evidence != compatibility certification
- current Windows evidence != preservation certification
- Manager preview evidence != install/apply mutation
- Workbench release facts != graphical Workbench MVP
- AIDE EvidencePacket != ScreenSave release authority

## Required Packet

A publication-ready packet must include:

- `releases/plasma-v2-stable/release-manifest.toml`
- `releases/plasma-v2-stable/artifact-manifest.toml`
- `releases/plasma-v2-stable/checksums.sha256`
- `releases/plasma-v2-stable/provenance.json`
- `releases/plasma-v2-stable/known-limits.md`
- `releases/plasma-v2-stable/support-matrix.md`
- `releases/plasma-v2-stable/install-notes.md`
- `releases/plasma-v2-stable/rollback-notes.md`
- `releases/plasma-v2-stable/third-party-notices.md`
- `releases/plasma-v2-stable/publication-checklist.md`

## Explicit Exclusions

PAW-J publication prep must not claim or perform:

- public release publication
- release asset upload
- GitHub release creation
- release page publication
- broad Windows preservation certification
- macOS/Linux support
- all Core saver migration
- public SDK freeze
- runtime executable plugin ecosystem
- Manager install/apply mutation
- graphical Workbench MVP
- automatic AIDE publication
- automatic AIDE promotion

## Permitted Publication-Ready State

Only after the publication-prep checker passes may `PROJECT_STATE.toml` set:

```toml
[authority]
active_program = "plasma-v2-publication"
release_candidate = "plasma-v2-rc1"

[plasma_v2]
status = "publication-ready"
stable = true
release_candidate = "plasma-v2-rc1"
release_promotion = "accepted"
publication_prep = "ready"
publication = "not-published"
opened_next = "plasma-v2-publication"
```

If the checker blocks, state must remain stable-promoted and record:

```toml
[plasma_v2]
status = "stable-promoted"
stable = true
release_promotion = "accepted"
publication_prep = "hold"
publication = "not-published"
opened_next = "plasma-v2-publication-prep"
```

The hold path must name exact blockers and create bounded repair WorkUnits.
