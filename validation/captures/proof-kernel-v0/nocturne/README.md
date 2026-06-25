# Proof Kernel v0 Nocturne Canary

This directory stores the first ScreenSave-owned deterministic visual proof
canary.

The capture is produced by:

```powershell
python tools\sslab\sslab.py render --product nocturne --preset observatory_night --width 96 --height 54 --seed 1536 --frames 8 --delta-ms 100 --output-dir validation\captures\proof-kernel-v0\nocturne
```

Scope:

- product: `nocturne`
- preset: `observatory_night`
- surface: RGBA8, top-left origin, sRGB bytes
- renderer: `soft-reference-v0`
- runner: compiled product-session proof runner
- status: informational canary, not compatibility certification

The validator `tools/scripts/check_proof_kernel.py` reruns the same canary in
temporary directories, checks the committed capture hash, and verifies that the
committed proof records clean-source provenance and Nocturne product-source
digests. The recorded source commit is the clean capture-baseline provenance;
the validator separately recomputes every recorded implementation SHA-256
against the current tree so proof-relevant source edits require an intentional
baseline refresh.
