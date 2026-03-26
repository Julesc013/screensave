# Saver API Specification

This document is normative.

## Intent

The saver API defines the long-term boundary between the shared host platform and individual saver products.
It is intentionally conceptual in Series 00 so the repository can preserve a stable shape without freezing low-level function signatures too early.

## Lifecycle Model

A saver instance is expected to move through these conceptual stages:

1. Discovery and metadata loading.
2. Configuration and preset resolution.
3. Session creation for a specific run mode.
4. Renderer attachment and capability selection.
5. Repeated update and render steps.
6. Resize, suspend, resume, or focus-related transitions as needed.
7. Orderly shutdown and state release.

The host owns mode selection and session orchestration.
The saver owns its visual behavior within the resources and capabilities granted to it.

## Host Responsibilities

The host layer is responsible for:

- Parsing classic screensaver invocation modes such as configure, preview, and full-screen execution.
- Creating and owning top-level windows and preview embedding.
- Selecting the renderer backend from the available capability set.
- Loading shared defaults, product defaults, presets, themes, and user overrides.
- Providing timing, frame pacing, and deterministic seed material to the saver session.
- Enforcing orderly startup and shutdown boundaries.

## Saver Responsibilities

A saver product is responsible for:

- Declaring stable product metadata and supported capability class.
- Defining product-local presets, themes, and content references.
- Creating session-local state from the resolved config and supplied seed.
- Updating that state over time without corrupting host-owned resources.
- Rendering only through the platform renderer contract or other explicitly allowed platform surfaces.

A saver must not own process-wide policy, global windowing decisions, or unrelated platform services.

## Renderer-Facing Boundary

Products should target the abstract renderer contract, not a raw backend implementation.

This means:

- Baseline savers should be able to run on the GDI contract alone.
- Savers may describe optional enhanced behavior for higher capability classes.
- Direct backend calls belong in the platform layer unless an explicit later spec introduces a tightly controlled exception.

## Config, Presets, And Themes

Each saver may define:

- A default configuration.
- Named presets intended for curated behavior bundles.
- Themes that affect presentation without redefining the whole runtime model.

Shared config keys belong in the shared schema.
Product-local keys stay product-local unless they are reused enough to justify promotion into the shared platform contract.

## Deterministic Seeds

The host provides seed material to each saver session.
Given the same product version, resolved preset, theme, mode, and seed, a saver should produce materially consistent behavior on the same capability class unless the release notes state otherwise.

Determinism is intended for repeatability, debugging, and validation evidence.
It does not require every backend to produce bit-identical pixels across all machines.

## Long-Run Stability

Savers are expected to remain stable during long unattended runs.

They must avoid:

- Unbounded memory growth.
- Unbounded handle growth.
- Time-step logic that becomes unstable over long durations.
- Persistent drift between config intent and actual runtime state.

Later implementation series may refine the low-level host interface, but they must preserve these conceptual responsibilities.
