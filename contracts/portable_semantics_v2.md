# Portable Semantics Contract v2

Status: planned contract, not yet a runtime implementation.

Current header slice: `platform/include/screensave/v2/` defines the initial
host-neutral public base, surface, draw, product, and session seam. It is not a
claim that product migrations, compatibility adapters, or v1/v2 equivalence are
complete.

## Purpose

ScreenSave v2 should let products share deterministic semantic infrastructure
while still producing native standalone saver artifacts for each host platform.

The portable layer exists for product behavior, configuration, deterministic
execution, bounded plans, surfaces, and diagnostics. It does not own native
windowing, installation, platform settings, or presenter lifecycles.

## Canonical Pipeline

```text
Presets / themes / materials / journeys / packs
                         |
                         v
               Product semantic model
          state, configuration, seed, time
                         |
                         v
              Product-local bounded IR
             validated immutable plan
                         |
             +-----------+-----------+
             v                       v
      Reference compute       Optional compute
          scalar              SIMD / GPU stage
             +-----------+-----------+
                         v
              Rendering realization
       software / OpenGL / D3D / Metal / Vulkan
                         |
                         v
                       Surface
         null / RGBA bitmap / GPU attachment
                         |
                         v
                      Presenter
    GDI / DXGI / Core Graphics / Metal / X11 / file
                         |
                         v
                         Host
 .scr / .saver / XScreenSaver / runner / headless
                         |
                         v
                     OS services
```

## Portable Layer May Know

- deterministic time
- seed
- dimensions
- resolved configuration
- product state
- capability facts
- bounded surfaces
- diagnostics and fallback reasons

## Portable Layer Must Not Know

- `HWND`
- `HDC`
- `HINSTANCE`
- registry keys
- AppKit objects
- Core Graphics contexts
- Metal devices
- X11 windows
- SDL windows
- configuration dialogs
- installation mechanics

## Product-Local IR Rule

Each product owns its bounded IR unless a shared abstraction is proven by at
least two structurally different products or by a platform requirement.

Examples:

- Plasma may own a field-generation and treatment plan.
- Ricochet may own a trajectory and particle plan.
- City may own a scene and layout plan.
- Lifeforms may own a cellular simulation plan.
- Gallery may own an asset-selection and composition plan.

ScreenSave must not adopt one universal visual graph just to satisfy one
product's needs.

## Mandatory Reference Path

Every Core saver should retain a reference realization that can operate with:

- scalar computation
- bounded CPU bitmap surface
- software rendering
- native low-level presenter
- deterministic seed and timeline

The reference path is the preservation baseline, compatibility fallback,
deterministic test oracle, headless capture source, and visual-regression
reference.

## Compatibility Shim Rule

The current v1 public saver and renderer contracts stay preserved until a v2
adapter proves:

- equivalent autonomous `.scr` behavior on Windows
- GDI/reference fallback
- deterministic headless execution
- no OS handles in portable product logic
- product-local IR boundaries
- evidence bundle output
