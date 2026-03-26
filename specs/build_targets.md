# Build Targets Specification

This document is normative.

## Toolchain Families

The intended baseline toolchain families are:

- Microsoft Visual C++ toolchains appropriate for Win32 native builds.
- MinGW-family toolchains where they can produce compatible Win32 artifacts.

The build system may grow later, but early-series work must keep both families conceptually viable unless a spec update states otherwise.

## Baseline Targets

The primary baseline target is an x86 Win32 `.scr` saver product.

Additional target classes may include:

- Shared platform libraries used by saver products.
- Suite-level helper applications such as Gallery, Player, and BenchLab.
- SDK examples and templates.

These extra target classes must not displace the `.scr` baseline.

## Artifact Naming Philosophy

Artifact names should make the product role obvious:

- Saver products produce product-named `.scr` artifacts.
- Applications produce `.exe` artifacts with stable suite-level names.
- Intermediate outputs should remain inside toolchain/profile-specific build trees rather than cluttering the repository root.

Final naming details can be refined later, but they must stay predictable and product-oriented.

## Debug And Release Intent

At minimum, later series should preserve:

- A debug-oriented path for development and investigation.
- A release-oriented path for shippable artifacts.

Debug builds may expose additional diagnostics.
Release builds must preserve the documented compatibility contract.

## Output Conventions

Checked-in repository structure under `build/` describes build coordination material, not generated output.

Generated build output should live in toolchain and profile specific directories outside committed source content, for example:

- toolchain-specific output roots
- profile-specific subdirectories such as debug and release
- architecture-specific subdivisions when needed

No later series should normalize a layout that mixes generated artifacts into source directories.

## Future-Series Responsibilities

Later implementation series are responsible for:

- Choosing concrete project files or generators.
- Proving that the baseline targets can be built reproducibly.
- Keeping build logic aligned with the compatibility and release specs.

Series 00 does not define the final build machinery.
