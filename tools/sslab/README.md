# sslab

Purpose: headless ScreenSave proof-kernel command surface.

`sslab` is the command-line runner that Workbench, CI, and optional AIDE
integration should eventually share. The current implementation is deliberately
small: it compiles and runs the Nocturne proof-kernel canary through the real
Nocturne product session and render functions, using a deterministic private
RGBA8/software path, then emits a proof-bundle v0 JSON record.

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
python tools\sslab\sslab.py proof --profile nocturne.reference.v0 --output-dir out\proof\sslab-profile
```

Proof Kernel v0 includes a compiled Nocturne canary runner. It is built and
checked by:

```powershell
python tools\scripts\check_compiled_nocturne_runner.py
```

The validator compiles `tools\sslab\nocturne_canary_runner.c` with GCC against
the Nocturne product sources, the private RGBA8 surface, and the soft renderer.
`sslab.py` is only an orchestrator and comparator for this path; it must not
carry separate Nocturne RNG, stepping, or rendering semantics. The validator
runs the fixed Nocturne canary and exact-compares the result with the committed
capture.
The lifecycle command runs a separate create, resize, fixed-step, render, and
destroy scenario through the same compiled product-session runner without
changing the committed render baseline.
The proof command loads `catalog/generated/proof_registry.json` and orchestrates
render, exact comparison, and lifecycle receipts from the named profile. It is
the PAW-B entry point for moving Nocturne, then Ricochet, onto catalog-owned
proof profiles.

This is not a public saver runtime API and not a compatibility certification.
It is the first ScreenSave-owned visual proof spine. Proof Kernel v0 uses
`exact` comparison as its gate. `tolerant`, `perceptual`, and `observational`
classes are available as report helpers only until later proof-kernel stages
admit them as validation gates. None of these classes replaces human artistic
acceptance.
