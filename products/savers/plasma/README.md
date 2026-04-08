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

The current release cut is Classic-first and lower-band-first.

- Stable Core Plasma is centered on the preserved default classic path and the truthful `gdi` / `gl11` baseline.
- Richer lanes, bounded dimensional presentation, transitions, and deeper lab-facing controls remain opt-in experimental material in the current cut.
- The current ship gate is documented as `GO WITH CAVEATS` in [`docs/pl14-ship-readiness.md`](./docs/pl14-ship-readiness.md).
