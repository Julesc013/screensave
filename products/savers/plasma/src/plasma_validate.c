#include "plasma_internal.h"

static const plasma_validation_matrix_entry g_plasma_validation_matrix[] = {
    {
        "classic_default",
        "gdi",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "validation/captures/pl13/benchlab-plasma-gdi.txt",
        "Forced GDI report proves the preserved default classic raster-plus-flat path remains truthful on the universal floor."
    },
    {
        "classic_default",
        "gl11",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "validation/captures/pl13/benchlab-plasma-gl11.txt",
        "Forced GL11 report proves the preserved default classic path remains truthful on the compat lane."
    },
    {
        "content_registry",
        "product",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "products/savers/plasma/tests/pl06-content-proof.md",
        "Registry, set, favorites, exclusions, and pack preservation claims are backed by smoke coverage and recorded proof."
    },
    {
        "advanced_lane",
        "gl21",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "validation/captures/pl13/benchlab-plasma-gl21.txt",
        "Forced GL21 report proves the bounded advanced lane is still real and separately routable."
    },
    {
        "modern_lane",
        "gl33",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "validation/captures/pl13/benchlab-plasma-gl33.txt",
        "Forced GL33 report proves the bounded modern lane is still real and separately routable."
    },
    {
        "premium_heightfield",
        "gl46",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt",
        "Forced GL46 heightfield report proves the bounded premium lane and current dimensional presentation subset."
    },
    {
        "premium_request_degrade",
        "auto",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "validation/captures/pl13/benchlab-plasma-degrade-heightfield-auto.txt",
        "Auto plus premium-only presentation forcing proves requested-versus-resolved-versus-degraded reporting on the lower-band default path."
    },
    {
        "transition_subset",
        "gl11",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "products/savers/plasma/tests/px11-transition-proof.md",
        "PX11 keeps the lower-band transition subset bounded but names the supported direct and fallback classes explicitly and records the current capture anchor honestly."
    },
    {
        "settings_subset",
        "product",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "products/savers/plasma/tests/pl11-settings-proof.md",
        "Layered settings surfaces and current gating rules are backed by smoke coverage and a recorded proof note."
    },
    {
        "benchlab_surface",
        "product",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "products/savers/plasma/tests/pl12-benchlab-proof.md",
        "BenchLab field export, bounded forcing, and clamp reporting are backed by smoke coverage and PL12 captures."
    },
    {
        "hardware_matrix",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/docs/px10-hardware-proof.md",
        "PX10 turns the current single-machine lane ladder, shared host carry-forward evidence, and blocked breadth into an explicit Plasma-local hardware matrix."
    },
    {
        "performance_soak_posture",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/docs/px10-performance-and-soak.md",
        "PX10 splits measured run counts, bounded qualitative envelopes, and unknown numeric claims explicitly without creating hard performance SLAs."
    },
    {
        "multi_monitor_baseline",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/docs/px10-multi-monitor.md",
        "PX10 freezes the truthful one-session virtual-desktop baseline for Plasma and leaves independent per-monitor behavior explicitly unsupported."
    },
    {
        "environment_safety_surface",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/docs/px10-environment-safety.md",
        "PX10 ties preview-safe, long-run-stable, dark-room descriptive content, and current clamp or fallback behavior back to actual product-local behavior."
    },
    {
        "transition_soak",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px11-transition-proof.md",
        "PX11 broadens the bounded transition soak story to include named warm and cool bridge-cycle paths in smoke while keeping exhaustive pairwise long-run proof explicitly out of scope."
    },
    {
        "pack_provenance_surface",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px20-content-validation-proof.md",
        "PX20 adds a Plasma-local provenance sidecar for lava_remix and validates that it stays aligned with the existing shared pack.ini shell."
    },
    {
        "authoring_substrate",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px20-authoring-substrate-proof.md",
        "PX20 makes authored preset-set, theme-set, and journey files real on disk and compares them against the compiled Plasma catalog."
    },
    {
        "lab_shell_surface",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px40-authoring-proof.md",
        "PX40 grows the first file-first Plasma Lab shell into a stronger CLI-first authoring surface for validate, inventory, authored compare, compatibility, and migration reporting without promising a full editor."
    },
    {
        "ops_toolchain_surface",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px40-ops-tools-proof.md",
        "PX40 adds bounded capture-backed degrade inspection and semantic BenchLab text-capture diff support without claiming broad rendered determinism or a platform-wide automation lab."
    },
    {
        "selection_foundation",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px20-selection-foundation-proof.md",
        "PX20 makes authored set weights materially real in fallback selection and freezes the current selection-foundation boundary honestly."
    },
    {
        "experimental_pool",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px30-output-proof.md",
        "PX30 and PX31 grow a bounded built-in experimental preset pool for contour, banded, treatment, glyph, and field-family studies without changing the default stable-only pool."
    },
    {
        "contour_output_subset",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px30-output-proof.md",
        "PX30 makes contour-only and contour-bands real on the bounded implemented subset while keeping unsupported combinations explicit."
    },
    {
        "banded_output_subset",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px30-output-proof.md",
        "PX30 makes posterized banded output a real first-class grammar on the bounded implemented subset instead of leaving banding as taxonomy only."
    },
    {
        "treatment_family_subset",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px30-treatment-proof.md",
        "PX30 makes a bounded treatment subset real with explicit slot placement, degrade policy, and experimental posture."
    },
    {
        "glyph_output_subset",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px31-glyph-proof.md",
        "PX31 makes a bounded glyph output subset real with explicit ASCII and Matrix modes, output-family truth, and no silent raster fallback."
    },
    {
        "field_family_i_subset",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px31-field-families-i-proof.md",
        "PX31 makes bounded chemical, lattice, and caustic field-family studies real on the existing Plasma engine seams without widening into later field-family or dimensional work."
    },
    {
        "field_family_ii_subset",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px32-field-families-ii-proof.md",
        "PX32 makes bounded aurora, substrate, and arc studies real on the existing Plasma engine seams without widening into scenic or later-wave ecosystem work."
    },
    {
        "dimensional_presentation_subset",
        "gl46",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/px32-dimensional-proof.md",
        "PX32 broadens the premium dimensional subset beyond heightfield to bounded curtain, ribbon, contour extrusion, and bounded surface modes while keeping unsupported breadth explicit."
    },
    {
        "cross_driver_ladder",
        "all",
        PLASMA_VALIDATION_STATUS_BLOCKED,
        "products/savers/plasma/docs/pl13-known-limits.md",
        "The current proof set comes from one capable validation machine rather than a wider cross-driver or reduced-capability lab sweep."
    }
};

static const plasma_performance_envelope_entry g_plasma_performance_envelopes[] = {
    {
        "classic_gdi",
        "gdi",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "bounded_qualitative",
        "Session creation, plan compilation, and BenchLab report-mode startup completed successfully on the preserved classic path.",
        "The validated runtime surface is the classic raster-plus-flat path with no richer-lane treatment or presentation uplift.",
        "No transition cost is claimed for the baseline GDI proof path beyond the bounded classic-default surface.",
        "Repeated session creation/destruction and bounded smoke stepping complete without failure.",
        "validation/captures/pl13/benchlab-plasma-gdi.txt"
    },
    {
        "classic_gl11",
        "gl11",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "bounded_qualitative",
        "Session creation, plan compilation, and BenchLab report-mode startup completed successfully on the compat lane.",
        "The validated runtime surface is the classic default identity plus the bounded compat acceleration path.",
        "Journey-focused transition proof exists on the lower-band compat path without requiring richer lanes.",
        "Repeated session creation/destruction and bounded transition stepping complete without failure.",
        "validation/captures/pl13/benchlab-plasma-gl11.txt"
    },
    {
        "advanced_gl21",
        "gl21",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "bounded_qualitative",
        "Compiled and selected successfully through BenchLab report mode on the advanced lane.",
        "Validated only for the bounded PL07 advanced component subset, not for broader future advanced aspirations.",
        "Transition behavior is validated through the shared engine subset rather than a GL21-only transition expansion.",
        "No separate long-duration GL21 soak claim is made beyond repeated smoke-backed session creation.",
        "validation/captures/pl13/benchlab-plasma-gl21.txt"
    },
    {
        "modern_gl33",
        "gl33",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "bounded_qualitative",
        "Compiled and selected successfully through BenchLab report mode on the modern lane.",
        "Validated only for the bounded PL08 refined component subset and flat presentation continuity.",
        "Transition behavior remains the shared bounded subset rather than a GL33-specific choreography layer.",
        "No separate long-duration GL33 soak claim is made beyond repeated smoke-backed session creation.",
        "validation/captures/pl13/benchlab-plasma-gl33.txt"
    },
    {
        "premium_gl46_heightfield",
        "gl46",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "bounded_qualitative",
        "Compiled and selected successfully through BenchLab report mode on the premium lane.",
        "Validated only for the bounded premium subset plus the current heightfield presentation path.",
        "Premium transition cost remains bounded by the shared transition engine subset; no premium-only transition envelope is claimed.",
        "No unrestricted heavy-feedback or scenic premium soak claim is made.",
        "validation/captures/pl13/benchlab-plasma-gl46-heightfield.txt"
    },
    {
        "transition_subset",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "bounded_qualitative",
        "Transition-capable session compilation and restart behavior are validated for the implemented subset.",
        "Real theme morph, preset morph, bridge, fallback, hard-cut, and journey behavior are exercised only for bounded named classes, curated bridge-cycle sets, and curated journeys.",
        "Requested, resolved, and fallback transition truth is now exported explicitly in BenchLab for the implemented subset only.",
        "Repeated transition stepping is bounded and stable in smoke, but exhaustive pairwise soak coverage is not claimed.",
        "products/savers/plasma/tests/px11-transition-proof.md"
    },
    {
        "lower_band_restart_soak",
        "product",
        PLASMA_VALIDATION_STATUS_VALIDATED,
        "measured",
        "The checked-in smoke harness repeatedly creates and compiles lower-band classic sessions on the preserved default path.",
        "The strongest restart and steady-state support surface remains the stable classic baseline on the lower bands.",
        "No transition-cost claim is attached to this restart envelope.",
        "The current smoke harness uses twelve lower-band restart iterations with twenty-four bounded steps per iteration.",
        "products/savers/plasma/tests/px10-soak-proof.md"
    },
    {
        "preview_safe_runtime",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "bounded_qualitative",
        "Preview-mode session creation uses the same truthful routing posture as the normal product path.",
        "Preview mode reduces field density, motion intensity, and smoothing aggressiveness rather than inventing a separate visual language.",
        "Preview transition behavior remains the bounded current product behavior rather than a separate preview-only transition system.",
        "PX10 formalizes the preview-safe runtime meaning, but does not add a fresh native preview-host rerun in this checkout.",
        "products/savers/plasma/docs/px10-environment-safety.md"
    },
    {
        "contour_and_banded_subset",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "bounded_qualitative",
        "PX30 compiles and renders a bounded contour and banded output subset through the normal product path.",
        "The truthful output subset remains lower-band-safe and uses explicit output-family and output-mode reporting rather than silent raster fallback.",
        "No claim is made for glyph, surface, or exhaustive output-mode coverage in this tranche.",
        "Fresh compiled reruns remain bounded by the current local toolchain blocker in this checkout.",
        "products/savers/plasma/tests/px30-output-proof.md"
    },
    {
        "treatment_subset",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "bounded_qualitative",
        "PX30 compiles and reports a bounded treatment subset across filter, emulation, and accent slots.",
        "The truthful treatment subset is limited to glow-edge, halftone-stipple, emboss-edge, phosphor, crt, and accent-pass plus existing advanced-only blur and overlay behavior.",
        "No broad effect-zoo or exhaustive lane-by-lane treatment guarantee is claimed.",
        "Fresh compiled reruns remain bounded by the current local toolchain blocker in this checkout.",
        "products/savers/plasma/tests/px30-treatment-proof.md"
    },
    {
        "glyph_subset",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "bounded_qualitative",
        "PX31 compiles and renders a bounded glyph subset through the normal product path with explicit ASCII and Matrix output-mode truth.",
        "The truthful glyph subset is preset-driven and experimental-first rather than a broad generic glyph rendering system.",
        "No claim is made for glyph-specific transitions, rich Unicode coverage, or broader output-family breadth beyond the admitted ASCII and Matrix modes.",
        "Fresh compiled reruns remain bounded by the current local toolchain blocker in this checkout.",
        "products/savers/plasma/tests/px31-glyph-proof.md"
    },
    {
        "field_family_i_subset",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "bounded_qualitative",
        "PX31 compiles and steps a bounded subset of new field families through the existing Plasma simulation path.",
        "The truthful subset is limited to chemical or cellular growth, lattice or quasi-crystal, and caustic or marbling studies on the existing engine seams.",
        "No claim is made for broader field-family-II breadth, richer dimensional work, or exhaustive lane sweeps in this tranche.",
        "Fresh compiled reruns remain bounded by the current local toolchain blocker in this checkout.",
        "products/savers/plasma/tests/px31-field-families-i-proof.md"
    },
    {
        "field_family_ii_subset",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "bounded_qualitative",
        "PX32 compiles and steps a bounded subset of later experimental field families through the existing Plasma simulation path.",
        "The truthful subset is limited to aurora or curtain-ribbon studies, substrate or vein-coral studies, and arc or discharge studies on the existing engine seams.",
        "No claim is made for scenic world behavior, broader ecosystem work, or exhaustive lane sweeps in this tranche.",
        "Fresh compiled reruns remain bounded by the current local toolchain blocker in this checkout.",
        "products/savers/plasma/tests/px32-field-families-ii-proof.md"
    },
    {
        "premium_dimensional_subset",
        "gl46",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "bounded_qualitative",
        "PX32 compiles and presents a bounded dimensional subset on the premium lane beyond the original heightfield path.",
        "The truthful dimensional subset is limited to heightfield, curtain, ribbon, contour extrusion, and bounded surface while keeping billboard-volume and scenic breadth out of scope.",
        "Dimensional degrade remains explicit back to flat presentation on lower lanes instead of silent fake premium behavior.",
        "Fresh compiled reruns remain bounded by the current local toolchain blocker in this checkout.",
        "products/savers/plasma/tests/px32-dimensional-proof.md"
    }
};

static const plasma_known_limit_entry g_plasma_known_limits[] = {
    {
        "experimental_pool_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "PX30 and PX31 add a bounded built-in experimental preset pool, but it remains a narrow opt-in study slice rather than a broad experimental catalog.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "dimensional_presentation_subset_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "PX32 broadens the dimensional subset to heightfield, curtain, ribbon, contour extrusion, and bounded surface, but billboard-style volume and broader scenic presentation remain unsupported.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "transition_pair_coverage_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "Transition proof now names the implemented direct classes, curated warm and cool bridge cycles, and explicit hard-cut or fallback grammar, but it still does not cover every theoretical preset or theme pairing.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "benchlab_forcing_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "BenchLab forcing is intentionally bounded to the product-local subset implemented in PL12. It is not a full forcing console for every future Plasma knob.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "cross_hardware_gap",
        PLASMA_VALIDATION_STATUS_BLOCKED,
        "The product-local PX10 matrix now makes the known machine and blocked breadth explicit, but the repo still lacks a second GPU, alternate driver family, or lower-capability denial sweep.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "numeric_perf_sla_gap",
        PLASMA_VALIDATION_STATUS_BLOCKED,
        "PX10 now splits measured run counts, bounded qualitative envelopes, and unknowns explicitly, but the repo still does not claim hard numeric frame-time, startup, or memory SLAs.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "multi_monitor_baseline_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "Current multi-monitor support is the shared one-window virtual-desktop baseline. Independent per-monitor behavior, hero or follower roles, and monitor-aware layout policy remain out of scope.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "environment_safety_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "Current safety claims are limited to preview-safe, long-run-stable posture, descriptive dark-room content choices, and truthful clamp or fallback behavior. OLED, battery, thermal, remote, and safe-mode terms remain unmodeled.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "authored_registry_partial",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "PX20 moves sets, journeys, and pack provenance into real authored files, but the built-in preset and theme descriptor inventory still remains primarily compiled and legacy-INI anchored.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "lab_shell_cli_only",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "PX40 grows the first Plasma Lab shell into a stronger CLI-first authoring and ops toolchain, but it is still not a live visual editor, gallery, or suite-grade authoring workstation.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "capture_diff_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "PX40 adds semantic BenchLab text-capture comparison and capture-backed degrade inspection, but it does not claim raw-text identity or pixel-perfect determinism across every lane or renderer path.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "output_family_subset_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "PX30 and PX31 make bounded contour, banded, and glyph output subsets real, but surface output and broader non-raster coverage remain later-wave work.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "treatment_family_subset_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "PX30 lands a curated treatment subset with explicit slot and degrade policy, but it does not implement every named treatment family or every theoretical combination.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "glyph_output_subset_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "PX31 lands a bounded ASCII and Matrix glyph subset, but it does not implement a broad text renderer, richer glyph alphabets, or every treatment and transition combination.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "field_family_i_subset_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "PX31 lands bounded chemical, lattice, and caustic studies, but it does not implement broader field-family-II breadth, dimensional uplift, or exhaustive lane proof for the new families.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "field_family_ii_subset_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "PX32 lands bounded aurora, substrate, and arc studies, but it does not implement richer ecological or scenic behavior, exhaustive lane proof, or broader later-wave ecosystem work.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "selection_foundation_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "Selection now honors authored set weights for bounded fallback behavior, but richer anti-repeat memory, semantic similarity, and safety-tag reasoning remain later-wave work.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    }
};

static int plasma_validation_has_text(const char *text)
{
    return text != NULL && text[0] != '\0';
}

static int plasma_validation_status_is_valid(plasma_validation_status status)
{
    return
        status >= PLASMA_VALIDATION_STATUS_VALIDATED &&
        status <= PLASMA_VALIDATION_STATUS_BLOCKED;
}

const char *plasma_validation_status_name(plasma_validation_status status)
{
    switch (status) {
    case PLASMA_VALIDATION_STATUS_VALIDATED:
        return "validated";

    case PLASMA_VALIDATION_STATUS_PARTIAL:
        return "partial";

    case PLASMA_VALIDATION_STATUS_DOCUMENTED_ONLY:
        return "documented_only";

    case PLASMA_VALIDATION_STATUS_UNSUPPORTED:
        return "unsupported";

    case PLASMA_VALIDATION_STATUS_BLOCKED:
    default:
        return "blocked";
    }
}

const plasma_validation_matrix_entry *plasma_validation_get_matrix(
    unsigned int *count_out
)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(
            sizeof(g_plasma_validation_matrix) / sizeof(g_plasma_validation_matrix[0])
        );
    }

    return g_plasma_validation_matrix;
}

const plasma_validation_matrix_entry *plasma_validation_find_matrix_entry(
    const char *area_key,
    const char *lane_key
)
{
    unsigned int count;
    unsigned int index;

    count = (unsigned int)(
        sizeof(g_plasma_validation_matrix) / sizeof(g_plasma_validation_matrix[0])
    );
    for (index = 0U; index < count; ++index) {
        if (
            strcmp(g_plasma_validation_matrix[index].area_key, area_key) == 0 &&
            strcmp(g_plasma_validation_matrix[index].lane_key, lane_key) == 0
        ) {
            return &g_plasma_validation_matrix[index];
        }
    }

    return NULL;
}

int plasma_validation_matrix_is_consistent(void)
{
    unsigned int count;
    unsigned int index;
    unsigned int compare_index;

    count = (unsigned int)(
        sizeof(g_plasma_validation_matrix) / sizeof(g_plasma_validation_matrix[0])
    );
    for (index = 0U; index < count; ++index) {
        const plasma_validation_matrix_entry *entry;

        entry = &g_plasma_validation_matrix[index];
        if (
            !plasma_validation_has_text(entry->area_key) ||
            !plasma_validation_has_text(entry->lane_key) ||
            !plasma_validation_status_is_valid(entry->status) ||
            !plasma_validation_has_text(entry->evidence_ref) ||
            !plasma_validation_has_text(entry->notes)
        ) {
            return 0;
        }

        for (compare_index = index + 1U; compare_index < count; ++compare_index) {
            if (
                strcmp(entry->area_key, g_plasma_validation_matrix[compare_index].area_key) == 0 &&
                strcmp(entry->lane_key, g_plasma_validation_matrix[compare_index].lane_key) == 0
            ) {
                return 0;
            }
        }
    }

    return 1;
}

const plasma_performance_envelope_entry *plasma_validation_get_performance_envelopes(
    unsigned int *count_out
)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(
            sizeof(g_plasma_performance_envelopes) /
            sizeof(g_plasma_performance_envelopes[0])
        );
    }

    return g_plasma_performance_envelopes;
}

const plasma_performance_envelope_entry *plasma_validation_find_performance_envelope(
    const char *envelope_key
)
{
    unsigned int count;
    unsigned int index;

    count = (unsigned int)(
        sizeof(g_plasma_performance_envelopes) /
        sizeof(g_plasma_performance_envelopes[0])
    );
    for (index = 0U; index < count; ++index) {
        if (strcmp(g_plasma_performance_envelopes[index].envelope_key, envelope_key) == 0) {
            return &g_plasma_performance_envelopes[index];
        }
    }

    return NULL;
}

int plasma_validation_performance_envelopes_are_consistent(void)
{
    unsigned int count;
    unsigned int index;
    unsigned int compare_index;

    count = (unsigned int)(
        sizeof(g_plasma_performance_envelopes) /
        sizeof(g_plasma_performance_envelopes[0])
    );
    for (index = 0U; index < count; ++index) {
        const plasma_performance_envelope_entry *entry;

        entry = &g_plasma_performance_envelopes[index];
        if (
            !plasma_validation_has_text(entry->envelope_key) ||
            !plasma_validation_has_text(entry->lane_key) ||
            !plasma_validation_status_is_valid(entry->status) ||
            !plasma_validation_has_text(entry->measurement_mode) ||
            !plasma_validation_has_text(entry->startup_notes) ||
            !plasma_validation_has_text(entry->runtime_notes) ||
            !plasma_validation_has_text(entry->transition_notes) ||
            !plasma_validation_has_text(entry->soak_notes) ||
            !plasma_validation_has_text(entry->evidence_ref)
        ) {
            return 0;
        }

        for (compare_index = index + 1U; compare_index < count; ++compare_index) {
            if (
                strcmp(
                    entry->envelope_key,
                    g_plasma_performance_envelopes[compare_index].envelope_key
                ) == 0
            ) {
                return 0;
            }
        }
    }

    return 1;
}

const plasma_known_limit_entry *plasma_validation_get_known_limits(
    unsigned int *count_out
)
{
    if (count_out != NULL) {
        *count_out = (unsigned int)(
            sizeof(g_plasma_known_limits) / sizeof(g_plasma_known_limits[0])
        );
    }

    return g_plasma_known_limits;
}

const plasma_known_limit_entry *plasma_validation_find_known_limit(
    const char *limit_key
)
{
    unsigned int count;
    unsigned int index;

    count = (unsigned int)(
        sizeof(g_plasma_known_limits) / sizeof(g_plasma_known_limits[0])
    );
    for (index = 0U; index < count; ++index) {
        if (strcmp(g_plasma_known_limits[index].limit_key, limit_key) == 0) {
            return &g_plasma_known_limits[index];
        }
    }

    return NULL;
}

int plasma_validation_known_limits_are_consistent(void)
{
    unsigned int count;
    unsigned int index;
    unsigned int compare_index;

    count = (unsigned int)(
        sizeof(g_plasma_known_limits) / sizeof(g_plasma_known_limits[0])
    );
    for (index = 0U; index < count; ++index) {
        const plasma_known_limit_entry *entry;

        entry = &g_plasma_known_limits[index];
        if (
            !plasma_validation_has_text(entry->limit_key) ||
            !plasma_validation_status_is_valid(entry->status) ||
            !plasma_validation_has_text(entry->notes) ||
            !plasma_validation_has_text(entry->evidence_ref)
        ) {
            return 0;
        }

        for (compare_index = index + 1U; compare_index < count; ++compare_index) {
            if (
                strcmp(entry->limit_key, g_plasma_known_limits[compare_index].limit_key) == 0
            ) {
                return 0;
            }
        }
    }

    return 1;
}

screensave_renderer_kind plasma_resolve_renderer_kind(
    const screensave_saver_environment *environment
)
{
    screensave_renderer_info renderer_info;

    if (environment == NULL || environment->renderer == NULL) {
        return SCREENSAVE_RENDERER_KIND_GDI;
    }

    screensave_renderer_get_info(environment->renderer, &renderer_info);
    if (
        renderer_info.active_kind == SCREENSAVE_RENDERER_KIND_GDI ||
        renderer_info.active_kind == SCREENSAVE_RENDERER_KIND_GL11 ||
        renderer_info.active_kind == SCREENSAVE_RENDERER_KIND_GL21 ||
        renderer_info.active_kind == SCREENSAVE_RENDERER_KIND_GL33 ||
        renderer_info.active_kind == SCREENSAVE_RENDERER_KIND_GL46
    ) {
        return renderer_info.active_kind;
    }

    return SCREENSAVE_RENDERER_KIND_UNKNOWN;
}

screensave_renderer_kind plasma_resolve_requested_renderer_kind(
    const screensave_saver_environment *environment
)
{
    screensave_renderer_info renderer_info;

    if (environment == NULL || environment->renderer == NULL) {
        return SCREENSAVE_RENDERER_KIND_UNKNOWN;
    }

    screensave_renderer_get_info(environment->renderer, &renderer_info);
    return renderer_info.requested_kind;
}

int plasma_is_lower_band_kind(screensave_renderer_kind renderer_kind)
{
    return
        renderer_kind == SCREENSAVE_RENDERER_KIND_GDI ||
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL11;
}

int plasma_is_advanced_runtime_kind(screensave_renderer_kind renderer_kind)
{
    return
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL21 ||
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL33 ||
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL46;
}

int plasma_is_modern_runtime_kind(screensave_renderer_kind renderer_kind)
{
    return
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL33 ||
        renderer_kind == SCREENSAVE_RENDERER_KIND_GL46;
}

int plasma_is_premium_runtime_kind(screensave_renderer_kind renderer_kind)
{
    return renderer_kind == SCREENSAVE_RENDERER_KIND_GL46;
}

int plasma_plan_is_lower_band_baseline(const struct plasma_plan_tag *plan)
{
    if (
        plan == NULL ||
        plan->advanced_enabled ||
        plan->advanced_components != 0UL ||
        plan->modern_enabled ||
        plan->modern_components != 0UL ||
        plan->premium_enabled ||
        plan->premium_components != 0UL ||
        plan->transition_requested ||
        plan->transition_enabled ||
        plan->output_family != PLASMA_OUTPUT_FAMILY_RASTER ||
        plan->output_mode != PLASMA_OUTPUT_MODE_NATIVE_RASTER ||
        plan->sampling_treatment != PLASMA_SAMPLING_TREATMENT_NONE ||
        plan->filter_treatment != PLASMA_FILTER_TREATMENT_NONE ||
        plan->emulation_treatment != PLASMA_EMULATION_TREATMENT_NONE ||
        plan->accent_treatment != PLASMA_ACCENT_TREATMENT_NONE ||
        plan->presentation_mode != PLASMA_PRESENTATION_MODE_FLAT
    ) {
        return 0;
    }

    if (
        plan->minimum_kind != SCREENSAVE_RENDERER_KIND_GDI ||
        plan->preferred_kind != SCREENSAVE_RENDERER_KIND_GL11 ||
        plan->quality_class != SCREENSAVE_CAPABILITY_QUALITY_SAFE
    ) {
        return 0;
    }

    return 1;
}

int plasma_plan_validate_for_renderer_kind(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module,
    screensave_renderer_kind renderer_kind
)
{
    if (
        module == NULL ||
        !plasma_plan_validate(plan, module) ||
        !screensave_saver_supports_renderer_kind(module, renderer_kind)
    ) {
        return 0;
    }

    if (plasma_is_lower_band_kind(renderer_kind)) {
        return
            plasma_is_lower_band_kind(plan->active_renderer_kind) &&
            !plan->advanced_enabled &&
            !plan->modern_enabled &&
            !plan->premium_enabled;
    }

    if (renderer_kind == SCREENSAVE_RENDERER_KIND_GL21) {
        return
            plan->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL21 &&
            plan->advanced_enabled &&
            !plan->modern_enabled &&
            !plan->premium_enabled;
    }

    if (renderer_kind == SCREENSAVE_RENDERER_KIND_GL33) {
        return
            plan->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL33 &&
            plan->advanced_enabled &&
            plan->modern_enabled &&
            !plan->premium_enabled;
    }

    if (renderer_kind == SCREENSAVE_RENDERER_KIND_GL46) {
        return
            plan->active_renderer_kind == SCREENSAVE_RENDERER_KIND_GL46 &&
            plan->advanced_enabled &&
            plan->modern_enabled &&
            (
                plan->premium_enabled ||
                (!plan->premium_enabled && !plan->premium_capable)
            );
    }

    return 0;
}

int plasma_plan_validate_lower_band_baseline(
    const struct plasma_plan_tag *plan,
    const screensave_saver_module *module
)
{
    return
        plasma_plan_validate_for_renderer_kind(plan, module, SCREENSAVE_RENDERER_KIND_GDI) &&
        plasma_plan_validate_for_renderer_kind(plan, module, SCREENSAVE_RENDERER_KIND_GL11);
}
