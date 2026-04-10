# Plasma Docs

This directory holds product-local Plasma planning and doctrine documents.
They live with the product so later `PL` work can extend them without rewriting shared platform law.

## Post-PL Control Docs

- [PX00 Post-PL Program](./px00-post-pl-program.md): defines Plasma's current post-PL posture, role, and early-wave priorities
- [PX00 Feature Ledger](./px00-feature-ledger.md): records the current stable, experimental, deferred, and caveat posture from the repo as it exists now
- [PX00 Risk Register](./px00-risk-register.md): records the current support, proof, governance, and scope risks that later `PX` work must manage explicitly

## Post-PL Wave Docs

- [PX10 Support Hardening](./px10-support-hardening.md): records the first post-PL Plasma support-hardening tranche over hardware proof, soak posture, multi-monitor baseline, and environment safety
- [PX11 Transition Coverage](./px11-transition-coverage.md): records the bounded transition-coverage expansion and first promotion review
- [PX11 Transition Compatibility Matrix](./px11-transition-compatibility-matrix.md): records the named direct, bridge, fallback, and unsupported transition classes currently admitted
- [PX11 Promotion Review 1](./px11-promotion-review-1.md): records the evidence-backed decision to keep the current broader transition-capable surface experimental
- [PX20 Data And Authoring Substrate](./px20-data-and-authoring-substrate.md): records the first Wave 2 Plasma-local data-first and authoring-first substrate tranche
- [PX20 Pack And Provenance](./px20-pack-and-provenance.md): records the Plasma-local pack provenance sidecar and compatibility boundary
- [PX20 Authorable Sets And Journeys](./px20-authorable-sets-and-journeys.md): records the first checked-in authored set and journey files
- [PX20 Plasma Lab Shell](./px20-plasma-lab-shell.md): records the first bounded file-first Plasma Lab shell
- [PX20 Validation And Compare](./px20-validation-and-compare.md): records the current validator, comparer, and degrade-report substrate
- [PX20 Selection Intelligence](./px20-selection-intelligence.md): records the current selection-foundation boundary and weighted authored fallback semantics
- [PX30 Contour And Banded](./px30-contour-and-banded.md): records the first bounded non-raster output-family expansion for contour and banded studies
- [PX30 Treatment Families](./px30-treatment-families.md): records the bounded treatment-family subset now implemented
- [PX30 Stable Vs Experimental Notes](./px30-stable-vs-experimental-notes.md): records the explicit experimental-first posture for the new PX30 surfaces

## PL00-PL14 Program Record

- [PL00 Constitution](./pl00-constitution.md): freezes Plasma identity, boundaries, lower-band truth, migration obligations, and future commitments after `SX09`
- [PL01 Taxonomy Freeze](./pl01-taxonomy.md): freezes Plasma ontology, vocabulary, and canonical taxonomy while separating current support from target terms
- [PL01 Schema Direction](./pl01-schema.md): freezes the Plasma-local logical schema direction without claiming current parser/runtime support for the full model
- [PL02 Plasma Classic](./pl02-plasma-classic.md): freezes the current real Plasma saver as the preserved `Plasma Classic` slice inside future Plasma
- [PL02 Classic Migration Map](./pl02-classic-migration-map.md): records preserved keys, aliases, defaults, pack compatibility, and later-phase invariants for the classic surface
- [PL03 Engine Core](./pl03-engine-core.md): establishes the internal `authored content -> compiled plan -> execution state` shape while preserving current Plasma Classic behavior
- [PL04 Output Framework](./pl04-output-framework.md): establishes explicit output, fixed treatment-slot, and presentation stages while keeping the classic path on raster plus flat presentation
- [PL05 Universal + Compat](./pl05-universal-compat.md): hardens and proves the preserved classic/default Plasma path on the truthful `gdi` and `gl11` lower bands
- [PL06 Content System](./pl06-content-system.md): makes presets, themes, packs, and product-local sets real registry-backed Plasma content instead of scattered assumptions
- [PL06 Selection And Sets](./pl06-selection-and-sets.md): defines the persisted selection state, favorites, exclusions, and stable/experimental filtering used by the new content layer
- [PL07 Advanced Lane](./pl07-advanced-lane.md): makes `advanced -> gl21` a bounded richer execution lane without changing Plasma's lower-band default center of gravity
- [PL08 Modern Lane](./pl08-modern-lane.md): makes `modern -> gl33` a bounded refined execution lane layered on top of the advanced path without changing Plasma's lower-band or GL21 reference posture
- [PL09 Premium Lane](./pl09-premium-lane.md): makes `premium -> gl46` a bounded showcase lane layered on top of the modern path and lands the first real bounded dimensional presentation mode without changing Plasma's lower-band default center of gravity
- [PL10 Transition Engine](./pl10-transition-engine.md): makes bounded real theme morphs, preset morphs, bridge morphs, fallback policy, and runtime transition state real without changing Plasma's default stable baseline
- [PL10 Journeys And Compatibility](./pl10-journeys-and-compatibility.md): freezes the implemented compatibility classes, bridge classes, companion set rules, journey model, and seed continuity rules used by the transition engine
- [PL11 Settings Surfaces](./pl11-settings-surfaces.md): makes the layered Basic, Advanced, and Author/Lab Plasma settings surfaces real through a product-local dialog surface selector, availability rules, and runtime settings resolution
- [PL11 Settings Catalog](./pl11-settings-catalog.md): freezes the implemented Plasma settings descriptor model, category rules, persistence scopes, and current shipped catalog subset
- [PL12 BenchLab Integration](./pl12-benchlab.md): makes Plasma's product-local BenchLab reporting, bounded forcing, and requested-versus-resolved-versus-degraded truth export real without changing the normal saver path
- [PL12 BenchLab Fields And Forcing](./pl12-benchlab-fields-and-forcing.md): freezes the implemented Plasma-specific BenchLab field catalog, forcing subset, clamp rules, and reporting semantics
- [PL13 Validation Matrix](./pl13-validation-matrix.md): records the current Plasma proof matrix across lower lanes, richer lanes, transitions, settings, and BenchLab surfaces with explicit evidence-backed status labels
- [PL13 Performance Envelopes](./pl13-performance-envelopes.md): records the bounded startup, runtime, transition, and soak envelopes that are honestly supportable for the implemented subset
- [PL13 Known Limits](./pl13-known-limits.md): records the current unsupported, partially validated, and blocked areas that PL14 must treat as factual constraints
- [PL14 Release Cut](./pl14-release-cut.md): defines the stable Core, experimental, and deferred Plasma ship scope for the current repo state
- [PL14 Stable Vs Experimental](./pl14-stable-vs-experimental.md): defines the actual stable-versus-experimental split and how the current repo enforces it
- [PL14 Ship Readiness](./pl14-ship-readiness.md): records the explicit PL14 flagship ship decision and the evidence and caveats behind it
- [PL14 Release Notes Draft](./pl14-release-notes-draft.md): provides a truthful release-note-grade summary of what this Plasma cut does and does not ship

## Intended Follow-On

`PL00` through `PL14` are complete.
Post-PL hardening and expansion now routes through the `PX00` control docs and the master `MX` control plane.
No later post-PL implementation phase is claimed as implemented until it lands explicitly.
