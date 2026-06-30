# Plasma V3 Visual Core Reset

Status: active visual-core spike, not a publication lane.

Plasma V3 is a visual-core reset for the case where the current running Plasma
artifact is rejected by product acceptance even though the surrounding
ScreenSave architecture remains useful.

This note records the current boundary:

- ScreenSave platform scaffolding remains valuable.
- Plasma v2 remains legacy, compatibility, and evidence material.
- Plasma V3 is a new product-owned visual center.
- Publication is blocked until artifact truth, hardware visual proof, and a
  human product verdict all converge.

## Product Target

Plasma V3 is a deterministic modular field instrument for ambient visuals.

The ordering is:

```text
direct controls first
presets second
software/reference path first
optional acceleration second
```

The first spike is intentionally narrow:

- one scalar field model;
- one fixed output domain;
- one material ramp system;
- one soft phosphor treatment;
- one flat present buffer;
- one deterministic frame pacing loop;
- one software/reference capture harness;
- one optional `gl11` preview route that resolves the same reference buffer;
- one control influence matrix over rendered pixels.

## Repository Boundary

The spike lives under `products/savers/plasma/src/v3/` and
`products/savers/plasma/tests/plasma_v3_visual_smoke.c`.

It does not change:

- `platform/`;
- the Win32 host;
- public renderer contracts;
- Plasma v2 release manifests;
- staged stable-promotion evidence;
- publication receipts;
- compatibility claims.

The software/reference path is canonical. GDI remains the floor. GL11 remains an
optional preview route and must not become a hidden minimum.

## Proof Boundary

`tools/scripts/plasma_v3_capture.py` renders deterministic PPM captures,
contact-sheet evidence, frame-time notes, and a control influence matrix.

This evidence proves only that the spike is deterministic and that direct
controls alter rendered output. It does not prove hardware appearance,
acceptance, publication readiness, release compatibility, or saver integration.
