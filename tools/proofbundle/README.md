# proofbundle

Purpose: normalize ScreenSave proof receipts into Proof Bundle v1 envelopes.

`proofbundle.py` does not rerun renderers or certify compatibility. It reads
ScreenSave-owned proof outputs, comparisons, PE audits, build receipts, or
adapter receipts and emits a single `proof-bundle-v1` JSON envelope with
separate result axes.

Example:

```powershell
python tools\proofbundle\proofbundle.py normalize --proof validation\captures\proof-kernel-v0\nocturne\proof.json --output out\proof\proof-bundle-v1\nocturne.json
```

Type: repository proof tooling.
