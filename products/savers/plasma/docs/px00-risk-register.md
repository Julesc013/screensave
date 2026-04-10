# PX00 Risk Register

This register records the main risks at the start of the post-PL program.

## Risk Register

| Risk Key | Category | Current Status | Meaning | Mitigation Direction |
| --- | --- | --- | --- | --- |
| `support_cross_hardware_gap` | support risk | open | Current proof comes from one capable validation machine, so support posture is stronger on the known machine than across the broader hardware field. | Treat broader proof as an early-wave requirement before stable widening. |
| `support_multi_monitor_bounded` | support risk | open | Plasma currently supports the shared one-window virtual-desktop baseline only, so broader monitor-topology expectations would overclaim current behavior. | Keep the current multi-monitor claim bounded until later product or host work proves anything broader. |
| `support_environment_safety_bounded` | support risk | open | Plasma has real preview-safe, long-run-stable, and descriptive dark-room posture, but broader safety-mode vocabulary is not yet modeled in runtime. | Keep richer safety terms explicit non-claims until later work lands real behavior and proof. |
| `proof_transition_bounded` | proof risk | open | Transition proof is real and now names the current direct classes, curated bridge cycles, and hard-cut or fallback grammar, but it is still limited to the curated implemented subset rather than the full theoretical matrix. | Keep transition claims bounded, use the explicit compatibility matrix, and require proof updates for any further coverage expansion. |
| `proof_dimensional_bounded` | proof risk | open | `heightfield` is the only dimensional mode backed by current repo reality. | Keep dimensional claims narrow until new modes are implemented and proved. |
| `proof_benchlab_forcing_bounded` | proof risk | open | BenchLab forcing is intentionally bounded and should not be described as a full unrestricted debugger. | Keep forcing support-facing and update proof surfaces whenever the forcing subset changes. |
| `proof_numeric_sla_gap` | proof risk | open | The repo has bounded qualitative envelopes, not numeric performance SLAs. | Avoid numeric promises until a later measurement pass exists. |
| `authoring_registry_partial` | architecture and governance risk | open | `PX20` makes sets, journeys, and pack provenance authorable, but the built-in preset and theme descriptor inventory is still not fully migrated into the same authored substrate. | Keep the partial boundary explicit and treat fuller migration as later-wave work. |
| `lab_shell_bounded` | support and tooling risk | open | The first Plasma Lab shell is real but intentionally CLI-first and bounded. It could be over-described as a full editor if later docs get sloppy. | Keep Lab claims narrow and tie them to the actual command surface. |
| `selection_foundation_bounded` | support and product risk | open | Weighted set fallback is now real, but richer anti-repeat memory, similarity logic, and safety-aware reasoning are still absent. | Keep selection-intelligence claims within the current weighted authored foundation until later waves land more. |
| `governance_scope_drift` | architecture and governance risk | open | Later work could try to use post-PL momentum to smuggle in runtime or platform changes outside its wave. | Enforce queue scope, wave gates, and write-surface discipline. |
| `shared_promotion_too_early` | shared-promotion risk | open | Plasma may discover reusable ideas before reuse is proven. `SY20` already kept the richer Wave 2 authoring substrate local, and later waves still need to preserve that discipline. | Apply the promotion rule strictly and keep richer authoring and provenance seams local until reuse is proven. |
| `product_identity_drift` | product-identity risk | open | Plasma could drift toward a generic suite redesign or scenic showcase identity. | Preserve `Plasma Classic`, lower-band-first posture, and field-derived identity as hard constraints. |
| `stable_scope_broadening` | release-posture risk | open | Implemented optional features could be over-described as stable before the proof supports that claim. | Keep stable widening gated until `PX50`. |
| `roadmap_truth_drift` | architecture and governance risk | monitor | Older roadmap docs still record pre-post-PL admission framing. | Use the MX control-plane docs as current truth and keep active indexes aligned. |
| `proof_surface_drift` | support and governance risk | open | Code or docs could move without matching proof notes, captures, known limits, or ledgers. | Treat proof-surface updates as part of done, not cleanup. |

## Risk Posture Summary

The current posture is compatible with continued post-PL work, but only if later waves stay disciplined about:

- proof
- support-boundary honesty
- owner boundaries
- promotion control
- stable-scope restraint

The current posture is not compatible with a broad "everything implemented is now stable" narrative.
