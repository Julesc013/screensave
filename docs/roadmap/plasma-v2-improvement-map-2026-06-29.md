# Plasma v2 Improvement Map - 2026-06-29

Status: planning and architecture assessment, not normative contract law.

Reference point:

- Repo commit: `f6d1101a585777726e4bd4d6072f5e6d6242e84c`
- External input report: `E:/Downloads/ScreenSave_Plasma_Architecture_Report_2026-06-29.md`
- External diagram pack: `E:/Downloads/ScreenSave_Plasma_Architecture_Diagrams_2026-06-29.md`
- Current Plasma artifact: `out/msvc/vs2017_xp/Release/plasma/plasma.scr`
- Current artifact SHA-256: `50080152f3b8300ea2cc7242a861af648b875911626b8fc804475f3695f58f64`

This note documents what the architecture now has, what it is missing, and the
highest-value improvements needed to make ScreenSave and Plasma more portable,
modular, extensible, powerful, efficient, performant, intelligent, supported,
customizable, redesignable, and compatible without breaking the project
doctrine.

## Current Ground Truth

The live project state records Plasma v2 as:

```text
status = publication-ready
stable = true
release_promotion = accepted
publication_prep = ready
publication = not-published
opened_next = plasma-v2-publication
```

The important boundary is that publication-ready is not publication. The
release packet is prepared, but no public upload, release page publication,
GitHub release creation, public download-link verification, compatibility
certification broadening, public SDK freeze, Manager install mutation,
Workbench MVP, all-saver migration, or platform expansion is included.

The current high-resolution Plasma default-demo evidence records an adaptive
field path with renderer-tier field budgets:

| Renderer tier | Current semantic field cap |
| --- | --- |
| GDI | `1280x720` |
| GL11 | `1920x1080` |
| GL21 | `2560x1440` |
| GL33 / GL46 | `6144x2304` |

The current architecture is best described as:

```text
ScreenSave:
  portable deterministic meaning
  + native saver delivery
  + proof-backed artifacts
  + optional Manager / Workbench / AIDE surfaces

Plasma:
  product-owned direct controls
  -> plasma_v2_spec
  -> plasma_v2_plan
  -> plasma_v2_runtime
  -> deterministic semantic field
  -> material and treatment mapping
  -> GDI floor / optional GL presentation
  -> proof and publication gates
```

## What Is Already Strong

The architecture is strongest where it refuses to collapse separate concerns:

| Strength | Why it matters |
| --- | --- |
| Product-owned semantics | Plasma can become a real field instrument instead of being forced into a generic visual graph. |
| C89, x86 Win32 `.scr`, ANSI host, GDI floor | The retro-compatible baseline remains protected while modern lanes grow around it. |
| Optional renderer ladder | GL11, GL21, GL33, and GL46 can improve presentation without becoming hidden minimum requirements. |
| Proof kernel and `libsslab` | CI, Workbench, AIDE, and release tooling can inspect the same deterministic product facts. |
| Evidence-classed compatibility | The project can talk honestly about buildable, targeted, certified, and experimental states. |
| Data-first extension policy | Packs and VisualIntent candidates can extend behavior without admitting arbitrary runtime plugins. |
| Publication-prep boundary | The repo has a real release packet without pretending it has already been published. |

## The Biggest Missing Pieces

These are the main gaps between the current state and the "most portable,
modular, extensible, powerful, efficient, supported, customizable, redesignable,
compatible" version of the system.

| Gap | Current state | Missing improvement |
| --- | --- | --- |
| Public publication | Packet is publication-ready and local. | PAW-K must create the release archive, public release page, public links, verified checksums, provenance, and publication receipt. |
| Hardware visual proof | BenchLab and gate evidence exist. | Fullscreen screenshots/contact sheets and human visual review on target hardware, including high-res Plasma. |
| Compatibility certification | Artifact-specific and evidence-classed. | Real OS/hardware matrix evidence before any broad Windows preservation claim. |
| Workbench | Proof cockpit/model shell exists. | Graphical Workbench MVP with live direct controls, preview, save/export, proof, compare, and review. |
| Manager | Preview/readiness evidence exists. | Install/apply/repair/rollback UX with proof-aware package handling. |
| Performance telemetry | Some performance envelope evidence exists. | Frame-time, memory, upload cost, resize cost, and soak thresholds for each renderer tier and display shape. |
| Native GPU field synthesis | Current high-res path is CPU semantic-field synthesis with renderer upload/presentation. | Optional GL33/GL46 shader/native-field implementation proven against the software oracle. |
| Multi-monitor timing | One virtual-desktop saver window and one timer request. | Per-monitor evidence, mixed-refresh policy, and optional per-monitor scheduling if it can preserve compatibility. |
| SDK and pack stability | Packc is v1-candidate/data-only. | Stable pack schema, migration rules, examples, linting, provenance, and author-facing docs. |
| Intelligent authoring | VisualIntent candidates are bounded inputs. | Local, deterministic suggestion/ranking loops that produce specs, not code or hidden runtime truth. |

## Improvement Principles

The project should optimize by strengthening the existing doctrine, not by
replacing it:

1. Keep software/reference truth canonical.
2. Keep GDI mandatory and every higher renderer optional.
3. Keep product meaning in product specs and plans.
4. Prefer data packages over runtime executable plugins.
5. Treat Workbench, Manager, AIDE, CI, and release tooling as consumers of
   ScreenSave truth, not owners of it.
6. Promote claims only when matching evidence exists.
7. Move shared code to `platform/` only after it is genuinely shared or
   required by platform law.

## Portability Improvements

The next portability gains should come from isolating semantic truth from host
delivery, not from weakening the Windows saver baseline.

Recommended work:

- Make Plasma's product core easier to run as a host-neutral proof unit through
  the portable v2 and `libsslab` paths.
- Add a clean MinGW-family build proof where feasible, keeping the VS2017 XP
  lane intact.
- Add reproducible clean-source artifact rebuild receipts before publication.
- Expand artifact audit evidence beyond PE headers into runtime environment
  facts where real machines or VMs are available.
- Keep future macOS, Linux, WebAssembly, or ARM64 work as shared semantics plus
  native hosts, not as a lowest-common-denominator replacement shell.
- Add a "portable semantic conformance" test that proves the same Plasma spec,
  seed, frame, and dimensions produce the same reference output across build
  environments.

Do not improve portability by adding mandatory middleware, network services,
Unicode-only paths, x64-only baselines, or GPU-mandatory runtime behavior.

## Modularity Improvements

Plasma should become more modular by making its own pipeline more explicit,
not by adopting one universal graphics graph.

The stable product-local module boundaries should be:

```text
input / preset / pack / VisualIntent candidate
-> plasma_v2_spec
-> plasma_v2_plan
-> runtime state and buffers
-> coordinate providers
-> sources
-> generators
-> modifiers
-> output transform
-> material mapping
-> treatment stack
-> presentation policy
-> renderer upload / blit
```

Recommended work:

- Define small product-local tables or descriptors for field sources,
  generators, modifiers, materials, treatments, and routing policies.
- Keep each module deterministic, fixed-budget, and allocation-free per frame
  where it touches the hot loop.
- Add unit or smoke checks for each pipeline boundary, especially spec-to-plan
  and plan-to-runtime degradation.
- Record module-level claim boundaries so experimental families cannot silently
  enter stable.
- Promote shared helpers to `platform/` only after at least two structurally
  different products need them.

## Extensibility And Customization Improvements

The extensible version of Plasma is a safe instrument with reviewable
configuration, not an unrestricted plugin runtime.

Recommended work:

- Stabilize `packc` from v1-candidate toward a documented data-pack contract.
- Add pack migration rules and versioned schema validation.
- Make presets explicit bookmarks over direct controls, not hidden runtime
  masters.
- Add Workbench direct-control editing for Field, Scale, Complexity, Motion,
  Speed, Warp, Feedback, Material, Contrast, Brightness, Softness, Treatment,
  and Seed.
- Add A/B capture comparison and semantic diff for specs, plans, and rendered
  outputs.
- Add import/export of reviewable pack candidates with checksums, provenance,
  and proof references.

## Performance And Efficiency Improvements

The current high-resolution path is a strong semantic upgrade, but it is still
CPU field synthesis plus GDI/OpenGL presentation/upload. That is the right
reference truth, but it is not the final high-refresh, high-wall-performance
architecture.

Recommended work:

- Add frame-time telemetry for field synthesis, material/treatment mapping,
  upload/blit, presentation, resize, and full-frame total.
- Add memory and allocation telemetry, with a hard no-hot-loop-allocation gate
  for stable paths.
- Add renderer-tier performance envelopes for GDI, GL11, GL21, GL33, and GL46.
- Add adaptive quality policy that can lower semantic field size or effect cost
  without changing product identity.
- Add optional tiling or row-chunk scheduling for CPU field synthesis if it
  helps without hurting determinism.
- Explore optional SIMD or multithreaded CPU paths only behind exact-reference
  proof and deterministic scheduling rules.
- Add GL33/GL46 native-field or shader synthesis as an optional realization,
  verified against the CPU/reference oracle within documented tolerances.
- Keep persistent texture/buffer allocation outside the frame loop for GL33 and
  GL46.

## Intelligence Improvements

The intelligent version of ScreenSave should help people author and evaluate
deterministic products. It should not become a hidden runtime, a source-code
generator, or an automatic release authority.

Recommended work:

- Treat text, image, and video inputs as VisualIntent sources only.
- Generate bounded candidate sets, each with explicit Plasma specs, rationale,
  proof captures, and claim boundaries.
- Add deterministic candidate IDs and stable candidate ordering.
- Add local ranking tools based on proof facts, contrast, motion, safety,
  performance, and reviewer decisions.
- Add explainability: every suggestion should say which spec fields it changed
  and why.
- Keep model/provider/network calls disabled unless a future reviewed gate
  explicitly admits them for a non-runtime authoring surface.

## Support And Compatibility Improvements

The strongest support posture is evidence-classed and artifact-specific.

Recommended work:

- Publish only after PAW-K records the actual publication receipt.
- Add a real compatibility matrix for Windows versions and VM/hardware
  profiles, using `certified`, `binary-audited`, `buildable`, `targeted`,
  `experimental`, `unsupported`, or `retired` precisely.
- Add public support wording that distinguishes GDI baseline, GL optional
  tiers, high-res demo evidence, and future shader/native-field work.
- Add installer, uninstall, rollback, and portable ZIP validation for the
  Plasma publication package if Plasma is released separately from Core.
- Add crash-safe diagnostics and fallback reason reporting around renderer
  selection.
- Keep GL21, GL33, GL46, D3D, Vulkan, Metal, macOS, Linux, public SDK, and
  executable plugins out of stable claims until separate gates admit them.

## Redesignability Improvements

Redesignability should come from typed, testable product semantics.

Recommended work:

- Make every visual family reducible to a stable spec and plan.
- Add plan serialization and diffing so a reviewer can understand exactly what
  changed.
- Add compatibility-preserving migrations for old presets and packs.
- Add a WorkbenchProject model that can hold source intent, reduced specs,
  proof captures, comparison sets, review decisions, and export candidates.
- Keep experimental graph or node editing as an authoring front end only; any
  graph must lower through validated semantic data before execution.

## Best Next Sequence

The highest-leverage path is:

1. Decide PAW-K direction: publish Plasma v2 now or explicitly defer
   publication.
2. If publishing, run PAW-K0 first: real-display fullscreen proof, contact
   sheets, renderer-tier comparisons, frame-time or pacing notes, and a human
   visual note.
3. If PAW-K0 passes and the owner still chooses publication, run PAW-K as a
   narrow publication turn: final archive, release notes, public links,
   checksums, provenance, release page, and publication receipt.
4. If publication is deferred or PAW-K0 holds, run PAW-W1: Workbench MVP over
   Plasma direct controls and `libsslab` live preview.
5. Add performance telemetry and thresholds before native shader work.
6. Add optional GL33/GL46 native-field synthesis only after the CPU/reference
   oracle and performance gates are strong enough to judge it.
7. Add Manager install/apply/repair only after publication and Workbench needs
   are clear.

## What Not To Do

Avoid these shortcuts:

- Do not publish just because the packet is publication-ready.
- Do not broaden compatibility certification from buildable or targeted
  evidence.
- Do not make GL33, GL46, shaders, or GPU-native synthesis the hidden minimum.
- Do not turn Workbench, Manager, AIDE, or VisualIntent into product truth.
- Do not create a universal graphics graph that erases product-owned semantics.
- Do not admit arbitrary DLL/script/plugin execution as the extension model.
- Do not move Plasma-specific meaning into `platform/` merely because it feels
  reusable.

## Bottom Line

ScreenSave and Plasma are now architecturally coherent. The next level is not a
bigger rewrite. It is a sequence of bounded upgrades:

```text
publish direction or defer explicitly
-> prove high-res behavior on real displays through PAW-K0
-> publish only through PAW-K, or hold intentionally
-> make Workbench a real direct-control authoring tool
-> add performance telemetry and optional native acceleration
-> certify compatibility only with real evidence
-> stabilize data-pack and authoring flows
```

That path makes the system more powerful while preserving the thing that makes
it special: portable meaning, native delivery, deterministic proof, and optional
automation.
