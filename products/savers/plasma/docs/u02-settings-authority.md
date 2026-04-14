# U02 Settings Authority Rewrite

`U02` moves Plasma's visual grammar authority out of runtime-side preset lookup and into resolved settings state.

## What Changed

- `plasma_config`, settings resolution, and `plasma_plan` now carry explicit output-family, output-mode, treatment-slot, and presentation-mode fields.
- `plasma_plan_compile` now resolves generator family, output family, output mode, filter treatment, emulation treatment, accent treatment, presentation mode, speed, detail, resolution, and smoothing from resolved settings instead of re-reading preset defaults as the final authority.
- preset application still seeds the config with a curated bundle, but that bundle is now just the starting settings state rather than a hidden second runtime plan.
- transition target commits now preserve real user overrides when moving from one preset identity to another, instead of resetting the runtime grammar to the target preset bundle unconditionally.
- advanced-lane bind no longer silently adds `blur` and `overlay_pass`, and premium-lane bind no longer silently promotes `flat` into `heightfield`.

## Runtime Truth

After `U02`, the runtime truth is:

1. preset selection chooses identity, default theme pairing, and a starting bundle
2. resolved settings decide the compiled visual grammar
3. lane binders may degrade unsupported requests, but they do not invent new grammar behind the user's back
4. requested-versus-resolved state remains visible in the plan

That keeps `Plasma Classic` alive as preserved content identity and compatibility surface without keeping a separate Classic-only execution regime.

## Current Limits

- `sampling_treatment` still clamps to `none`; it remains a non-claim surface and should stay hidden from end-user UI until a real second sampling path exists.
- the dialog surface is still pre-salvage and will be rebuilt in `U03`; the runtime is now ahead of the UI.
- BenchLab already reports the resolved grammar fields, but richer requested-versus-resolved reporting for each setting family is still part of the later salvage phases.

## Proof

The `plasma` smoke harness now covers:

- import/export round-tripping for the new product grammar fields
- direct settings-to-plan authority for generator, output, treatment, and presentation choices
- honest premium degradation with requested presentation preserved and resolved presentation flattened on lower lanes
- honest advanced-treatment degradation for explicit `blur` and `overlay_pass` requests
- transition commit behavior that preserves user overrides across preset morphs

Build and smoke proof for this phase was run with Visual Studio 2022 using the installed MSVC `v141_xp` toolset.
