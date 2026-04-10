# PX00 Risk Register

This register records the main risks at the start of the post-PL program.

## Risk Register

| Risk Key | Category | Current Status | Meaning | Mitigation Direction |
| --- | --- | --- | --- | --- |
| `support_cross_hardware_gap` | support risk | open | Current proof comes from one capable validation machine, so support posture is stronger on the known machine than across the broader hardware field. | Treat broader proof as an early-wave requirement before stable widening. |
| `support_multi_monitor_bounded` | support risk | open | Plasma currently supports the shared one-window virtual-desktop baseline only, so broader monitor-topology expectations would overclaim current behavior. | Keep the current multi-monitor claim bounded until later product or host work proves anything broader. |
| `support_environment_safety_bounded` | support risk | open | Plasma has real preview-safe, long-run-stable, and descriptive dark-room posture, but broader safety-mode vocabulary is not yet modeled in runtime. | Keep richer safety terms explicit non-claims until later work lands real behavior and proof. |
| `proof_transition_bounded` | proof risk | open | Transition proof is real and now names the current direct classes, curated bridge cycles, and hard-cut or fallback grammar, but it is still limited to the curated implemented subset rather than the full theoretical matrix. | Keep transition claims bounded, use the explicit compatibility matrix, and require proof updates for any further coverage expansion. |
| `proof_dimensional_bounded` | proof risk | open | Only the bounded dimensional subset is backed by current repo reality, and fresh reruns remain locally blocked in this checkout. | Keep dimensional claims narrow, subset-based, and proof-backed until broader modes are implemented and rerun honestly. |
| `proof_output_treatment_subset_bounded` | proof risk | open | `PX30` makes bounded contour, banded, and treatment subsets real, but the new surface is still narrower than the full taxonomy and fresh reruns remain locally blocked in this checkout. | Keep PX30 claims subset-based, keep experimental posture explicit, and require proof updates before any stable widening. |
| `proof_glyph_field_i_subset_bounded` | proof risk | open | `PX31` makes bounded glyph and field-families-I subsets real, but those surfaces are still narrower than the later-wave taxonomy and fresh reruns remain locally blocked in this checkout. | Keep PX31 claims subset-based, keep experimental posture explicit, and require proof updates before any stable widening. |
| `proof_field_ii_dimensional_subset_bounded` | proof risk | open | `PX32` makes bounded field-families-II and dimensional subsets real, but those surfaces remain narrower than the later-wave taxonomy and fresh reruns remain locally blocked in this checkout. | Keep PX32 claims subset-based, keep experimental posture explicit, and require proof updates before any stable widening. |
| `proof_benchlab_forcing_bounded` | proof risk | open | BenchLab forcing is intentionally bounded and should not be described as a full unrestricted debugger. | Keep forcing support-facing and update proof surfaces whenever the forcing subset changes. |
| `proof_numeric_sla_gap` | proof risk | open | The repo has bounded qualitative envelopes, not numeric performance SLAs. | Avoid numeric promises until a later measurement pass exists. |
| `authoring_registry_partial` | architecture and governance risk | open | `PX20` makes sets, journeys, and pack provenance authorable, but the built-in preset and theme descriptor inventory is still not fully migrated into the same authored substrate. | Keep the partial boundary explicit and treat fuller migration as later-wave work. |
| `lab_shell_bounded` | support and tooling risk | open | The PX40 Plasma Lab shell is now a stronger CLI-first authoring and ops toolchain, but it could still be over-described as a full editor or suite-grade workstation if later docs get sloppy. | Keep Lab claims narrow and tie them to the actual command surface. |
| `capture_diff_bounded` | support and tooling risk | open | PX40 adds semantic BenchLab text-capture comparison, but that surface could be over-described as universal deterministic replay or pixel-perfect diffing if later docs get sloppy. | Keep capture-compare claims explicitly text-based, semantic, and bounded to the current evidence surface. |
| `integration_surface_report_first` | support and tooling risk | open | PX41 adds local projection, SDK/reference, control, and curation reports, but those surfaces could be over-described as live `suite`, `anthology`, or remote-automation behavior if later docs get sloppy. | Keep integration claims metadata-first, report-first, and product-local until later live-consumer proof exists. |
| `curation_surface_local_only` | shared-promotion and product risk | open | PX41 adds curated collections and a provenance index, but those surfaces could be over-described as a shared community platform or shared provenance law if later docs get sloppy. | Keep curation and provenance claims local, bounded, and non-platform until later shared need is proved. |
| `selection_foundation_bounded` | support and product risk | open | Weighted set fallback is now real, but richer anti-repeat memory, similarity logic, and safety-aware reasoning are still absent. | Keep selection-intelligence claims within the current weighted authored foundation until later waves land more. |
| `glyph_field_scope_creep` | architecture and product risk | open | Later-wave enthusiasm could treat the first glyph and field-family-I tranche as permission to add broader glyph alphabets, field-families II, or dimensional work too early. | Keep PX31 explicitly bounded to the admitted glyph subset and field-families-I slice, and defer broader breadth to later waves. |
| `field_dimensional_scope_creep` | architecture and product risk | open | Later-wave enthusiasm could treat the first bounded field-families-II and dimensional tranche as permission to add billboard volume, scenic rendering, or Wave 4 ecosystem work too early. | Keep PX32 explicitly bounded to the admitted field-family-II and dimensional subset, and defer broader breadth to later waves. |
| `governance_scope_drift` | architecture and governance risk | open | Later work could try to use post-PL momentum to smuggle in runtime or platform changes outside its wave. | Enforce queue scope, wave gates, and write-surface discipline. |
| `shared_promotion_too_early` | shared-promotion risk | open | Plasma may discover reusable ideas before reuse is proven. `SY20` already kept the richer Wave 2 authoring substrate local, and later waves still need to preserve that discipline. | Apply the promotion rule strictly and keep richer authoring and provenance seams local until reuse is proven. |
| `product_identity_drift` | product-identity risk | open | Plasma could drift toward a generic suite redesign or scenic showcase identity. | Preserve `Plasma Classic`, lower-band-first posture, and field-derived identity as hard constraints. |
| `stable_scope_broadening` | release-posture risk | open | Implemented optional features could be over-described as stable before the proof supports that claim. | Keep stable widening gated until `PX50`. |
| `roadmap_truth_drift` | architecture and governance risk | monitor | Older roadmap docs still record pre-post-PL admission framing. | Use the MX control-plane docs as current truth and keep active indexes aligned. |
| `proof_surface_drift` | support and governance risk | open | Code or docs could move without matching proof notes, captures, known limits, or ledgers. | Treat proof-surface updates as part of done, not cleanup. |

## PX50 Recut Disposition

The current `PX50` recut treats the risk register as follows:

- accepted into the current narrow ship posture:
  - `lab_shell_bounded`
  - `capture_diff_bounded`
  - `integration_surface_report_first`
  - `curation_surface_local_only`
- carried forward as the strongest remaining blockers or caveats:
  - `support_cross_hardware_gap`
  - `proof_transition_bounded`
  - `proof_dimensional_bounded`
  - `proof_output_treatment_subset_bounded`
  - `proof_glyph_field_i_subset_bounded`
  - `proof_field_ii_dimensional_subset_bounded`
  - `proof_numeric_sla_gap`
  - `authoring_registry_partial`
- intentionally held out of the stable runtime cut:
  - `selection_foundation_bounded`
  - `field_dimensional_scope_creep`
  - `stable_scope_broadening`

## Risk Posture Summary

The current posture is compatible with continued post-PL work, but only if later waves stay disciplined about:

- proof
- support-boundary honesty
- owner boundaries
- promotion control
- stable-scope restraint

The current posture is not compatible with a broad "everything implemented is now stable" narrative.
