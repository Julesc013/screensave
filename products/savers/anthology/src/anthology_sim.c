#include <stdlib.h>
#include <string.h>

#include "../../../../platform/src/core/base/saver_registry.h"
#include "anthology_internal.h"

const screensave_saver_module *nocturne_get_module(void);
const screensave_saver_module *ricochet_get_module(void);
const screensave_saver_module *deepfield_get_module(void);
const screensave_saver_module *plasma_get_module(void);
const screensave_saver_module *phosphor_get_module(void);
const screensave_saver_module *pipeworks_get_module(void);
const screensave_saver_module *lifeforms_get_module(void);
const screensave_saver_module *signals_get_module(void);
const screensave_saver_module *mechanize_get_module(void);
const screensave_saver_module *ecosystems_get_module(void);
const screensave_saver_module *stormglass_get_module(void);
const screensave_saver_module *transit_get_module(void);
const screensave_saver_module *observatory_get_module(void);
const screensave_saver_module *vector_get_module(void);
const screensave_saver_module *explorer_get_module(void);
const screensave_saver_module *city_get_module(void);
const screensave_saver_module *atlas_get_module(void);
const screensave_saver_module *gallery_get_module(void);

typedef const screensave_saver_module *(*anthology_module_getter)(void);

typedef struct anthology_catalog_entry_tag {
    const char *product_key;
    const char *display_name;
    unsigned long family_flags;
    unsigned long default_weight;
    anthology_module_getter get_module;
} anthology_catalog_entry;

static const anthology_catalog_entry g_anthology_catalog[] = {
    { "nocturne", "Nocturne", ANTHOLOGY_FAMILY_QUIET, 100UL, nocturne_get_module },
    { "ricochet", "Ricochet", ANTHOLOGY_FAMILY_MOTION, 100UL, ricochet_get_module },
    { "deepfield", "Deepfield", ANTHOLOGY_FAMILY_MOTION, 100UL, deepfield_get_module },
    { "plasma", "Plasma", ANTHOLOGY_FAMILY_FRAMEBUFFER_VECTOR, 100UL, plasma_get_module },
    { "phosphor", "Phosphor", ANTHOLOGY_FAMILY_FRAMEBUFFER_VECTOR, 100UL, phosphor_get_module },
    { "pipeworks", "Pipeworks", ANTHOLOGY_FAMILY_GRID_SIMULATION, 100UL, pipeworks_get_module },
    { "lifeforms", "Lifeforms", ANTHOLOGY_FAMILY_GRID_SIMULATION, 100UL, lifeforms_get_module },
    { "signals", "Signals", ANTHOLOGY_FAMILY_SYSTEMS_AMBIENT, 100UL, signals_get_module },
    { "mechanize", "Mechanize", ANTHOLOGY_FAMILY_SYSTEMS_AMBIENT, 100UL, mechanize_get_module },
    { "ecosystems", "Ecosystems", ANTHOLOGY_FAMILY_SYSTEMS_AMBIENT, 100UL, ecosystems_get_module },
    { "stormglass", "Stormglass", ANTHOLOGY_FAMILY_PLACES_ATMOSPHERE, 100UL, stormglass_get_module },
    { "transit", "Transit", ANTHOLOGY_FAMILY_PLACES_ATMOSPHERE, 100UL, transit_get_module },
    { "observatory", "Observatory", ANTHOLOGY_FAMILY_PLACES_ATMOSPHERE, 100UL, observatory_get_module },
    { "vector", "Vector", ANTHOLOGY_FAMILY_HEAVYWEIGHT_WORLDS, 100UL, vector_get_module },
    { "explorer", "Explorer", ANTHOLOGY_FAMILY_HEAVYWEIGHT_WORLDS, 100UL, explorer_get_module },
    { "city", "City", ANTHOLOGY_FAMILY_HEAVYWEIGHT_WORLDS, 100UL, city_get_module },
    { "atlas", "Atlas", ANTHOLOGY_FAMILY_HEAVYWEIGHT_WORLDS, 100UL, atlas_get_module },
    {
        "gallery",
        "Gallery",
        ANTHOLOGY_FAMILY_HEAVYWEIGHT_WORLDS | ANTHOLOGY_FAMILY_SHOWCASE,
        100UL,
        gallery_get_module
    }
};

static void anthology_emit_session_diag(
    const screensave_saver_environment *environment,
    screensave_diag_level level,
    unsigned long code,
    const char *text
)
{
    if (environment == NULL || environment->diagnostics == NULL) {
        return;
    }

    screensave_diag_emit(
        environment->diagnostics,
        level,
        SCREENSAVE_DIAG_DOMAIN_SAVER,
        code,
        "anthology",
        text
    );
}

static unsigned long anthology_mix_seed(unsigned long value)
{
    return (value * 1664525UL) + 1013904223UL;
}

static screensave_renderer_kind anthology_resolve_renderer_kind(
    const screensave_saver_environment *environment
)
{
    screensave_renderer_info info;

    if (environment == NULL || environment->renderer == NULL) {
        return SCREENSAVE_RENDERER_KIND_GDI;
    }

    screensave_renderer_get_info(environment->renderer, &info);
    return info.active_kind;
}

static screensave_renderer_kind anthology_effective_renderer_tier(screensave_renderer_kind renderer_kind)
{
    switch (renderer_kind) {
    case SCREENSAVE_RENDERER_KIND_GL46:
    case SCREENSAVE_RENDERER_KIND_GL33:
    case SCREENSAVE_RENDERER_KIND_GL21:
        return SCREENSAVE_RENDERER_KIND_GL21;

    case SCREENSAVE_RENDERER_KIND_GL11:
        return SCREENSAVE_RENDERER_KIND_GL11;

    case SCREENSAVE_RENDERER_KIND_NULL:
    case SCREENSAVE_RENDERER_KIND_GDI:
    case SCREENSAVE_RENDERER_KIND_UNKNOWN:
    default:
        return SCREENSAVE_RENDERER_KIND_GDI;
    }
}

static int anthology_module_supports_renderer(
    const screensave_saver_module *module,
    screensave_renderer_kind renderer_kind
)
{
    screensave_renderer_kind effective_kind;

    if (module == NULL) {
        return 0;
    }

    effective_kind = anthology_effective_renderer_tier(renderer_kind);
    switch (effective_kind) {
    case SCREENSAVE_RENDERER_KIND_GL21:
        return (module->capability_flags & SCREENSAVE_SAVER_CAP_GL21) != 0UL;

    case SCREENSAVE_RENDERER_KIND_GL11:
        return (module->capability_flags & SCREENSAVE_SAVER_CAP_GL11) != 0UL;

    case SCREENSAVE_RENDERER_KIND_GDI:
    default:
        return (module->capability_flags & SCREENSAVE_SAVER_CAP_GDI) != 0UL;
    }
}

static unsigned long anthology_candidate_weight(
    const screensave_saver_session *session,
    unsigned int catalog_index,
    const screensave_saver_module *module,
    screensave_renderer_kind renderer_kind
)
{
    unsigned long weight;

    if (session == NULL || module == NULL || catalog_index >= ANTHOLOGY_INNER_SAVER_COUNT) {
        return 0UL;
    }

    if (session->config.selection_mode == ANTHOLOGY_SELECTION_RANDOM_UNIFORM) {
        weight = 1UL;
    } else {
        weight = session->config.saver_weights[catalog_index];
        if (weight == 0UL) {
            weight = anthology_catalog_get_default_weight(catalog_index);
        }
    }

    if ((session->config.favorite_mask & (1UL << catalog_index)) != 0UL) {
        weight += ANTHOLOGY_FAVORITE_WEIGHT_BONUS;
    }

    if (
        session->config.renderer_policy == ANTHOLOGY_RENDERER_POLICY_PREFER_EXPLICIT_TIER &&
        anthology_effective_renderer_tier(renderer_kind) != SCREENSAVE_RENDERER_KIND_GDI &&
        anthology_module_supports_renderer(module, renderer_kind)
    ) {
        weight += ANTHOLOGY_TIER_WEIGHT_BONUS;
    }

    return weight;
}

unsigned int anthology_catalog_count(void)
{
    return (unsigned int)(sizeof(g_anthology_catalog) / sizeof(g_anthology_catalog[0]));
}

const screensave_saver_module *anthology_catalog_get_module(unsigned int index)
{
    if (index >= anthology_catalog_count()) {
        return NULL;
    }

    return g_anthology_catalog[index].get_module();
}

const char *anthology_catalog_get_product_key(unsigned int index)
{
    if (index >= anthology_catalog_count()) {
        return "";
    }

    return g_anthology_catalog[index].product_key;
}

const char *anthology_catalog_get_display_name(unsigned int index)
{
    if (index >= anthology_catalog_count()) {
        return "";
    }

    return g_anthology_catalog[index].display_name;
}

unsigned long anthology_catalog_get_family_flags(unsigned int index)
{
    if (index >= anthology_catalog_count()) {
        return 0UL;
    }

    return g_anthology_catalog[index].family_flags;
}

unsigned long anthology_catalog_get_default_weight(unsigned int index)
{
    if (index >= anthology_catalog_count()) {
        return ANTHOLOGY_DEFAULT_WEIGHT;
    }

    return g_anthology_catalog[index].default_weight;
}

int anthology_catalog_find_index(const char *product_key)
{
    const char *canonical_key;
    unsigned int index;

    canonical_key = screensave_saver_registry_canonical_product_key(product_key);
    if (canonical_key == NULL || lstrcmpiA(canonical_key, "anthology") == 0) {
        return -1;
    }

    for (index = 0U; index < anthology_catalog_count(); ++index) {
        if (lstrcmpiA(g_anthology_catalog[index].product_key, canonical_key) == 0) {
            return (int)index;
        }
    }

    return -1;
}

unsigned long anthology_all_family_flags(void)
{
    return ANTHOLOGY_FAMILY_QUIET |
        ANTHOLOGY_FAMILY_MOTION |
        ANTHOLOGY_FAMILY_FRAMEBUFFER_VECTOR |
        ANTHOLOGY_FAMILY_GRID_SIMULATION |
        ANTHOLOGY_FAMILY_SYSTEMS_AMBIENT |
        ANTHOLOGY_FAMILY_PLACES_ATMOSPHERE |
        ANTHOLOGY_FAMILY_HEAVYWEIGHT_WORLDS |
        ANTHOLOGY_FAMILY_SHOWCASE;
}

unsigned long anthology_all_saver_bits(void)
{
    return (1UL << ANTHOLOGY_INNER_SAVER_COUNT) - 1UL;
}

static void anthology_text_copy(char *buffer, unsigned int buffer_size, const char *text)
{
    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    if (text == NULL) {
        text = "";
    }

    lstrcpynA(buffer, text, (int)buffer_size);
}

static void anthology_append_text(char *buffer, unsigned int buffer_size, const char *text)
{
    unsigned int used;
    unsigned int needed;

    if (buffer == NULL || buffer_size == 0U || text == NULL) {
        return;
    }

    used = (unsigned int)lstrlenA(buffer);
    needed = (unsigned int)lstrlenA(text);
    if (used + needed + 1U > buffer_size) {
        return;
    }

    lstrcpyA(buffer + used, text);
}

static void anthology_append_family_text(char *buffer, unsigned int buffer_size, unsigned long family_mask)
{
    unsigned int index;
    unsigned long family_flag;

    if (buffer == NULL || buffer_size == 0U) {
        return;
    }

    buffer[0] = '\0';
    if (family_mask == anthology_all_family_flags()) {
        anthology_text_copy(buffer, buffer_size, "all");
        return;
    }
    if (family_mask == 0UL) {
        anthology_text_copy(buffer, buffer_size, "none");
        return;
    }

    for (index = 0U; index < 8U; ++index) {
        family_flag = 1UL << index;
        if ((family_mask & family_flag) == 0UL) {
            continue;
        }

        if (buffer[0] != '\0') {
            anthology_append_text(buffer, buffer_size, ",");
        }
        anthology_append_text(buffer, buffer_size, anthology_family_display_name(family_flag));
    }
}

static void anthology_destroy_inner_runtime(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    if (
        session->inner_session != NULL &&
        session->inner_module != NULL &&
        session->inner_module->callbacks != NULL &&
        session->inner_module->callbacks->destroy_session != NULL
    ) {
        session->inner_module->callbacks->destroy_session(session->inner_session);
    }

    session->inner_session = NULL;
    session->inner_module = NULL;
    screensave_saver_config_state_dispose(&session->inner_stored_config);
    screensave_saver_config_state_dispose(&session->inner_resolved_config);
    ZeroMemory(&session->inner_config_binding, sizeof(session->inner_config_binding));
}

static void anthology_build_inner_environment(
    const screensave_saver_session *session,
    const screensave_saver_environment *environment,
    screensave_saver_environment *inner_environment
)
{
    if (session == NULL || environment == NULL || inner_environment == NULL) {
        return;
    }

    *inner_environment = *environment;
    inner_environment->config_binding = &session->inner_config_binding;
    inner_environment->seed = session->inner_seed;
}

static void anthology_prepare_inner_randomization(
    const screensave_saver_session *session,
    screensave_common_config *common_config
)
{
    unsigned long randomization_scope;

    if (session == NULL || common_config == NULL) {
        return;
    }

    randomization_scope = 0UL;
    if (session->config.respect_inner_randomization) {
        if (common_config->randomization_mode == SCREENSAVE_RANDOMIZATION_MODE_SESSION) {
            randomization_scope = common_config->randomization_scope;
        }
    }

    if (session->config.randomize_inner_presets) {
        randomization_scope |= SCREENSAVE_RANDOMIZATION_SCOPE_PRESET;
    }
    if (session->config.randomize_inner_themes) {
        randomization_scope |= SCREENSAVE_RANDOMIZATION_SCOPE_THEME;
    }

    if (randomization_scope == 0UL) {
        common_config->randomization_mode = SCREENSAVE_RANDOMIZATION_MODE_OFF;
    } else {
        common_config->randomization_mode = SCREENSAVE_RANDOMIZATION_MODE_SESSION;
    }
    common_config->randomization_scope = randomization_scope;
}

static int anthology_build_candidates(
    const screensave_saver_session *session,
    screensave_renderer_kind renderer_kind,
    int favorites_only,
    int require_explicit_tier,
    unsigned long reject_mask,
    unsigned int *candidate_indices,
    unsigned long *candidate_weights
)
{
    unsigned int catalog_index;
    unsigned int candidate_count;

    if (session == NULL || candidate_indices == NULL || candidate_weights == NULL) {
        return 0;
    }

    candidate_count = 0U;
    for (catalog_index = 0U; catalog_index < anthology_catalog_count(); ++catalog_index) {
        const screensave_saver_module *module;
        unsigned long saver_bit;
        unsigned long family_flags;
        unsigned long weight;

        saver_bit = 1UL << catalog_index;
        if ((reject_mask & saver_bit) != 0UL) {
            continue;
        }
        if ((session->config.include_mask & saver_bit) == 0UL) {
            continue;
        }

        family_flags = anthology_catalog_get_family_flags(catalog_index);
        if ((family_flags & session->config.family_mask) == 0UL) {
            continue;
        }
        if (favorites_only && (session->config.favorite_mask & saver_bit) == 0UL) {
            continue;
        }

        module = anthology_catalog_get_module(catalog_index);
        if (!screensave_saver_module_is_valid(module)) {
            continue;
        }
        if (require_explicit_tier && !anthology_module_supports_renderer(module, renderer_kind)) {
            continue;
        }

        weight = anthology_candidate_weight(session, catalog_index, module, renderer_kind);
        if (weight == 0UL) {
            continue;
        }

        candidate_indices[candidate_count] = catalog_index;
        candidate_weights[candidate_count] = weight;
        candidate_count += 1U;
    }

    return (int)candidate_count;
}

static int anthology_choose_catalog_index(
    screensave_saver_session *session,
    const screensave_saver_environment *environment,
    unsigned long reject_mask,
    int *catalog_index_out
)
{
    unsigned int candidate_indices[ANTHOLOGY_INNER_SAVER_COUNT];
    unsigned long candidate_weights[ANTHOLOGY_INNER_SAVER_COUNT];
    screensave_renderer_kind renderer_kind;
    int favorites_only;
    int require_explicit_tier;
    int candidate_count;
    unsigned int candidate_index;
    unsigned long total_weight;
    unsigned long pick_value;

    if (session == NULL || environment == NULL || catalog_index_out == NULL) {
        return 0;
    }

    renderer_kind = anthology_resolve_renderer_kind(environment);
    require_explicit_tier =
        session->config.renderer_policy == ANTHOLOGY_RENDERER_POLICY_REQUIRE_EXPLICIT_TIER &&
        anthology_effective_renderer_tier(renderer_kind) != SCREENSAVE_RENDERER_KIND_GDI;
    favorites_only =
        session->config.selection_mode == ANTHOLOGY_SELECTION_FAVORITES_FIRST &&
        (session->config.favorite_mask & session->config.include_mask) != 0UL;

    candidate_count = anthology_build_candidates(
        session,
        renderer_kind,
        favorites_only,
        require_explicit_tier,
        reject_mask,
        candidate_indices,
        candidate_weights
    );
    if (candidate_count <= 0 && favorites_only) {
        favorites_only = 0;
        candidate_count = anthology_build_candidates(
            session,
            renderer_kind,
            favorites_only,
            require_explicit_tier,
            reject_mask,
            candidate_indices,
            candidate_weights
        );
    }
    if (candidate_count <= 0 && require_explicit_tier) {
        require_explicit_tier = 0;
        candidate_count = anthology_build_candidates(
            session,
            renderer_kind,
            favorites_only,
            require_explicit_tier,
            reject_mask,
            candidate_indices,
            candidate_weights
        );
    }

    session->last_renderer_kind = renderer_kind;
    session->last_candidate_count = candidate_count > 0 ? (unsigned int)candidate_count : 0U;
    session->last_favorites_only = favorites_only;
    if (candidate_count <= 0) {
        anthology_text_copy(
            session->last_filter_summary,
            sizeof(session->last_filter_summary),
            "no eligible inner saver survived the current include/family/renderer filters"
        );
        anthology_text_copy(
            session->last_selection_reason,
            sizeof(session->last_selection_reason),
            "black_fallback_no_candidate"
        );
        return 0;
    }

    if (candidate_count > 1 && session->current_catalog_index >= 0) {
        total_weight = 0UL;
        for (candidate_index = 0U; candidate_index < (unsigned int)candidate_count; ++candidate_index) {
            if ((int)candidate_indices[candidate_index] == session->current_catalog_index) {
                continue;
            }
            total_weight += candidate_weights[candidate_index];
        }
        if (total_weight > 0UL) {
            for (candidate_index = 0U; candidate_index < (unsigned int)candidate_count; ++candidate_index) {
                if ((int)candidate_indices[candidate_index] == session->current_catalog_index) {
                    candidate_weights[candidate_index] = 0UL;
                }
            }
        }
    }

    total_weight = 0UL;
    for (candidate_index = 0U; candidate_index < (unsigned int)candidate_count; ++candidate_index) {
        total_weight += candidate_weights[candidate_index];
    }
    if (total_weight == 0UL) {
        return 0;
    }

    session->selection_stream = anthology_mix_seed(
        session->selection_stream ^
        environment->seed.stream_seed ^
        reject_mask ^
        (unsigned long)(session->switch_count + 1UL)
    );
    pick_value = session->selection_stream % total_weight;
    for (candidate_index = 0U; candidate_index < (unsigned int)candidate_count; ++candidate_index) {
        if (candidate_weights[candidate_index] == 0UL) {
            continue;
        }
        if (pick_value < candidate_weights[candidate_index]) {
            *catalog_index_out = (int)candidate_indices[candidate_index];
            session->last_effective_weight = candidate_weights[candidate_index];
            session->last_family_flags = anthology_catalog_get_family_flags(candidate_indices[candidate_index]);
            wsprintfA(
                session->last_selection_reason,
                "%s weight=%lu",
                anthology_catalog_get_product_key(candidate_indices[candidate_index]),
                candidate_weights[candidate_index]
            );
            wsprintfA(
                session->last_filter_summary,
                "mode=%s renderer=%s policy=%s candidates=%u favorites_only=%s",
                anthology_selection_mode_name(session->config.selection_mode),
                screensave_renderer_kind_name(renderer_kind),
                anthology_renderer_policy_name(session->config.renderer_policy),
                (unsigned int)candidate_count,
                favorites_only ? "yes" : "no"
            );
            return 1;
        }
        pick_value -= candidate_weights[candidate_index];
    }

    return 0;
}

static int anthology_activate_catalog_index(
    screensave_saver_session *session,
    const screensave_saver_environment *environment,
    int catalog_index
)
{
    const screensave_saver_module *module;
    screensave_saver_environment inner_environment;

    if (session == NULL || environment == NULL || catalog_index < 0) {
        return 0;
    }

    module = anthology_catalog_get_module((unsigned int)catalog_index);
    if (
        module == NULL ||
        module->callbacks == NULL ||
        module->callbacks->create_session == NULL
    ) {
        return 0;
    }

    anthology_destroy_inner_runtime(session);
    if (
        !screensave_saver_config_state_init(module, &session->inner_stored_config) ||
        !screensave_saver_config_state_init(module, &session->inner_resolved_config)
    ) {
        anthology_destroy_inner_runtime(session);
        return 0;
    }

    screensave_saver_config_state_set_defaults(module, &session->inner_stored_config);
    if (!screensave_saver_config_state_load(module, &session->inner_stored_config, environment->diagnostics)) {
        screensave_saver_config_state_set_defaults(module, &session->inner_stored_config);
        anthology_emit_session_diag(
            environment,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            6805UL,
            "Anthology fell back to inner-saver defaults because its stored config could not be loaded."
        );
    }
    anthology_prepare_inner_randomization(session, &session->inner_stored_config.common);
    screensave_saver_config_state_clamp(module, &session->inner_stored_config);

    session->inner_seed = environment->seed;
    session->inner_seed.base_seed = anthology_mix_seed(
        environment->seed.base_seed ^ (unsigned long)(catalog_index + 1U) ^ session->selection_stream
    );
    session->inner_seed.stream_seed = anthology_mix_seed(
        environment->seed.stream_seed ^ (unsigned long)(catalog_index + 1U) ^ (session->switch_count + 1UL)
    );

    if (
        !screensave_saver_config_state_resolve_for_session(
                module,
                &session->inner_stored_config,
                &session->inner_seed,
                &session->inner_resolved_config,
                environment->diagnostics
            )
    ) {
        if (
            !screensave_saver_config_state_copy(
                    module,
                    &session->inner_resolved_config,
                    &session->inner_stored_config
                )
        ) {
            anthology_destroy_inner_runtime(session);
            return 0;
        }
        screensave_saver_config_state_clamp(module, &session->inner_resolved_config);
        anthology_emit_session_diag(
            environment,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            6806UL,
            "Anthology fell back to the stored inner-saver config because session randomization could not be resolved."
        );
    }

    screensave_config_binding_init(
        &session->inner_config_binding,
        &session->inner_resolved_config.common,
        session->inner_resolved_config.product_config,
        session->inner_resolved_config.product_config_size
    );
    anthology_build_inner_environment(session, environment, &inner_environment);

    if (!module->callbacks->create_session(module, &session->inner_session, &inner_environment)) {
        anthology_destroy_inner_runtime(session);
        return 0;
    }

    session->previous_catalog_index = session->current_catalog_index;
    session->current_catalog_index = catalog_index;
    session->inner_module = module;
    session->switch_count += 1UL;
    session->next_switch_elapsed_millis = environment->clock.elapsed_millis + session->interval_millis;
    return 1;
}

static int anthology_switch_inner_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long reject_mask;
    int catalog_index;
    unsigned int attempt;

    if (session == NULL || environment == NULL) {
        return 0;
    }

    reject_mask = 0UL;
    for (attempt = 0U; attempt < ANTHOLOGY_INNER_SAVER_COUNT; ++attempt) {
        if (!anthology_choose_catalog_index(session, environment, reject_mask, &catalog_index)) {
            break;
        }

        if (anthology_activate_catalog_index(session, environment, catalog_index)) {
            return 1;
        }

        reject_mask |= (1UL << catalog_index);
        anthology_emit_session_diag(
            environment,
            SCREENSAVE_DIAG_LEVEL_WARNING,
            6807UL,
            "Anthology skipped a chosen inner saver because its session could not be started."
        );
    }

    anthology_destroy_inner_runtime(session);
    session->current_catalog_index = -1;
    session->next_switch_elapsed_millis = environment->clock.elapsed_millis + 5000UL;
    anthology_text_copy(session->last_selection_reason, sizeof(session->last_selection_reason), "black_fallback_start_failure");
    return 0;
}

static int anthology_current_renderer_is_acceptable(
    const screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || session->inner_module == NULL) {
        return 0;
    }

    if (session->config.renderer_policy != ANTHOLOGY_RENDERER_POLICY_REQUIRE_EXPLICIT_TIER) {
        return 1;
    }

    return anthology_module_supports_renderer(
        session->inner_module,
        anthology_resolve_renderer_kind(environment)
    );
}

int anthology_session_build_summary(
    const screensave_saver_session *session,
    char *buffer,
    unsigned int buffer_size
)
{
    char current_text[32];
    char previous_text[32];
    char family_text[96];

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    if (session == NULL) {
        return 0;
    }

    anthology_text_copy(
        current_text,
        sizeof(current_text),
        session->current_catalog_index >= 0 ? anthology_catalog_get_product_key((unsigned int)session->current_catalog_index) : "none"
    );
    anthology_text_copy(
        previous_text,
        sizeof(previous_text),
        session->previous_catalog_index >= 0 ? anthology_catalog_get_product_key((unsigned int)session->previous_catalog_index) : "none"
    );
    anthology_append_family_text(family_text, sizeof(family_text), session->config.family_mask);
    wsprintfA(
        buffer,
        "Anthology inner=%s prev=%s mode=%s interval=%lus candidates=%u renderer=%s",
        current_text,
        previous_text,
        anthology_selection_mode_name(session->config.selection_mode),
        session->config.interval_seconds,
        session->last_candidate_count,
        screensave_renderer_kind_name(session->last_renderer_kind)
    );
    anthology_append_text(buffer, buffer_size, "\r\nFamilies: ");
    anthology_append_text(buffer, buffer_size, family_text);
    anthology_append_text(buffer, buffer_size, "\r\nSelection: ");
    anthology_append_text(buffer, buffer_size, session->last_selection_reason);
    anthology_append_text(buffer, buffer_size, "\r\nFilter: ");
    anthology_append_text(buffer, buffer_size, session->last_filter_summary);
    return 1;
}

int anthology_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;
    const anthology_config *configured;
    screensave_common_config scratch_common;
    const screensave_common_config *configured_common;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    *session_out = NULL;
    session = (screensave_saver_session *)calloc(1, sizeof(*session));
    if (session == NULL) {
        anthology_emit_session_diag(
            environment,
            SCREENSAVE_DIAG_LEVEL_ERROR,
            6808UL,
            "Anthology could not allocate its session state."
        );
        return 0;
    }

    session->current_catalog_index = -1;
    session->previous_catalog_index = -1;
    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    session->last_renderer_kind = anthology_resolve_renderer_kind(environment);

    configured_common = NULL;
    screensave_common_config_set_defaults(&scratch_common);
    anthology_config_set_defaults(&scratch_common, &session->config, sizeof(session->config));
    if (
        environment->config_binding != NULL &&
        environment->config_binding->common_config != NULL
    ) {
        configured_common = environment->config_binding->common_config;
        scratch_common = *configured_common;
    }

    configured = NULL;
    if (
        environment->config_binding != NULL &&
        environment->config_binding->product_config != NULL &&
        environment->config_binding->product_config_size == sizeof(anthology_config)
    ) {
        configured = (const anthology_config *)environment->config_binding->product_config;
    }

    if (configured != NULL) {
        session->config = *configured;
    }
    anthology_config_clamp(&scratch_common, &session->config, sizeof(session->config));

    session->interval_millis = session->config.interval_seconds * 1000UL;
    session->selection_stream = anthology_mix_seed(environment->seed.stream_seed ^ environment->seed.base_seed ^ 0xA8710F08UL);
    anthology_text_copy(session->last_selection_reason, sizeof(session->last_selection_reason), "initial_selection_pending");
    anthology_text_copy(session->last_filter_summary, sizeof(session->last_filter_summary), "filters_pending");
    *session_out = session;
    (void)anthology_switch_inner_session(session, environment);
    return 1;
}

void anthology_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    anthology_destroy_inner_runtime(session);
    free(session);
}

void anthology_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_saver_environment inner_environment;

    if (session == NULL || environment == NULL) {
        return;
    }

    session->drawable_size = environment->drawable_size;
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    if (
        session->inner_session != NULL &&
        session->inner_module != NULL &&
        session->inner_module->callbacks != NULL &&
        session->inner_module->callbacks->resize_session != NULL
    ) {
        anthology_build_inner_environment(session, environment, &inner_environment);
        session->inner_module->callbacks->resize_session(session->inner_session, &inner_environment);
    }
}

void anthology_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    screensave_saver_environment inner_environment;

    if (session == NULL || environment == NULL) {
        return;
    }

    session->last_elapsed_millis = environment->clock.elapsed_millis;
    session->last_renderer_kind = anthology_resolve_renderer_kind(environment);
    if (
        session->inner_session == NULL ||
        !anthology_current_renderer_is_acceptable(session, environment) ||
        environment->clock.elapsed_millis >= session->next_switch_elapsed_millis
    ) {
        (void)anthology_switch_inner_session(session, environment);
    }

    if (
        session->inner_session != NULL &&
        session->inner_module != NULL &&
        session->inner_module->callbacks != NULL &&
        session->inner_module->callbacks->step_session != NULL
    ) {
        anthology_build_inner_environment(session, environment, &inner_environment);
        session->inner_module->callbacks->step_session(session->inner_session, &inner_environment);
    }
}
