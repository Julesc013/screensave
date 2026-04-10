# Plasma

Plasma is the framebuffer-and-palette product in the Framebuffer and Vector Family.

It provides:

- three related effect modes: plasma, fire, and interference waves
- a real internal software framebuffer with restrained palette cycling
- modest speed, resolution, and smoothing controls instead of a broad effects lab
- curated themes and presets tuned for both the mandatory GDI path and optional GL11
- preview-safe and long-run-stable behavior through the shared saver and renderer contracts
- `C13` adds stronger effect-mode palette distinction, curated dark-room amber and midnight variants, and periodic composition refresh so long runs keep evolving without becoming a pixel-lab sandbox

Plasma stays intentionally focused.
It is not a general pixel-effects lab, palette editor, or shader showcase.

## Current Release Posture

The current release cut is recorded in the `PX50` recut docs and remains Classic-first and lower-band-first.

- Stable Core Plasma remains centered on the preserved default classic path and the truthful `gdi` / `gl11` baseline.
- `PX50` keeps richer lanes, bounded dimensional presentation, transitions, capture-backed compare, and the bounded PX30 through PX32 study surfaces opt-in experimental.
- `PX50` admits a narrow stable support-surface widening only: BenchLab reporting, PX40 read-only authoring and ops reports, and the PX41 stable metadata-report slice that restates already-stable product truth.
- The current ship gate remains `GO WITH CAVEATS` and is now documented in [`docs/px50-ship-posture.md`](./docs/px50-ship-posture.md).
