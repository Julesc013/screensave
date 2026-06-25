# libsslab ABI v0

Status: active private proof-runtime ABI.

## Purpose

`libsslab` is the private compiled proof runtime that will replace
Nocturne-specific proof executables during `proof-kernel-v1-ricochet`.

It is the shared execution spine for:

- fixed-step proof runs;
- canonical RGBA8 capture production;
- exact comparison;
- lifecycle receipts;
- profiling and short-soak receipts;
- Proof Bundle v1 assembly hooks;
- future Workbench, CI, and AIDE evidence consumption.

It is not a public saver runtime API, not the portable v2 semantic seam, not a
plugin ABI, and not a compatibility certification.

## Current Host Boundary

ABI v0 is a current Windows/32-bit v1 proof ABI. It may be compiled and used on
developer hosts that can execute the proof runner, but it must block proof
promotion when the ABI-width precondition is not met:

```text
sizeof(unsigned long) == 4
```

This is deliberate. Existing product headers and Ricochet state still use
current v1 assumptions. Gate C must replace this with fixed-width,
host-neutral portable v2 types before native LP64/Linux equivalence is claimed.

## Required Shape

All public structs in the private ABI must carry:

- `size`;
- `abi_version`;
- explicit ownership/lifetime rules where pointers are present.

Opaque handles must be used for:

- context;
- product;
- session;
- capture;
- proof profile.

The ABI must expose distinct operations for:

- create context;
- open product;
- create session;
- resize;
- fixed-step advance;
- render to RGBA8;
- capture;
- compare;
- lifecycle;
- profile;
- short soak;
- write proof bundle.

## Product Adapter Boundary

Product adapters may know product-local state and semantics. The common
runtime may know only:

- product key;
- preset or specification key;
- seed;
- dimensions;
- fixed timeline;
- lifecycle operation;
- RGBA8 surface;
- diagnostics and receipts.

Product adapters must not make Workbench, AIDE, Python, or CI the owner of
product simulation or rendering semantics.

## Python Boundary

Python may:

- load catalog/proof profiles;
- launch the compiled runner;
- format JSON receipts;
- compare receipt status;
- orchestrate CI/AIDE command surfaces.

Python must not:

- implement product simulation;
- implement product rendering;
- compute product-owned state transitions;
- become the proof oracle.

## Gate B Acceptance

`libsslab` ABI v0 is accepted when:

- Nocturne and Ricochet run through one compiled library;
- Nocturne retains the established exact canonical hash;
- Ricochet has multi-frame exact baselines;
- both products emit generic lifecycle receipts;
- both products emit profiling and short-soak evidence;
- Proof Bundle v1 consumes the complete profile-driven receipt set;
- CI runs the Nocturne/Ricochet proof matrix;
- Workbench can link the same library without creating another runner.

Type: private proof-runtime ABI contract.
