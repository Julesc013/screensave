# U03 Settings Model

This short note remains as a compatibility summary for older `U03` references.

The canonical `U03` surface truth now lives in:

- [`u03-settings-surfaces.md`](./u03-settings-surfaces.md)
- [`u03-control-inventory.md`](./u03-control-inventory.md)
- [`u03-ui-and-runtime-truth.md`](./u03-ui-and-runtime-truth.md)
- [`u03-post-ui-invariants.md`](./u03-post-ui-invariants.md)

## Current Summary

- `Basic` is now the stable first-pass surface only.
- `Advanced` is now the exact grammar surface only.
- `Author/Lab` is now the exact curation and diagnostics surface only.
- higher surfaces no longer act like cumulative copies of lower ones.
- routine dialog refreshes no longer reread hidden controls back into the
  working config.

`U03` is a UI and surface-truth recut, not a deeper visual-grammar rewrite.
