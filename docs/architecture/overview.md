# Architecture Overview

ScreenSave is organized as one shared platform plus many separate products.

The platform side exists to solve the common problems once:

- classic Win32 `.scr` hosting
- preview and full-screen mode handling
- config resolution
- renderer selection
- shared runtime services

The product side exists to keep each saver or suite-level application separate:

- savers define their own identity, presets, themes, and content
- applications such as Gallery, Player, and BenchLab serve different purposes than the `.scr` products
- SDK material supports later contributors without becoming part of the runtime baseline

The architecture is intentionally conservative.
Baseline functionality must remain available on a GDI-only path and must not depend on modern-only Windows assumptions.

Series 03 makes the first part of that architecture real: the private Win32 host now handles classic screen, preview, and configuration entry paths with a temporary host-local visual path.
Reusable renderer code, the broader public saver/runtime API, and real saver-product behavior remain deferred while preserving the compatibility and ownership rules defined in `specs/`.
