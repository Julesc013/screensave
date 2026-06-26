# sslab

Purpose: headless ScreenSave proof-kernel command surface.

`sslab` is the command-line runner that Workbench, CI, and optional AIDE
integration should eventually share. The current implementation is deliberately
small: it now builds one private `libsslab` archive and one generic
`sslab_runner` executable, then uses named proof profiles to drive deterministic
proof output without product-specific Python dispatch.

PAW-B introduces the private `libsslab` ABI v0 at
`tools/sslab/include/screensave/sslab.h`. That ABI is a current
Windows/32-bit v1 proof-runtime boundary for Nocturne and Ricochet. It is not
the portable v2 seam and must block proof promotion on hosts where
`sizeof(unsigned long) != 4`.

The first reusable proof mechanics live under `tools/sslab/src/`:

- `renderer_rgba8.c` exposes the proof-local RGBA8 renderer shim used by
  current v1 product-session canaries.
- `capture.c` writes canonical raw RGBA bytes and PPM review captures without
  owning product simulation or rendering semantics.

Current commands:

```powershell
python tools\sslab\sslab.py render --product nocturne --output-dir validation\captures\proof-kernel-v0\nocturne
python tools\sslab\sslab.py compare --actual out\proof\run-a\capture.ppm --expected validation\captures\proof-kernel-v0\nocturne\capture.ppm --class exact
python tools\sslab\sslab.py lifecycle --product nocturne --output-dir out\proof\sslab-lifecycle
python tools\sslab\sslab.py lifecycle --product ricochet --width 128 --height 72 --resize-width 96 --resize-height 54 --seed 2048 --frames 32 --delta-ms 100 --create-destroy-cycles 32 --output-dir out\proof\ricochet-lifecycle
python tools\sslab\sslab.py profile --product nocturne --frames 8 --iterations 5 --short-soak-cycles 3 --output-dir out\proof\nocturne-profile-run
python tools\sslab\sslab.py profile --product ricochet --width 128 --height 72 --seed 2048 --frames 32 --delta-ms 100 --iterations 5 --short-soak-cycles 3 --output-dir out\proof\ricochet-profile-run
python tools\sslab\sslab.py proof --profile nocturne.reference.v0 --output-dir out\proof\sslab-profile
python tools\sslab\sslab.py proof --profile ricochet.reference.v1 --output-dir out\proof\ricochet-profile
```

Proof Kernel v0 includes a compiled Nocturne canary runner. It is built and
checked by:

```powershell
python tools\scripts\check_compiled_nocturne_runner.py
```

The validator compiles `tools\sslab\nocturne_canary_runner.c` with GCC against
the Nocturne product sources, the private RGBA8 surface, and the soft renderer.
`sslab.py` is only an orchestrator and comparator over the shared generic
runner path; it must not carry separate product RNG, stepping, or rendering
semantics. The validator runs the fixed Nocturne canary and exact-compares the
result with the committed capture.
The lifecycle, profile, and proof commands are named-profile compatibility
aliases over the shared `sslab_runner` path. They preserve the established
Nocturne and Ricochet proof expectations while removing product-specific
executable dispatch from Python.
The Ricochet profile path captures canonical raw RGBA plus PPM review frames at
0, 4, 8, and 32 fixed-step checkpoints, and requires repeated exact raw hashes
plus a generalized lifecycle receipt with resize evidence and 32
create/destroy cycles. Ricochet profiling and short-soak receipts are emitted
by `sslab profile` as informational process-level evidence.
They are not yet performance qualification gates.

This is not a public saver runtime API and not a compatibility certification.
It is the first ScreenSave-owned visual proof spine. Proof Kernel v0 uses
`exact` comparison as its gate. `tolerant`, `perceptual`, and `observational`
classes are available as report helpers only until later proof-kernel stages
admit them as validation gates. None of these classes replaces human artistic
acceptance.
