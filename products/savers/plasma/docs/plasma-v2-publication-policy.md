# Plasma v2 Publication Policy

Status: publication-prep policy, not publication.

Plasma v2 has a stable-promoted rc1 slice, but publication is a separate release packet decision. The publication-prep packet may gather release notes,
manifests, artifact references, checksums, provenance, known limits, support
wording, install notes, rollback notes, and proof references. It must not upload or publish release assets.

The published product scope, when a later publication gate allows it, remains:

- product: Plasma v2
- release candidate: `plasma-v2-rc1`
- profile: `plasma.v2.reference.preview`
- software/reference path: canonical
- renderer floor: `gdi`
- first optional acceleration candidate: `gl11`
- pack status: data-only `packc` v1-candidate
- authoring flow: direct controls and VisualIntent candidates reduce to
  `plasma_v2_spec`, `plasma_v2_plan`, and `plasma_v2_runtime`

Publication prep must keep support wording evidence-classed and
artifact-specific. Current Windows evidence may be cited, but broad Windows preservation certification is not created by publication prep. macOS, Linux,
public SDK stability, all Core saver migration, runtime executable plug-ins,
Manager install/apply mutation, and a graphical Workbench authoring app remain
outside this scope.

Publication prep may reach `publication-ready`. It may not become `published`
until a later publication gate verifies the final release archive, release page,
public links, checksums, provenance, and publication receipt.
