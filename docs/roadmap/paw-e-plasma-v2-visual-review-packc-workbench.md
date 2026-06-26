# PAW-E: Plasma v2 Visual Review, Packc, and Workbench Authoring

Status: active preview-candidate wave.

PAW-E starts from the Plasma v2 reference-preview slice and asks whether it can
be authored, reviewed, packed, inspected, compared, and proof-qualified as a
real preview candidate.

It does not ask whether Plasma v2 is stable, released, compatibility certified,
or accepted as final art.

## Boundaries

Keep these axes separate:

- reference-preview is not artistic acceptance
- artistic acceptance is not stable release
- stable release is not compatibility certification
- AIDE evidence is not ScreenSave product truth

The doctrine remains:

```text
Portable meaning.
Native delivery.
Deterministic proof.
Optional automation.
```

## Work Packets

- `SS-PLV2-E0`: checkpoint Plasma v2 reference-preview truth
- `SS-PLV2-E1`: add visual review protocol
- `SS-PLV2-E2`: harden the data-only Pack v1 preview pipeline
- `SS-PLV2-E3`: resolve bounded Plasma candidates from VisualIntent
- `SS-PLV2-E4`: expand the preview proof matrix
- `SS-PLV2-E5`: add the Workbench Author, Inspect, and Compare review loop
- `SS-PLV2-E6`: record development-plane review evidence
- `SS-PLV2-E7`: add the reviewed-preview checker
- `SS-PLV2-E8`: record Plasma v2 reviewed-preview state

## Non-Goals

PAW-E must not start stable Core release, Manager pack installation, all-saver
migration, macOS or Linux hosts, D3D12 or Vulkan, public SDK freeze, runtime
executable plugins, automatic AIDE mutation, or automatic release promotion.

## Exit

PAW-E is complete when the reviewed-preview checker passes and
`PROJECT_STATE.toml` records Plasma v2 as `reviewed-preview`, with
`stable = false` and release promotion still blocked.
