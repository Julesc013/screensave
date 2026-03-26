# Architecture

This document describes the intended repository shape established during bootstrap series `S00`.
It is a target structure, not a claim that the implementation already exists.

## Layers

- `src/common/` will hold shared C89 utilities that do not depend on a specific product.
- `src/win32/` will hold Win32 saver host code, message loops, preview handling, settings plumbing, and `.scr` integration.
- `src/render/gdi/` will hold the universal renderer floor that every product can rely on.
- `src/render/gl11/` will hold the optional conservative acceleration layer.
- `products/` will hold product-specific code, content, and configuration that should not leak into shared APIs without strong justification.

## Compatibility Intent

The architecture is anchored to the following invariants:

- Baseline code remains C89-friendly.
- Baseline products target x86 Win32 `.scr`.
- GDI support is mandatory for baseline functionality.
- GL11 is optional and must not become a hidden requirement.

## Bootstrap Status

The current repository only contains structure and control documents.
Future series should add code into the directories above while keeping this document synchronized with actual architecture decisions.
