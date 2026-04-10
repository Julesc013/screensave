# PX10 Hardware Proof

## Purpose

This document turns Plasma's current hardware-proof posture into an explicit product-local matrix.
It uses the shared `SY10` hardware-proof vocabulary and keeps unsupported breadth explicit instead of burying it inside a single catch-all caveat.

## Fingerprint Fields Used

Each row below uses the following environment fingerprint fields where the evidence supports them:

- host OS
- host mode or harness
- GPU vendor
- backend renderer identity
- backend detail or driver text when present
- monitor topology or topology knowledge
- requested lane
- resolved lane
- proof status

Current repo reality only provides a complete GPU and driver fingerprint for the BenchLab OpenGL captures.
The GDI floor capture still provides backend identity, but not an external GPU vendor string.

## Proof Status Meanings

- `validated`: backed by actual checked-in evidence
- `partial`: backed by real evidence, but not across the full support space implied by the row
- `blocked`: cannot be proved further with the currently available environment
- `not_tested`: the product surface is described, but that exact environment or combination was not rerun

## Current Matrix

| Surface | Environment Fingerprint | Topology / Mode | Requested -> Resolved | Status | Evidence | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| Stable default floor | Windows 10 Enterprise `2009` x64; Win32 GDI DIB32 floor | single drawable BenchLab window | `gdi -> gdi` | `validated` | [`../../../../validation/captures/pl13/benchlab-plasma-gdi.txt`](../../../../validation/captures/pl13/benchlab-plasma-gdi.txt) | Confirms the preserved classic/default path on the mandatory floor. |
| Stable default preferred lane | Windows 10 Enterprise `2009` x64; NVIDIA GeForce RTX 2070; driver string `gl=4.6.0 NVIDIA 591.86 ...` | single drawable BenchLab window | `gl11 -> gl11` | `validated` | [`../../../../validation/captures/pl13/benchlab-plasma-gl11.txt`](../../../../validation/captures/pl13/benchlab-plasma-gl11.txt) | Confirms the truthful preferred stable lane remains the compat path, not a richer-tier dependency. |
| Optional richer-lane ladder | Windows 10 Enterprise `2009` x64; NVIDIA GeForce RTX 2070; same driver family | single drawable BenchLab window | `gl21 -> gl21`, `gl33 -> gl33`, `gl46 -> gl46` | `validated` | [`../../../../validation/captures/pl13/README.md`](../../../../validation/captures/pl13/README.md) | Confirms the bounded richer lanes are real on the known validation machine. |
| Policy-cap truth | Windows 10 Enterprise `2009` x64; NVIDIA GeForce RTX 2070; same driver family | single drawable BenchLab window | `auto -> gl11`, `gl46 -> gl11` under saver policy | `validated` | [`../../../../validation/captures/sx09/benchlab-plasma-auto.txt`](../../../../validation/captures/sx09/benchlab-plasma-auto.txt), [`../../../../validation/captures/sx09/benchlab-plasma-gl46.txt`](../../../../validation/captures/sx09/benchlab-plasma-gl46.txt) | Confirms requested-versus-resolved truth for Plasma's lower-band-first policy on a capable machine. |
| Fullscreen `.scr` host startup and close path | Windows 10 Enterprise `2009` x64 | fullscreen saver host path | shared host path under `gdi` | `partial` | [`../../../../validation/notes/ss02-support-matrix.md`](../../../../validation/notes/ss02-support-matrix.md) | Confirms the real saver host path and preserved routing floor, but not a full Plasma-specific hardware sweep. |
| Preview child-window host path | shared host audit; no fresh Plasma-local rerun in this tranche | preview child-window path | host preview path | `partial` | [`../../../../validation/notes/c05-windows-integration-matrix.md`](../../../../validation/notes/c05-windows-integration-matrix.md) | Shared host preview wiring exists, but PX10 did not add a fresh native Control Panel rerun for Plasma itself. |
| Mixed-topology virtual desktop | no checked-in mixed-topology capture | more than one physical monitor | current shared host policy | `not_tested` | [`px10-multi-monitor.md`](./px10-multi-monitor.md) | Plasma now documents the actual one-session baseline, but mixed physical-monitor reruns remain absent. |
| Lower-capability denial ladder | no checked-in lower-capability machine capture | any | richer request denied by hardware rather than saver policy | `blocked` | [`pl13-known-limits.md`](./pl13-known-limits.md) | The repo still lacks a second reduced-capability machine or cross-driver denial sweep. |

## What Was Actually Exercised

Actually exercised in checked-in evidence:

- `gdi`, `gl11`, `gl21`, `gl33`, and `gl46` report-mode runs on the known capable validation machine
- Plasma policy-cap truth for `auto` and forced `gl46` requests
- real fullscreen host startup and close behavior on Windows 10 x64 in the release-validation pass

Not actually exercised in checked-in evidence:

- a second GPU or driver family
- low-capability denial proof caused by missing `gl21`, `gl33`, or `gl46`
- fresh mixed-topology or per-monitor reruns
- a fresh native preview-host rerun specific to Plasma in this tranche

## Per-Lane And Per-Feature Notes

- `gdi` remains the only guaranteed floor and must be read as the support floor, not as a fallback afterthought.
- `gl11` remains the truthful preferred stable lane and is still proved directly.
- `gl21`, `gl33`, and `gl46` are real on the known machine, but that does not convert them into broad cross-hardware support claims.
- The current hardware picture is stronger as an explicit matrix than it was as a vague caveat, but it is still a narrow matrix.

## Remaining Hardware-Proof Caveats

- Current live runtime proof still centers on one capable validation machine.
- The current repo still does not include a second GPU vendor, alternate driver family, or lower-capability denial capture.
- Fullscreen `.scr` behavior should not be inferred from BenchLab alone.
- Mixed-topology and multi-monitor hardware breadth remain explicitly unproved.

## Evidence Boundary

Actual evidence:

- checked-in `sx09` and `pl13` BenchLab captures
- checked-in shared and release-validation notes

Still unavailable:

- new machine evidence
- new driver-family evidence
- a fuller multi-monitor or preview-host rerun matrix
