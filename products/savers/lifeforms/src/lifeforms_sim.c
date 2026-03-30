#include <stdlib.h>

#include "lifeforms_internal.h"

void lifeforms_rng_seed(lifeforms_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x0C411F35UL;
}

unsigned long lifeforms_rng_next(lifeforms_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long lifeforms_rng_range(lifeforms_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return lifeforms_rng_next(state) % upper_bound;
}

static const screensave_theme_descriptor *lifeforms_resolve_theme(
    const screensave_saver_environment *environment
)
{
    const screensave_common_config *common_config;
    const screensave_theme_descriptor *theme;

    if (environment == NULL || environment->config_binding == NULL) {
        return lifeforms_find_theme_descriptor(LIFEFORMS_DEFAULT_THEME_KEY);
    }

    common_config = environment->config_binding->common_config;
    if (common_config == NULL) {
        return lifeforms_find_theme_descriptor(LIFEFORMS_DEFAULT_THEME_KEY);
    }

    theme = lifeforms_find_theme_descriptor(common_config->theme_key);
    if (theme == NULL) {
        theme = lifeforms_find_theme_descriptor(LIFEFORMS_DEFAULT_THEME_KEY);
    }

    return theme;
}

static unsigned long lifeforms_step_interval(const screensave_saver_session *session)
{
    unsigned long interval;

    interval = 110UL;
    if (session == NULL) {
        return interval;
    }

    if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW) {
        interval = 140UL;
    } else if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH) {
        interval = 84UL;
    }

    if (session->preview_mode && interval < 120UL) {
        interval = 136UL;
    }

    return interval;
}

static int lifeforms_compute_cell_size(
    const screensave_saver_session *session,
    const screensave_sizei *drawable_size
)
{
    int cell_size;
    int max_for_grid;

    if (drawable_size == NULL) {
        return 8;
    }

    cell_size = 9;
    if (session != NULL) {
        switch (session->detail_level) {
        case SCREENSAVE_DETAIL_LEVEL_LOW:
            cell_size = 12;
            break;

        case SCREENSAVE_DETAIL_LEVEL_HIGH:
            cell_size = 7;
            break;

        case SCREENSAVE_DETAIL_LEVEL_STANDARD:
        default:
            cell_size = 9;
            break;
        }

        switch (session->config.density_mode) {
        case LIFEFORMS_DENSITY_SPARSE:
            cell_size += 3;
            break;

        case LIFEFORMS_DENSITY_DENSE:
            cell_size -= 2;
            break;

        default:
            break;
        }

        if (session->preview_mode && cell_size < 10) {
            cell_size = 10;
        }
    }

    max_for_grid = drawable_size->width / 12;
    if (drawable_size->height / 9 < max_for_grid || max_for_grid <= 0) {
        max_for_grid = drawable_size->height / 9;
    }
    if (max_for_grid > 0 && cell_size > max_for_grid) {
        cell_size = max_for_grid;
    }
    if (cell_size < 1) {
        cell_size = 1;
    }

    return cell_size;
}

static unsigned int lifeforms_total_cells(const screensave_saver_session *session)
{
    if (session == NULL || session->grid_size.width <= 0 || session->grid_size.height <= 0) {
        return 0U;
    }

    return (unsigned int)(session->grid_size.width * session->grid_size.height);
}

static unsigned int lifeforms_cell_index(
    const screensave_saver_session *session,
    int x,
    int y
)
{
    return (unsigned int)(y * session->current_cells.stride_cells + x);
}

static void lifeforms_clear_history(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    for (index = 0U; index < 4U; ++index) {
        session->recent_checksums[index] = 0UL;
        session->recent_populations[index] = 0U;
    }
    session->history_count = 0U;
    session->history_index = 0U;
    session->stagnant_ticks = 0U;
    session->low_activity_ticks = 0U;
}

static void lifeforms_record_history(
    screensave_saver_session *session,
    unsigned long checksum,
    unsigned int population
)
{
    if (session == NULL) {
        return;
    }

    session->recent_checksums[session->history_index] = checksum;
    session->recent_populations[session->history_index] = population;
    session->history_index = (session->history_index + 1U) % 4U;
    if (session->history_count < 4U) {
        session->history_count += 1U;
    }
}

static int lifeforms_history_repeats(
    const screensave_saver_session *session,
    unsigned long checksum,
    unsigned int population
)
{
    unsigned int index;

    if (session == NULL) {
        return 0;
    }

    for (index = 0U; index < session->history_count; ++index) {
        if (
            session->recent_checksums[index] == checksum &&
            session->recent_populations[index] == population
        ) {
            return 1;
        }
    }

    return 0;
}

static void lifeforms_seed_cluster(
    screensave_saver_session *session,
    int center_x,
    int center_y,
    int radius,
    unsigned int chance_scale
)
{
    int x;
    int y;

    if (session == NULL || session->current_cells.cells == NULL || radius < 0) {
        return;
    }

    for (y = center_y - radius; y <= center_y + radius; ++y) {
        for (x = center_x - radius; x <= center_x + radius; ++x) {
            int dx;
            int dy;
            unsigned int chance;
            unsigned int index;

            if (x < 0 || y < 0 || x >= session->grid_size.width || y >= session->grid_size.height) {
                continue;
            }

            dx = x - center_x;
            dy = y - center_y;
            chance = chance_scale;
            if ((dx * dx) + (dy * dy) > (radius * radius)) {
                chance /= 2U;
            }

            if (lifeforms_rng_range(&session->rng, 100UL) < chance) {
                index = lifeforms_cell_index(session, x, y);
                session->current_cells.cells[index] = 1U;
            }
        }
    }
}

static void lifeforms_seed_field(screensave_saver_session *session)
{
    unsigned int cluster_count;
    unsigned int cluster_index;
    unsigned int pattern_index;
    int radius;
    unsigned int base_chance;
    unsigned int total_cells;
    unsigned int index;

    if (session == NULL) {
        return;
    }

    screensave_byte_grid_clear(&session->current_cells, 0U);
    screensave_byte_grid_clear(&session->next_cells, 0U);
    screensave_byte_grid_clear(&session->ages, 0U);
    screensave_byte_grid_clear(&session->next_ages, 0U);

    cluster_count = 7U;
    radius = 2;
    base_chance = 58U;
    switch (session->config.seed_mode) {
    case LIFEFORMS_SEED_SPARSE:
        cluster_count = 4U;
        radius = 1;
        base_chance = 46U;
        break;

    case LIFEFORMS_SEED_BLOOM:
        cluster_count = 11U;
        radius = 3;
        base_chance = 72U;
        break;

    default:
        break;
    }

    if (session->preview_mode && cluster_count > 5U) {
        cluster_count = 5U;
    }

    for (cluster_index = 0U; cluster_index < cluster_count; ++cluster_index) {
        int center_x;
        int center_y;

        center_x = (int)lifeforms_rng_range(&session->rng, (unsigned long)session->grid_size.width);
        center_y = (int)lifeforms_rng_range(&session->rng, (unsigned long)session->grid_size.height);
        lifeforms_seed_cluster(session, center_x, center_y, radius, base_chance);
    }

    pattern_index = session->reseed_count % 4U;
    if (pattern_index == 0U) {
        lifeforms_seed_cluster(
            session,
            session->grid_size.width / 2,
            session->grid_size.height / 2,
            radius + 1,
            base_chance + 8U
        );
    } else if (pattern_index == 1U) {
        lifeforms_seed_cluster(
            session,
            session->grid_size.width / 3,
            session->grid_size.height / 3,
            radius,
            base_chance
        );
        lifeforms_seed_cluster(
            session,
            (session->grid_size.width * 2) / 3,
            (session->grid_size.height * 2) / 3,
            radius,
            base_chance
        );
    } else if (pattern_index == 2U) {
        lifeforms_seed_cluster(
            session,
            session->grid_size.width / 4,
            (session->grid_size.height * 3) / 4,
            radius,
            base_chance
        );
        lifeforms_seed_cluster(
            session,
            (session->grid_size.width * 3) / 4,
            session->grid_size.height / 4,
            radius,
            base_chance
        );
    } else {
        lifeforms_seed_cluster(
            session,
            session->grid_size.width / 2,
            session->grid_size.height / 3,
            radius,
            base_chance + 4U
        );
        lifeforms_seed_cluster(
            session,
            session->grid_size.width / 2,
            (session->grid_size.height * 2) / 3,
            radius,
            base_chance + 4U
        );
    }

    total_cells = lifeforms_total_cells(session);
    for (index = 0U; index < total_cells; ++index) {
        unsigned int sprinkle_chance;

        sprinkle_chance = 6U;
        if (session->config.seed_mode == LIFEFORMS_SEED_SPARSE) {
            sprinkle_chance = 3U;
        } else if (session->config.seed_mode == LIFEFORMS_SEED_BLOOM) {
            sprinkle_chance = 10U;
        }
        if (lifeforms_rng_range(&session->rng, 100UL) < sprinkle_chance) {
            session->current_cells.cells[index] = 1U;
        }
    }
}

static int lifeforms_should_live(
    const screensave_saver_session *session,
    int currently_alive,
    unsigned int neighbor_count
)
{
    if (currently_alive) {
        return neighbor_count == 2U || neighbor_count == 3U;
    }

    if (session != NULL && session->config.rule_mode == LIFEFORMS_RULE_HIGHLIFE) {
        return neighbor_count == 3U || neighbor_count == 6U;
    }

    return neighbor_count == 3U;
}

static unsigned int lifeforms_neighbor_count(
    const screensave_saver_session *session,
    int x,
    int y
)
{
    unsigned int count;
    int sample_x;
    int sample_y;

    count = 0U;
    if (session == NULL || session->current_cells.cells == NULL) {
        return 0U;
    }

    for (sample_y = y - 1; sample_y <= y + 1; ++sample_y) {
        for (sample_x = x - 1; sample_x <= x + 1; ++sample_x) {
            unsigned int index;

            if (sample_x == x && sample_y == y) {
                continue;
            }
            if (
                sample_x < 0 ||
                sample_y < 0 ||
                sample_x >= session->grid_size.width ||
                sample_y >= session->grid_size.height
            ) {
                continue;
            }

            index = lifeforms_cell_index(session, sample_x, sample_y);
            if (session->current_cells.cells[index] != 0U) {
                count += 1U;
            }
        }
    }

    return count;
}

static unsigned int lifeforms_low_activity_limit(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 72U;
    }

    switch (session->config.reseed_mode) {
    case LIFEFORMS_RESEED_PATIENT:
        return 120U;

    case LIFEFORMS_RESEED_ALERT:
        return 36U;

    case LIFEFORMS_RESEED_STANDARD:
    default:
        return 72U;
    }
}

static unsigned int lifeforms_stagnation_limit(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 48U;
    }

    switch (session->config.reseed_mode) {
    case LIFEFORMS_RESEED_PATIENT:
        return 80U;

    case LIFEFORMS_RESEED_ALERT:
        return 24U;

    case LIFEFORMS_RESEED_STANDARD:
    default:
        return 48U;
    }
}

static unsigned int lifeforms_low_population_threshold(const screensave_saver_session *session)
{
    unsigned int total_cells;
    unsigned int threshold;

    total_cells = lifeforms_total_cells(session);
    threshold = total_cells / 28U;
    if (threshold < 4U) {
        threshold = 4U;
    }
    return threshold;
}

static void lifeforms_reseed_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    session->population = 0U;
    session->generation = 0UL;
    session->step_accumulator = 0UL;
    lifeforms_clear_history(session);
    lifeforms_seed_field(session);
    session->reseed_count += 1U;
}

static void lifeforms_evaluate_reseed(
    screensave_saver_session *session,
    unsigned long checksum,
    unsigned int population
)
{
    unsigned int low_population_threshold;

    if (session == NULL) {
        return;
    }

    if (population == 0U) {
        lifeforms_reseed_session(session);
        return;
    }

    if (lifeforms_history_repeats(session, checksum, population)) {
        session->stagnant_ticks += 1U;
    } else if (session->stagnant_ticks > 0U) {
        session->stagnant_ticks -= 1U;
    }

    low_population_threshold = lifeforms_low_population_threshold(session);
    if (population <= low_population_threshold) {
        session->low_activity_ticks += 1U;
    } else if (session->low_activity_ticks > 0U) {
        session->low_activity_ticks -= 1U;
    }

    if (
        session->stagnant_ticks >= lifeforms_stagnation_limit(session) ||
        session->low_activity_ticks >= lifeforms_low_activity_limit(session)
    ) {
        lifeforms_reseed_session(session);
        return;
    }

    lifeforms_record_history(session, checksum, population);
}

static void lifeforms_step_generation(screensave_saver_session *session)
{
    int x;
    int y;
    unsigned int population;
    unsigned long checksum;

    if (session == NULL || session->current_cells.cells == NULL || session->next_cells.cells == NULL) {
        return;
    }

    population = 0U;
    for (y = 0; y < session->grid_size.height; ++y) {
        for (x = 0; x < session->grid_size.width; ++x) {
            unsigned int index;
            unsigned int neighbors;
            int currently_alive;
            int next_alive;
            unsigned char next_age;

            index = lifeforms_cell_index(session, x, y);
            neighbors = lifeforms_neighbor_count(session, x, y);
            currently_alive = session->current_cells.cells[index] != 0U;
            next_alive = lifeforms_should_live(session, currently_alive, neighbors);

            session->next_cells.cells[index] = next_alive ? 1U : 0U;
            if (next_alive) {
                if (currently_alive) {
                    next_age = session->ages.cells[index];
                    if (next_age < 248U) {
                        next_age = (unsigned char)(next_age + 8U);
                    }
                } else {
                    next_age = 32U;
                }
                session->next_ages.cells[index] = next_age;
                population += 1U;
            } else {
                session->next_ages.cells[index] = 0U;
            }
        }
    }

    screensave_byte_grid_swap(&session->current_cells, &session->next_cells);
    screensave_byte_grid_swap(&session->ages, &session->next_ages);
    session->population = population;
    session->generation += 1UL;

    checksum = screensave_byte_grid_checksum(&session->current_cells);
    lifeforms_evaluate_reseed(session, checksum, population);
}

static int lifeforms_resize_buffers(
    screensave_saver_session *session,
    const screensave_sizei *drawable_size
)
{
    screensave_sizei grid_size;

    if (session == NULL || drawable_size == NULL) {
        return 0;
    }

    session->drawable_size = *drawable_size;
    session->cell_size = lifeforms_compute_cell_size(session, drawable_size);
    grid_size.width = drawable_size->width / session->cell_size;
    grid_size.height = drawable_size->height / session->cell_size;
    if (grid_size.width < 1) {
        grid_size.width = 1;
    }
    if (grid_size.height < 1) {
        grid_size.height = 1;
    }

    if (
        !screensave_byte_grid_resize(&session->current_cells, &grid_size) ||
        !screensave_byte_grid_resize(&session->next_cells, &grid_size) ||
        !screensave_byte_grid_resize(&session->ages, &grid_size) ||
        !screensave_byte_grid_resize(&session->next_ages, &grid_size)
    ) {
        return 0;
    }

    session->grid_size = grid_size;
    lifeforms_reseed_session(session);
    return 1;
}

static void lifeforms_copy_bound_config(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    const screensave_config_binding *binding;

    if (session == NULL) {
        return;
    }

    session->config.rule_mode = LIFEFORMS_RULE_CONWAY;
    session->config.density_mode = LIFEFORMS_DENSITY_STANDARD;
    session->config.seed_mode = LIFEFORMS_SEED_BALANCED;
    session->config.reseed_mode = LIFEFORMS_RESEED_STANDARD;
    session->detail_level = SCREENSAVE_DETAIL_LEVEL_STANDARD;

    binding = environment != NULL ? environment->config_binding : NULL;
    if (
        binding != NULL &&
        binding->product_config != NULL &&
        binding->product_config_size == sizeof(lifeforms_config)
    ) {
        session->config = *(const lifeforms_config *)binding->product_config;
    }
    if (binding != NULL && binding->common_config != NULL) {
        session->detail_level = binding->common_config->detail_level;
    }

    if (session->config.rule_mode < LIFEFORMS_RULE_CONWAY || session->config.rule_mode > LIFEFORMS_RULE_HIGHLIFE) {
        session->config.rule_mode = LIFEFORMS_RULE_CONWAY;
    }
    if (
        session->config.density_mode < LIFEFORMS_DENSITY_SPARSE ||
        session->config.density_mode > LIFEFORMS_DENSITY_DENSE
    ) {
        session->config.density_mode = LIFEFORMS_DENSITY_STANDARD;
    }
    if (session->config.seed_mode < LIFEFORMS_SEED_SPARSE || session->config.seed_mode > LIFEFORMS_SEED_BLOOM) {
        session->config.seed_mode = LIFEFORMS_SEED_BALANCED;
    }
    if (
        session->config.reseed_mode < LIFEFORMS_RESEED_PATIENT ||
        session->config.reseed_mode > LIFEFORMS_RESEED_ALERT
    ) {
        session->config.reseed_mode = LIFEFORMS_RESEED_STANDARD;
    }
}

int lifeforms_create_session(
    const screensave_saver_module *module,
    screensave_saver_session **session_out,
    const screensave_saver_environment *environment
)
{
    screensave_saver_session *session;

    (void)module;

    if (session_out == NULL || environment == NULL) {
        return 0;
    }

    *session_out = NULL;
    session = (screensave_saver_session *)calloc(1U, sizeof(*session));
    if (session == NULL) {
        return 0;
    }

    lifeforms_copy_bound_config(session, environment);
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    session->theme = lifeforms_resolve_theme(environment);
    lifeforms_rng_seed(&session->rng, environment->seed.stream_seed ^ 0x4C696665UL);

    if (!lifeforms_resize_buffers(session, &environment->drawable_size)) {
        lifeforms_destroy_session(session);
        return 0;
    }

    *session_out = session;
    return 1;
}

void lifeforms_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    screensave_byte_grid_dispose(&session->current_cells);
    screensave_byte_grid_dispose(&session->next_cells);
    screensave_byte_grid_dispose(&session->ages);
    screensave_byte_grid_dispose(&session->next_ages);
    free(session);
}

void lifeforms_resize_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    if (session == NULL || environment == NULL) {
        return;
    }

    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    if (environment->config_binding != NULL && environment->config_binding->common_config != NULL) {
        session->detail_level = environment->config_binding->common_config->detail_level;
    }
    session->theme = lifeforms_resolve_theme(environment);
    (void)lifeforms_resize_buffers(session, &environment->drawable_size);
}

void lifeforms_step_session(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    unsigned long step_interval;
    unsigned int iteration_count;

    if (session == NULL || environment == NULL) {
        return;
    }

    if (environment->clock.delta_millis == 0UL) {
        return;
    }

    session->theme = lifeforms_resolve_theme(environment);
    session->step_accumulator += environment->clock.delta_millis;
    step_interval = lifeforms_step_interval(session);
    iteration_count = 0U;
    while (session->step_accumulator >= step_interval && iteration_count < 6U) {
        lifeforms_step_generation(session);
        session->step_accumulator -= step_interval;
        iteration_count += 1U;
    }
}
