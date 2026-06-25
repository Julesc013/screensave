# Surface RGBA8 Contract v0

Status: private proof-kernel contract.

## Purpose

`surface_rgba8_v0` is the first explicit pixel surface for ScreenSave proof
work. It is intentionally private until at least two products prove the shape.

The surface exists so headless runs, Workbench captures, CI, and later AIDE
evidence packets can compare pixels without depending on a native window,
device context, OpenGL context, or operating-system presenter.

## Pixel Format

- channel order: red, green, blue, alpha
- bits per channel: 8
- row order: top-left origin
- row stride: `width * 4`
- color encoding: sRGB bytes unless a later proof profile says otherwise
- alpha: straight alpha, normally `255` for Core saver captures

## Ownership

- the surface owns its pixel buffer
- callers may request a mutable pixel pointer from private implementation code
- callers must not retain pointers after surface disposal
- allocation failure must be reported as a failed proof run, not as a support
  claim

## Raster Law

The first software renderer must define primitive behavior explicitly:

- clear writes every pixel in the surface
- filled rectangles clip to the surface bounds
- frame rectangles draw a one-pixel border inside the clipped rectangle
- lines use a deterministic integer algorithm
- polylines draw each adjacent segment in order
- out-of-bounds coordinates are clipped or ignored safely

No primitive may delegate proof output to unspecified platform rasterization.

## Scope Boundary

This contract does not create a public plugin ABI.
It does not replace `gdi` as the required Core floor.
It does not certify cross-platform support by itself.
