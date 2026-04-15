# Plasma Smoke

This directory keeps lightweight product validation for Plasma.
The current smoke check covers module validity, default config/clamp behavior, Plasma Classic default and routing invariants, the preserved classic preset/theme inventory, alias resolution for `ember_lava`, runtime-plan compilation for the preserved classic surface, explicit `gdi` and `gl11` lower-band validation for the classic/default path, the explicit output/treatment/presentation framework for the classic raster path, the PL06 content registry for built-in presets/themes plus the shipped `lava_remix` pack, product-local preset/theme sets, favorites and exclusions selection behavior, stable-versus-experimental filtering defaults, content import/export metadata round-tripping, randomization compatibility with the selection layer, the bounded PL07 `gl21` advanced-lane plan and session path, explicit advanced degrade behavior back to the lower-band baseline, the bounded PL08 `gl33` modern-lane plan and session path, explicit modern degrade behavior back to `gl21` and then the lower-band baseline, the bounded PL09 `gl46` premium-lane plan and session path, explicit premium degrade behavior back to `gl33`, then lower lanes, the current first-class presentation subset through `heightfield` and `ribbon` with compatibility-only coverage still preserved for `curtain`, `contour_extrusion`, and `bounded_surface`, the PL10 transition-config round-trip surface, lower-band and richer-lane transition plan binding, real theme morphs, real direct preset morphs, bounded bridge morphs, explicit fallback and reject behavior for unsupported pairs, journey progression across curated sets, honest lower-band and richer-lane transition gating, the PL11 settings catalog and surface descriptors, settings-resolution defaults, availability gating for experimental content, hidden favorites compatibility, deterministic seed values, journey-selection gating, and the PL12 product-local BenchLab forcing parser, config bridge, plan forcing, snapshot export, overlay/report builders, clamp reporting, and requested-versus-resolved-versus-degraded reporting for both premium-capable and degraded proof paths.

PL13 adds proof-catalog consistency checks, bounded performance-envelope and known-limit assertions, repeated lower-band create/destroy soak coverage, and repeated journey-driven transition soak coverage that uses the real authored dwell values and bounded bridge-settle ticks.

PX11 extends the bounded transition proof surface with explicit hard-cut coverage, curated warm and cool bridge-cycle journey coverage, and BenchLab assertions for requested-versus-resolved-versus-fallback transition truth.

U02 extends the smoke surface so generator, output, treatment, and presentation selections are proven to round-trip through product settings, drive the compiled plan directly, degrade honestly on lower lanes, and surface requested-versus-resolved BenchLab truth for advanced-treatment and premium-presentation clamps.

U03 adds a smaller but important UI-truth guardrail by asserting the exact settings-surface mapping for the recut Basic, Advanced, and Author/Lab layers while the proof note records the new exact-surface capture and hidden-state behavior.

U04 adds the first explicit visual-grammar truth checkpoint: shared admitted-subset helpers for output, treatment, and presentation, stronger deterministic render-signature assertions for non-baseline output and treatment choices, explicit premium-presentation signature separation, and focused BenchLab degrade captures for requested-versus-resolved presentation truth.

U05 extends the smoke surface so the destructive simplification pass stays aligned with the actual shipped control surface, the admitted first-class grammar subset, the narrowed stable preset and theme pools, and the preserved compatibility-only escape hatches.

U07 adds deterministic render-signature and pixel-difference checks so generator, detail, speed, resolution, smoothing, output family, treatment slots, and premium presentation changes are all proven to move the rendered result. It also adds the `plasma_lab.py preset-audit` report for compiled preset-signature and palette crowding.

U08 records the visual QA bar that now governs whether a surface remains stable, stays experimental, or should be hidden.

PX20 adds a bounded authored-substrate and Lab-shell proof surface through `plasma_lab.py`, authored file validation, weighted-selection smoke assertions, and explicit proof notes for content validation, authoring substrate, the Lab shell, and the current selection-foundation boundary.

PX30 adds bounded contour and banded output assertions, a bounded experimental preset pool, explicit treatment-subset assertions, and BenchLab text-surface assertions for experimental output and treatment truth.

PX31 adds bounded glyph assertions for `ascii_glyph` and `matrix_glyph`, bounded field-family-I assertions for chemical, lattice, and caustic studies, and BenchLab text-surface assertions for experimental glyph and generator-family truth.

PX40 adds a stronger CLI-first authoring and ops proof surface through expanded `plasma_lab.py` command coverage for authored inventory, compatibility, migration reporting, capture-backed degrade inspection, and semantic BenchLab text-capture comparison.

PX41 adds a bounded metadata-first integration and curation proof surface through local projection, SDK/reference, control-profile, curated-collection, and provenance-index reports.

PX50 adds a recut-proof note that records exactly which proof notes, captures, and read-only report commands were used to make the final stable, experimental, and deferred classification.

U00 now adds a diagnostic note that records the inspected repo surfaces and the
grounding commands used to refresh the corrective-program baseline against the
current snapshot.

U01 now adds an explicit runtime proof note that records the current MSVC
`v141_xp` rebuild, the manual smoke rebuild, and the canonical-versus-legacy
lookup invariants that keep Classic preserved as content compatibility rather
than as a runtime regime.

`u00-diagnostic-notes.md`, `u01-runtime-proof.md`, `u02-settings-proof.md`, `u03-ui-proof.md`, `u04-visual-grammar-proof.md`, `u05-simplification-proof.md`, `pl05-lower-band-proof.md`, `pl06-content-proof.md`, `pl07-advanced-proof.md`, `pl08-modern-proof.md`, `pl09-premium-proof.md`, `pl10-transition-proof.md`, `pl11-settings-proof.md`, `pl12-benchlab-proof.md`, `pl13-soak-and-proof.md`, `pl13-transition-validation.md`, `px10-hardware-proof.md`, `px10-soak-proof.md`, `px11-transition-proof.md`, `px11-promotion-review-proof.md`, `px20-content-validation-proof.md`, `px20-authoring-substrate-proof.md`, `px20-lab-shell-proof.md`, `px20-selection-foundation-proof.md`, `px30-output-proof.md`, `px30-treatment-proof.md`, `px31-glyph-proof.md`, `px31-field-families-i-proof.md`, `px32-field-families-ii-proof.md`, `px32-dimensional-proof.md`, `px40-authoring-proof.md`, `px40-ops-tools-proof.md`, `px41-integration-proof.md`, `px41-curation-proof.md`, and `px50-recut-proof.md` record the exact proof evidence, rebuild commands where available, and current rerun blockers for those phases.
