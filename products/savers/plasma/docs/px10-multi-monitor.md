# PX10 Multi-Monitor

## Purpose

This document freezes the actual supported Plasma multi-monitor baseline.
It consumes the shared `SY10` host contract and states exactly what Plasma does and does not support today.

## Current Supported Baseline

Plasma currently supports the shared host baseline:

- one saver window spanning the Windows virtual desktop in fullscreen
- one preview child-window path
- one Plasma session and one product state stream per host session
- topology-change rebinding through the shared host path

Plasma does not currently create:

- independent per-monitor saver sessions
- per-monitor renderer selection
- hero and follower choreography
- monitor-specific journey or seed logic
- phase-offset monitor variants

## Supported Modes Today

| Mode | Current Product Meaning | Status | Evidence |
| --- | --- | --- | --- |
| `mirrored` | one shared Plasma session spans the virtual desktop; all visible output comes from that single session | current baseline | [`../../../../validation/notes/c05-windows-integration-matrix.md`](../../../../validation/notes/c05-windows-integration-matrix.md) |
| `synchronized_shared_state` | one selection state, one RNG stream, one transition state, one presentation state | current baseline | current product runtime shape plus shared host contract |
| `phase_offset_variants` | no per-monitor phase offset model exists | unsupported | product-local runtime has no per-monitor session split |
| `independent_per_monitor_state` | no independent per-monitor product state exists | unsupported | current host and product shape |
| `hero_follower_roles` | no hero or follower role assignment exists | unsupported | current host and product shape |

## Preview And Fullscreen Interaction

- Preview uses the shared child-window host path rather than a fullscreen path.
- Plasma still runs as one product session in preview mode, not as a different feature family.
- Preview-safe behavior is product-local and is described in [`px10-environment-safety.md`](./px10-environment-safety.md).
- Fullscreen multi-monitor behavior inherits the shared virtual-desktop host path and does not add monitor-specific product choreography.

## Mixed-Topology Limitations

Current repo truth does not support claims about:

- mixed refresh-rate monitor policy
- monitor-specific renderer downgrade choices
- one monitor dark while another remains active
- independent monitor pause, reseed, or transition timing
- different preset or theme choices per monitor

Those areas must remain caveated until a later product or host tranche implements and proves them explicitly.

## What Remains Product-Local

Even though the host path is shared, the following stay Plasma-local:

- any future hero and follower choreography
- any future shared-seed or reseed policy across displays
- any future transition suitability rules for specific topologies
- any future monitor-aware presentation or selection policy

`PX10` does not promote any of those ideas into shared law.

## Support Caveats That Remain

- No fresh mixed-topology hardware rerun was added in this tranche.
- No independent per-monitor behavior is implemented.
- Fullscreen `.scr` behavior still must be read from real host-path evidence, not from BenchLab alone.
- The current support claim is a bounded baseline, not a multi-monitor feature framework.

## Scope Boundary

`PX10` makes the current multi-monitor baseline explicit and truthful.
It does not widen the product into monitor-aware choreography work.
