# Repository Structure

The repository is shaped as a long-lived monorepo rather than a single saver project.

## Why This Layout Is Stable

- `platform/` isolates reusable runtime work from product-specific behavior.
- `products/` gives each saver, application, and SDK area a clear ownership home.
- `assets/` keeps shared content separate from code and from product-local assets.
- `tests/` and `validation/` separate executable verification from stored evidence.
- `tools/`, `build/`, and `packaging/` keep support concerns visible without mixing them into runtime layers.
- `third_party/` provides an explicit place to track future external material and licenses without implying that any are present today.

## Why The Platform Tree Uses `include/` And `src/`

This split is familiar to native-code contributors and makes public-versus-private boundaries easier to police:

- `include/` is where the eventual public contract belongs.
- `src/` is where host, core, and renderer internals belong.

That distinction matters because the project is expected to support many products over time.

## Why Products Are Grouped By Role

The product tree is grouped as:

- `savers/` for `.scr` products
- `apps/` for helper applications
- `sdk/` for examples and templates

This keeps the repository readable when the product count grows and helps later series make scoped changes without broad collateral edits.
