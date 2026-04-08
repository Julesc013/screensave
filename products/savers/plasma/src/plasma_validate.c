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
        "validation/captures/pl13/benchlab-plasma-journey-gl11.txt",
        "Journey-focused GL11 report proves the bounded transition subset remains active without premium dependence."
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
        "transition_soak",
        "product",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "products/savers/plasma/tests/pl13-transition-validation.md",
        "Bounded repeated transition stepping is validated, but exhaustive pairwise long-run transition proof remains outside PL13."
    },
    {
        "experimental_pool",
        "product",
        PLASMA_VALIDATION_STATUS_DOCUMENTED_ONLY,
        "products/savers/plasma/docs/pl06-selection-and-sets.md",
        "Experimental-pool plumbing exists, but shipped Plasma content remains stable-only in the current repo."
    },
    {
        "non_heightfield_presentations",
        "gl46",
        PLASMA_VALIDATION_STATUS_UNSUPPORTED,
        "products/savers/plasma/docs/pl09-premium-lane.md",
        "Curtain, ribbon, contour extrusion, bounded surface, and billboard-style premium presentations are not implemented."
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
        "Real theme morph, preset morph, bridge, fallback, and journey behavior are exercised only for bounded curated pairs and sets.",
        "Transition timing, fallback, and journey progression are validated for the implemented subset only.",
        "Repeated transition stepping is bounded and stable in smoke, but exhaustive pairwise soak coverage is not claimed.",
        "products/savers/plasma/tests/pl13-transition-validation.md"
    }
};

static const plasma_known_limit_entry g_plasma_known_limits[] = {
    {
        "experimental_pool_empty",
        PLASMA_VALIDATION_STATUS_DOCUMENTED_ONLY,
        "The current repo ships stable-only Plasma content. Experimental filtering and selection posture exist, but there is no shipped experimental pool to validate as content.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "premium_heightfield_only",
        PLASMA_VALIDATION_STATUS_UNSUPPORTED,
        "Heightfield is the only implemented dimensional presentation mode. Other premium-dimensional modes remain intentionally unsupported.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "transition_pair_coverage_bounded",
        PLASMA_VALIDATION_STATUS_PARTIAL,
        "Transition proof covers the implemented theme, preset, bridge, and journey subset, not every theoretical preset/theme pairing.",
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
        "The PL13 capture set comes from one capable validation machine and does not include a live lower-capability denial sweep across multiple drivers or GPUs.",
        "products/savers/plasma/docs/pl13-known-limits.md"
    },
    {
        "numeric_perf_sla_gap",
        PLASMA_VALIDATION_STATUS_BLOCKED,
        "PL13 records bounded qualitative envelopes and repeatable proof runs, but it does not claim hard numeric frame-time or memory SLAs.",
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
        !plan->classic_execution ||
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
            plan->premium_enabled;
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
