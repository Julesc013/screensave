# Backlog Inventory

This is the `C11` backlog baseline.
It turns the remaining idea space into explicit, routed work instead of leaving it as scattered working titles or ad hoc follow-up notes.

## Cross-Cutting And Shared Items

| ID | Focus | Category | Priority | Effort | Risk | Depends On | Target Bucket | Owner Area | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `X-01` | Config-dialog wording, default naming, and per-saver metadata consistency | shared-platform/runtime | `P1` | `M` | low | none | `C12-X1` | `platform` | Align labels and manifest-facing language without changing product ownership |
| `X-02` | Suite and saver metadata parity across README, manifest summaries, and status notes | docs/SDK/tooling | `P1` | `S` | low | none | `C12-X1` | `docs` | Reduce drift between suite/browser surfaces and product docs |
| `X-03` | Preset/theme/pack curation sweep plus authoring examples refresh | docs/SDK/tooling | `P2` | `M` | low | none | `C12-X1` | `sdk` | Keep the shared settings architecture discoverable and consistent |
| `X-04` | Core and installer payload refresh once complete local outputs are available | release/distribution | `P1` | `M` | medium | `X-02` | `REL-R` | `distribution` | Packaging truth should catch up to the post-`C15` doctrine and the real product line |
| `X-05` | Renderer-tier messaging, fallback wording, and per-product capability evidence sweep | renderer-tier work | `P1` | `M` | medium | none | `C12-X2` | `renderer` | Keep `gdi`, `gl11`, and `gl21` claims honest across the suite |
| `X-06` | Preview, fullscreen, persistence, and long-run smoke coverage sweep | validation/hardening | `P1` | `M` | medium | none | `C12-X2` | `validation` | Strengthen evidence before family polish waves fan out |
| `X-07` | Suite, anthology, and BenchLab role-boundary polish | suite/meta layer | `P2` | `M` | low | `X-02` | `C12-X1` | `suite` | Keep browser, meta-saver, and diagnostics roles easy to understand |
| `X-08` | Contributor-surface follow-on polish for copy-map, examples, and validation notes | docs/SDK/tooling | `P2` | `S` | low | `X-03` | `C12-X2` | `sdk` | Keep `C10` durable after cross-cutting cleanup lands |

## Saver-Local Backlog

| ID | Product | Focus | Category | Priority | Effort | Risk | Depends On | Target Bucket | Owner Area | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `SAV-NOC-01` | `nocturne` | Dark-room preset curation, preview calmness, and default wording cleanup | saver-local polish | `P1` | `S` | low | `X-01` | `Wave-A` | `saver:nocturne` | `C13` Wave A lands the first premium pass while keeping Nocturne the low-distraction reference product |
| `SAV-RIC-01` | `ricochet` | Trail balance, object pacing, and theme contrast pass | saver-local polish | `P1` | `M` | low | `X-01` | `Wave-A` | `saver:ricochet` | `C13` Wave A tunes the motion-family anchor without shared churn |
| `SAV-DEP-01` | `deepfield` | Density, camera comfort, and preview-safe defaults | saver-local polish | `P1` | `M` | medium | `X-01`, `X-06` | `Wave-A` | `saver:deepfield` | `C13` Wave A keeps atmospheric motion calm over long runs |
| `SAV-PIP-01` | `pipeworks` | Rebuild cadence, low-detail readability, and config wording pass | saver-local polish | `P1` | `M` | low | `X-01` | `Wave-B` | `saver:pipeworks` | `C13` Wave B keeps grid-growth polish product-local while strengthening curated pulse and rebuild behavior |
| `SAV-LIF-01` | `lifeforms` | Reseed policy, ruleset defaults, and theme clarity | saver-local polish | `P1` | `M` | medium | `X-01`, `X-06` | `Wave-B` | `saver:lifeforms` | `C13` Wave B improves contemplative long-run behavior through calmer reseed choreography and theme curation |
| `SAV-SIG-01` | `signals` | Panel density, detail-level readability, and preset naming cleanup | saver-local polish | `P2` | `M` | low | `X-01` | `Wave-B` | `saver:signals` | `C13` Wave B keeps system-interface polish product-local while strengthening panel cadence and preset identity |
| `SAV-PLA-01` | `plasma` | Effect preset curation, pack example growth, and export/import parity | saver-local polish | `P2` | `M` | medium | `X-03` | `Wave-A` | `saver:plasma` | `C13` Wave A preserves `plasma` as the framebuffer anchor while strengthening its curated presets |
| `SAV-PHO-01` | `phosphor` | GDI/GL presentation parity and preset-description cleanup | saver-local polish | `P2` | `M` | medium | `X-05` | `Wave-A` | `saver:phosphor` | `C13` Wave A keeps capability claims honest while polishing phosphor identity |
| `SAV-MEC-01` | `mechanize` | Motion restraint, config labels, and default pacing pass | saver-local polish | `P2` | `M` | low | `X-01` | `Wave-B` | `saver:mechanize` | `C13` Wave B avoids promoting machine-specific logic to shared code while improving exhibit pacing and scene variants |
| `SAV-ECO-01` | `ecosystems` | Habitat balance, sample pack growth, and long-run tuning | saver-local polish | `P2` | `M` | medium | `X-03`, `X-06` | `Wave-B` | `saver:ecosystems` | `C13` Wave B keeps habitat behavior product-owned while improving regroup choreography and calmer room-safe defaults |
| `SAV-STO-01` | `stormglass` | Precipitation readability, winter/rain balance, and preview safety | saver-local polish | `P2` | `M` | medium | `X-06` | `Wave-C` | `saver:stormglass` | Scenic polish only, no new framework |
| `SAV-TRA-01` | `transit` | Scene pacing, light balance, and route-pack curation | saver-local polish | `P2` | `M` | medium | `X-03` | `Wave-C` | `saver:transit` | Keep route-specific content inside Transit |
| `SAV-OBS-01` | `observatory` | Exhibit pacing, low-detail fallback clarity, and config wording | saver-local polish | `P2` | `M` | low | `X-05` | `Wave-C` | `saver:observatory` | Preserve celestial exhibit identity |
| `SAV-VEC-01` | `vector` | Detail scaling, performance tuning, and renderer-tier evidence pass | saver-local polish | `P1` | `L` | high | `X-05`, `X-06` | `Wave-C` | `saver:vector` | Heavyweight product with renderer-sensitive tuning |
| `SAV-EXP-01` | `explorer` | Traversal comfort, config disclosure, and deterministic replay polish | saver-local polish | `P2` | `L` | medium | `X-01` | `Wave-C` | `saver:explorer` | Keep traversal-specific behavior product-local |
| `SAV-CIT-01` | `city` | Night-scene contrast, skyline curation, and preset balance | saver-local polish | `P2` | `M` | medium | `X-01` | `Wave-C` | `saver:city` | Urban presentation polish only |
| `SAV-ATL-01` | `atlas` | Fractal progression pacing, pack growth, and summary-text cleanup | saver-local polish | `P1` | `L` | high | `X-03`, `X-05` | `Wave-C` | `saver:atlas` | Includes all future Infinity Atlas-style expansion ideas under Atlas only |
| `SAV-GAL-01` | `gallery` | Compatibility preset curation, tier explanation, and showcase messaging | saver-local polish | `P1` | `M` | medium | `X-05` | `Wave-C` | `saver:gallery` | Keep the renderer-showcase role explicit and honest |
| `SAV-ANT-01` | `anthology` | Selection defaults, interval tuning, fallback summaries, and family-filter clarity | suite/meta layer | `P1` | `M` | medium | `X-05`, `X-07` | `Wave-C` | `anthology` | Meta-saver remains one bounded `.scr`, not the suite app |

## Non-Saver Product Backlog

| ID | Product | Focus | Category | Priority | Effort | Risk | Depends On | Target Bucket | Owner Area | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `APP-SUI-01` | `suite` | Discovery polish, launch/config clarity, and local artifact truthfulness | suite/meta layer | `P1` | `M` | low | `X-02`, `X-07` | `REL-R` | `suite` | Keep Suite as browser/launcher/config surface only; saver-polish waves do not absorb it |
| `APP-BEN-01` | `benchlab` | Diagnostics overlay cleanup, comparison ergonomics, and evidence capture notes | validation/hardening | `P2` | `M` | low | `X-06`, `X-07` | `REL-R` | `benchlab` | BenchLab remains diagnostics-only and only gets narrow release-hardening support |
| `SDK-01` | `sdk` | Template copy-forward docs, examples refresh, and validator polish | docs/SDK/tooling | `P2` | `S` | low | `X-03`, `X-08` | `C12-X2` | `sdk` | Keep contributor guidance aligned with the live tree |

## Distribution And Release Follow-On

| ID | Focus | Category | Priority | Effort | Risk | Depends On | Target Bucket | Owner Area | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `REL-01` | Refresh the Core candidate payload and freeze the actual Core baseline | release/distribution | `P1` | `M` | medium | `X-04` | `REL-R` | `distribution` | Apply the `C15` Core gate to the current saver line once fresh outputs exist |
| `REL-02` | Refresh the Installer channel, registration notes, and uninstall evidence against the frozen Core baseline | release/distribution | `P1` | `M` | medium | `REL-01` | `REL-R` | `distribution` | Keep Installer as a companion channel derived from Core |
| `REL-03` | Machine-wide install, repair, and prune policy evaluation | release/distribution | `P3` | `L` | high | `REL-02` | `REL-R` | `distribution` | Deferred until the active polish line is stable |

## Parked Concept Pool

These items are intentionally not on the active continuation line.
They exist so future idea work has a home without interrupting `C12` through `C16`.

| ID | Working Title | Likely Route | Priority | Effort | Risk | Target Bucket | Notes |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `PARK-01` | `weather_glass` | new saver product | `Parked` | `M` | medium | `PARKED` | Ambient utility-style concept; only revive if baseline-safe and distinct from `stormglass` |
| `PARK-02` | `starfield` | new saver product | `Parked` | `S` | low | `PARKED` | Classical depth/drift concept; revisit only if it is not redundant with `deepfield` |
| `PARK-03` | `ribbon_drift` | new saver product | `Parked` | `S` | low | `PARKED` | Motion-study concept; keep separate from current motion-family polish |
| `PARK-04` | `lattice` | new saver product | `Parked` | `M` | medium | `PARKED` | Grid/topology concept; revisit only after `pipeworks` and `lifeforms` polish settles |
| `PARK-05` | `signal_garden` | new saver product | `Parked` | `M` | medium | `PARKED` | Systems-oriented concept; do not let it redefine `signals` or `ecosystems` |
| `PARK-06` | `midnight_city` | new saver product | `Parked` | `M` | medium | `PARKED` | Scenic concept parked until `city` polish clarifies the gap |
| `PARK-07` | `canopy` | new saver product | `Parked` | `M` | medium | `PARKED` | Scenic layered concept gated by baseline-safe fallback rules |
| `PARK-08` | `glass_reef` | new saver product | `Parked` | `M` | high | `PARKED` | Scenic reflective concept; only revive if baseline-safe |
| `PARK-09` | `prism_hall` | new saver product or renderer showcase concept | `Parked` | `L` | high | `PARKED` | Heavyweight showcase concept gated by later renderer evidence |
| `PARK-10` | `pendulum_study` | new saver product | `Parked` | `S` | low | `PARKED` | Deterministic motion concept parked until current line polish is complete |
| `PARK-11` | `cell_garden` | new saver product | `Parked` | `M` | medium | `PARKED` | Automata-oriented concept; revisit only after `lifeforms` polish clarifies the gap |
| `PARK-12` | `particle_foundry` | new saver product or renderer R&D | `Parked` | `L` | high | `RND-R` | Long-run particle concept parked pending renderer and validation evidence |
| `PARK-13` | `ecosystem_sketch` | new saver product | `Parked` | `M` | medium | `PARKED` | Simulation-oriented concept parked until `ecosystems` is more mature |

## Wave Summary

- `C12-X1` and `C12-X2` are the shared cleanup buckets that should run before saver-local wave work.
- `Wave-A` covers `nocturne`, `ricochet`, `deepfield`, `plasma`, and `phosphor`.
- `Wave-B` covers `pipeworks`, `lifeforms`, `signals`, `mechanize`, and `ecosystems`.
- `Wave-C` landed for `stormglass`, `transit`, `observatory`, `vector`, `explorer`, `city`, `atlas`, `gallery`, and `anthology` while keeping `suite` and `benchlab` limited to narrow validation support.
- `C14` final rerelease hardening preserved the release-candidate baseline.
- `C15` release doctrine and channel split are complete.
- `C16` Core release refresh and baseline freeze is the next scheduled continuation prompt.
- `REL-R` stays after the active polish line unless a release blocker forces it forward.
- `PARKED` and `RND-R` are intentionally out of the current continuation line.
