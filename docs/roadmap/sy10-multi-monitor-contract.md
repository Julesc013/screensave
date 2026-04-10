# SY10 Multi-Monitor Contract

This document freezes the shared multi-monitor host and present policy for the current post-PL program.

It records the host behavior the repo already implements and the vocabulary later product work may use without quietly inventing new monitor-management law.

## Purpose

Use this contract when describing:

- fullscreen multi-monitor host behavior
- preview behavior in relation to monitor topology
- topology-change or hotplug support expectations
- whether a behavior is current shared host truth or a future product-local idea

## Shared Host And Present Vocabulary

Use these terms with the following meanings:

| Term | Meaning |
| --- | --- |
| `mirrored` | the same effective presentation is shown across multiple displays without distinct per-monitor content identity |
| `synchronized_shared_state` | one session and one shared runtime state drive the visible multi-monitor result |
| `phase_offset_variant` | displays intentionally differ by phase or timing while still belonging to one coordinated product concept |
| `independent_per_monitor_state` | each monitor owns its own session or materially separate runtime state |
| `hero_follower_roles` | one monitor is treated as primary while others follow a derived or secondary policy |

Current repo-truth note:
the shared host today only guarantees `synchronized_shared_state` through one fullscreen saver window spanning the virtual desktop.
The other terms are frozen vocabulary for later support discussion, not current shared runtime guarantees.

## Current Shared Multi-Monitor Policy

Current shared host truth is:

- fullscreen mode uses one saver window spanning the Windows virtual desktop
- preview mode uses one child window embedded in the preview parent
- topology changes rebinding preview or fullscreen bounds are part of the current host contract
- independent per-monitor saver sessions are not current shared behavior
- monitor-specific renderer selection is not current shared behavior

This is a correctness-first host policy, not a generalized multi-monitor feature framework.

## Topology And Enumeration Expectations

When later notes discuss topology, they should distinguish:

- `single_monitor`
- `virtual_desktop_span`
- another truthful monitor layout descriptor when the host path actually observed it

Support-facing rule:
describe the topology the host actually used.
Do not imply per-monitor orchestration when the host only created one virtual-desktop-spanning session.

## Preview And Fullscreen Distinction

The shared distinction is:

- `preview`: one bounded child-window session inside the Windows preview parent
- `fullscreen`: one top-level saver session sized to the current virtual desktop

The current shared host contract requires each mode to keep its own bounds truthful when display layout changes.

## Hotplug And Topology-Change Expectations

`SY10` freezes the current support-facing expectation for topology changes:

- the host should re-evaluate preview or fullscreen bounds when Windows reports a display change
- the current contract is bounded rebinding and sizing correctness
- the current contract does not fan out new per-monitor sessions at runtime

This is the only hotplug expectation `SY10` formalizes now.

## What Remains Product-Local

The following stay product-local for now:

- hero and follower semantics
- synchronized journeys, seeds, or transition choreography across displays
- phase-offset multi-monitor art direction
- per-monitor content selection
- per-monitor renderer or lane policy

If later work needs any of those as a true shared host contract, it must come back through `MX` and satisfy the promotion rule.
