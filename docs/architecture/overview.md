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

Series 03 made the private Win32 host real.
Series 04 makes the reusable non-renderer runtime and first public platform surface real.
Series 05 makes the mandatory reusable GDI backend real and routes the host through the shared renderer path.
Real saver-product behavior and optional higher renderer tiers remain deferred while preserving the compatibility and ownership rules defined in `specs/`.
