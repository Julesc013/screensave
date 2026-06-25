# sslab

Purpose: headless ScreenSave proof-kernel command surface.

`sslab` is the command-line runner that Workbench, CI, and optional AIDE
integration should eventually share. The current implementation is deliberately
small: it renders the Nocturne proof-kernel canary through a deterministic
private RGBA8/software path and emits a proof-bundle v0 JSON record.

Current command:

```powershell
python tools\sslab\sslab.py render --product nocturne --output-dir validation\captures\proof-kernel-v0\nocturne
```

This is not a public saver runtime API and not a compatibility certification.
It is the first ScreenSave-owned visual proof spine.
