# sslab

Purpose: headless ScreenSave proof-kernel command surface.

`sslab` is the command-line runner that Workbench, CI, and optional AIDE
integration should eventually share. The current implementation is deliberately
small: it renders the Nocturne proof-kernel canary through a deterministic
private RGBA8/software path and emits a proof-bundle v0 JSON record.

Current commands:

```powershell
python tools\sslab\sslab.py render --product nocturne --output-dir validation\captures\proof-kernel-v0\nocturne
python tools\sslab\sslab.py compare --actual out\proof\run-a\capture.ppm --expected validation\captures\proof-kernel-v0\nocturne\capture.ppm --class exact
```

Proof Kernel v0 also includes a compiled Nocturne canary runner. It is built
and checked by:

```powershell
python tools\scripts\check_compiled_nocturne_runner.py
```

The validator compiles `tools\sslab\nocturne_canary_runner.c` with GCC against
the private RGBA8 surface and soft renderer, runs the fixed Nocturne canary,
and exact-compares the result with the committed capture.

This is not a public saver runtime API and not a compatibility certification.
It is the first ScreenSave-owned visual proof spine. Proof Kernel v0 uses
`exact` comparison as its gate. `tolerant`, `perceptual`, and `observational`
classes are available as report helpers only until later proof-kernel stages
admit them as validation gates. None of these classes replaces human artistic
acceptance.
