#include <stdlib.h>

#include "pipeworks_internal.h"

static const int g_pipeworks_dir_x[4] = { 0, 1, 0, -1 };
static const int g_pipeworks_dir_y[4] = { -1, 0, 1, 0 };
static const unsigned char g_pipeworks_dir_mask[4] = {
    PIPEWORKS_CELL_NORTH,
    PIPEWORKS_CELL_EAST,
    PIPEWORKS_CELL_SOUTH,
    PIPEWORKS_CELL_WEST
};
static const unsigned char g_pipeworks_opposite_mask[4] = {
    PIPEWORKS_CELL_SOUTH,
    PIPEWORKS_CELL_WEST,
    PIPEWORKS_CELL_NORTH,
    PIPEWORKS_CELL_EAST
};

void pipeworks_rng_seed(pipeworks_rng_state *state, unsigned long seed)
{
    if (state == NULL) {
        return;
    }

    state->state = seed != 0UL ? seed : 0x0B7710A5UL;
}

unsigned long pipeworks_rng_next(pipeworks_rng_state *state)
{
    if (state == NULL) {
        return 0UL;
    }

    state->state = state->state * 1664525UL + 1013904223UL;
    return state->state;
}

unsigned long pipeworks_rng_range(pipeworks_rng_state *state, unsigned long upper_bound)
{
    if (upper_bound == 0UL) {
        return 0UL;
    }

    return pipeworks_rng_next(state) % upper_bound;
}

static const screensave_theme_descriptor *pipeworks_resolve_theme(
    const screensave_saver_environment *environment
)
{
    const screensave_common_config *common_config;
    const screensave_theme_descriptor *theme;

    if (environment == NULL || environment->config_binding == NULL) {
        return pipeworks_find_theme_descriptor(PIPEWORKS_DEFAULT_THEME_KEY);
    }

    common_config = environment->config_binding->common_config;
    if (common_config == NULL) {
        return pipeworks_find_theme_descriptor(PIPEWORKS_DEFAULT_THEME_KEY);
    }

    theme = pipeworks_find_theme_descriptor(common_config->theme_key);
    if (theme == NULL) {
        theme = pipeworks_find_theme_descriptor(PIPEWORKS_DEFAULT_THEME_KEY);
    }

    return theme;
}

static unsigned long pipeworks_step_interval(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 60UL;
    }

    switch (session->config.speed_mode) {
    case PIPEWORKS_SPEED_PATIENT:
        return session->preview_mode ? 88UL : 96UL;

    case PIPEWORKS_SPEED_BRISK:
        return session->preview_mode ? 42UL : 34UL;

    case PIPEWORKS_SPEED_STANDARD:
    default:
        return session->preview_mode ? 60UL : 56UL;
    }
}

static unsigned long pipeworks_growth_limit(const screensave_saver_session *session)
{
    unsigned long millis;

    millis = 11000UL;
    if (session == NULL) {
        return millis;
    }

    switch (session->config.rebuild_mode) {
    case PIPEWORKS_REBUILD_PATIENT:
        millis = 15000UL;
        break;

    case PIPEWORKS_REBUILD_FREQUENT:
        millis = 8000UL;
        break;

    case PIPEWORKS_REBUILD_CYCLE:
    default:
        millis = 11000UL;
        break;
    }

    if (session->preview_mode && millis > 3500UL) {
        millis = (millis * 3UL) / 5UL;
    }

    return millis;
}

static unsigned long pipeworks_flow_limit(const screensave_saver_session *session)
{
    unsigned long millis;

    millis = 5200UL;
    if (session == NULL) {
        return millis;
    }

    switch (session->config.rebuild_mode) {
    case PIPEWORKS_REBUILD_PATIENT:
        millis = 7000UL;
        break;

    case PIPEWORKS_REBUILD_FREQUENT:
        millis = 3600UL;
        break;

    case PIPEWORKS_REBUILD_CYCLE:
    default:
        millis = 5200UL;
        break;
    }

    if (session->preview_mode && millis > 2600UL) {
        millis = (millis * 3UL) / 5UL;
    }

    return millis;
}

static unsigned int pipeworks_target_fill_percent(const screensave_saver_session *session)
{
    int percent;

    percent = 62;
    if (session == NULL) {
        return 62U;
    }

    switch (session->config.density_mode) {
    case PIPEWORKS_DENSITY_TIGHT:
        percent = 72;
        break;

    case PIPEWORKS_DENSITY_OPEN:
        percent = 50;
        break;

    case PIPEWORKS_DENSITY_STANDARD:
    default:
        percent = 62;
        break;
    }

    switch (session->config.rebuild_mode) {
    case PIPEWORKS_REBUILD_PATIENT:
        percent += 6;
        break;

    case PIPEWORKS_REBUILD_FREQUENT:
        percent -= 10;
        break;

    default:
        break;
    }

    if (session->preview_mode) {
        percent -= 8;
    }
    if (percent < 28) {
        percent = 28;
    }
    if (percent > 82) {
        percent = 82;
    }

    return (unsigned int)percent;
}

static unsigned int pipeworks_branch_chance(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 0U;
    }

    switch (session->config.branch_mode) {
    case PIPEWORKS_BRANCH_ORDERLY:
        return 28U;

    case PIPEWORKS_BRANCH_WILD:
        return 132U;

    case PIPEWORKS_BRANCH_BALANCED:
    default:
        return 72U;
    }
}

static unsigned int pipeworks_continue_chance(const screensave_saver_session *session)
{
    if (session == NULL) {
        return 160U;
    }

    switch (session->config.branch_mode) {
    case PIPEWORKS_BRANCH_ORDERLY:
        return 228U;

    case PIPEWORKS_BRANCH_WILD:
        return 112U;

    case PIPEWORKS_BRANCH_BALANCED:
    default:
        return 172U;
    }
}

static unsigned int pipeworks_total_cells(const screensave_saver_session *session)
{
    if (session == NULL || session->grid_size.width <= 0 || session->grid_size.height <= 0) {
        return 0U;
    }

    return (unsigned int)(session->grid_size.width * session->grid_size.height);
}

static int pipeworks_compute_cell_size(
    const screensave_saver_session *session,
    const screensave_sizei *drawable_size
)
{
    int cell_size;
    int max_for_grid;

    if (drawable_size == NULL) {
        return 12;
    }

    cell_size = 13;
    if (session != NULL) {
        switch (session->detail_level) {
        case SCREENSAVE_DETAIL_LEVEL_LOW:
            cell_size = 17;
            break;

        case SCREENSAVE_DETAIL_LEVEL_HIGH:
            cell_size = 10;
            break;

        case SCREENSAVE_DETAIL_LEVEL_STANDARD:
        default:
            cell_size = 13;
            break;
        }

        switch (session->config.density_mode) {
        case PIPEWORKS_DENSITY_TIGHT:
            cell_size -= 2;
            break;

        case PIPEWORKS_DENSITY_OPEN:
            cell_size += 3;
            break;

        default:
            break;
        }

        if (session->preview_mode && cell_size < 14) {
            cell_size = 14;
        }
    }

    max_for_grid = drawable_size->width / 8;
    if (drawable_size->height / 6 < max_for_grid || max_for_grid <= 0) {
        max_for_grid = drawable_size->height / 6;
    }
    if (max_for_grid > 0 && cell_size > max_for_grid) {
        cell_size = max_for_grid;
    }
    if (cell_size < 1) {
        cell_size = 1;
    }

    return cell_size;
}

static unsigned int pipeworks_cell_index(
    const screensave_saver_session *session,
    int x,
    int y
)
{
    return (unsigned int)(y * session->cells.stride_cells + x);
}

static int pipeworks_in_bounds(
    const screensave_saver_session *session,
    int x,
    int y
)
{
    if (session == NULL) {
        return 0;
    }

    return x >= 0 &&
        y >= 0 &&
        x < session->grid_size.width &&
        y < session->grid_size.height;
}

static unsigned char pipeworks_get_cell(
    const screensave_saver_session *session,
    int x,
    int y
)
{
    if (!pipeworks_in_bounds(session, x, y) || session->cells.cells == NULL) {
        return 0U;
    }

    return session->cells.cells[pipeworks_cell_index(session, x, y)];
}

static void pipeworks_write_cell(
    screensave_saver_session *session,
    int x,
    int y,
    unsigned char value
)
{
    unsigned int index;
    unsigned char previous;

    if (!pipeworks_in_bounds(session, x, y) || session->cells.cells == NULL) {
        return;
    }

    index = pipeworks_cell_index(session, x, y);
    previous = session->cells.cells[index];
    if (previous == 0U && value != 0U) {
        session->occupied_cells += 1U;
    } else if (previous != 0U && value == 0U && session->occupied_cells > 0U) {
        session->occupied_cells -= 1U;
    }
    session->cells.cells[index] = value;
}

static void pipeworks_add_connection(
    screensave_saver_session *session,
    int x,
    int y,
    unsigned char value_mask
)
{
    unsigned char value;

    value = pipeworks_get_cell(session, x, y);
    pipeworks_write_cell(session, x, y, (unsigned char)(value | value_mask));
}

static void pipeworks_boost_glow(
    screensave_saver_session *session,
    int x,
    int y,
    unsigned char glow_value
)
{
    unsigned int index;

    if (!pipeworks_in_bounds(session, x, y) || session->glow.cells == NULL) {
        return;
    }

    index = pipeworks_cell_index(session, x, y);
    if (session->glow.cells[index] < glow_value) {
        session->glow.cells[index] = glow_value;
    }
}

static void pipeworks_decay_glow(
    screensave_saver_session *session,
    unsigned int keep_scale
)
{
    unsigned int total_cells;
    unsigned int index;

    if (session == NULL || session->glow.cells == NULL) {
        return;
    }

    total_cells = pipeworks_total_cells(session);
    for (index = 0U; index < total_cells; ++index) {
        session->glow.cells[index] = (unsigned char)(((unsigned int)session->glow.cells[index] * keep_scale) / 255U);
    }
}

static void pipeworks_clear_all_state(screensave_saver_session *session)
{
    unsigned int index;

    if (session == NULL) {
        return;
    }

    screensave_byte_grid_clear(&session->cells, 0U);
    screensave_byte_grid_clear(&session->glow, 0U);
    session->occupied_cells = 0U;
    session->stage_millis = 0UL;
    session->step_accumulator = 0UL;
    session->clear_scan_start = 0U;
    session->clear_stall_ticks = 0U;
    session->stage = PIPEWORKS_STAGE_GROW;
    session->head_count = 0U;
    session->pulse_count = 0U;

    for (index = 0U; index < PIPEWORKS_MAX_HEADS; ++index) {
        session->heads[index].active = 0;
        session->heads[index].age = 0U;
    }
    for (index = 0U; index < PIPEWORKS_MAX_PULSES; ++index) {
        session->pulses[index].active = 0;
        session->pulses[index].life = 0U;
    }
}

static int pipeworks_spawn_head(
    screensave_saver_session *session,
    int x,
    int y,
    int direction
)
{
    unsigned int index;

    if (session == NULL) {
        return 0;
    }

    for (index = 0U; index < PIPEWORKS_MAX_HEADS; ++index) {
        if (!session->heads[index].active) {
            session->heads[index].x = x;
            session->heads[index].y = y;
            session->heads[index].direction = direction;
            session->heads[index].active = 1;
            session->heads[index].age = 0U;
            if (index + 1U > session->head_count) {
                session->head_count = index + 1U;
            }
            return 1;
        }
    }

    return 0;
}

static void pipeworks_seed_heads(screensave_saver_session *session)
{
    int origin_x;
    int origin_y;
    unsigned int desired_heads;
    unsigned int created;
    int used_directions[4];
    unsigned int used_count;
    unsigned int attempt;
    unsigned int index;
    int direction;
    int duplicate;

    if (session == NULL) {
        return;
    }

    origin_x = session->grid_size.width / 2;
    origin_y = session->grid_size.height / 2;
    if (session->grid_size.width > 2) {
        origin_x += (int)pipeworks_rng_range(&session->rng, 3UL) - 1;
    }
    if (session->grid_size.height > 2) {
        origin_y += (int)pipeworks_rng_range(&session->rng, 3UL) - 1;
    }
    if (origin_x < 0) {
        origin_x = 0;
    } else if (origin_x >= session->grid_size.width) {
        origin_x = session->grid_size.width - 1;
    }
    if (origin_y < 0) {
        origin_y = 0;
    } else if (origin_y >= session->grid_size.height) {
        origin_y = session->grid_size.height - 1;
    }

    desired_heads = 2U;
    if (session->config.branch_mode == PIPEWORKS_BRANCH_ORDERLY) {
        desired_heads = 1U;
    } else if (session->config.branch_mode == PIPEWORKS_BRANCH_WILD) {
        desired_heads = 3U;
    }
    if (session->preview_mode && desired_heads > 2U) {
        desired_heads = 2U;
    }

    used_count = 0U;
    created = 0U;
    for (attempt = 0U; attempt < 12U && created < desired_heads; ++attempt) {
        direction = (int)pipeworks_rng_range(&session->rng, 4UL);
        duplicate = 0;
        for (index = 0U; index < used_count; ++index) {
            if (used_directions[index] == direction) {
                duplicate = 1;
                break;
            }
        }
        if (duplicate) {
            continue;
        }
        if (pipeworks_spawn_head(session, origin_x, origin_y, direction)) {
            used_directions[used_count++] = direction;
            created += 1U;
        }
    }

    if (created == 0U) {
        (void)pipeworks_spawn_head(session, origin_x, origin_y, 1);
    }
}

static void pipeworks_reset_world(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    pipeworks_clear_all_state(session);
    pipeworks_seed_heads(session);
}

static unsigned int pipeworks_active_head_count(const screensave_saver_session *session)
{
    unsigned int index;
    unsigned int count;

    if (session == NULL) {
        return 0U;
    }

    count = 0U;
    for (index = 0U; index < session->head_count; ++index) {
        if (session->heads[index].active) {
            count += 1U;
        }
    }

    return count;
}

static unsigned int pipeworks_available_directions(
    const screensave_saver_session *session,
    const pipeworks_head *head,
    int directions_out[4]
)
{
    unsigned int count;
    int direction;
    int next_x;
    int next_y;

    count = 0U;
    if (session == NULL || head == NULL || directions_out == NULL) {
        return 0U;
    }

    for (direction = 0; direction < 4; ++direction) {
        next_x = head->x + g_pipeworks_dir_x[direction];
        next_y = head->y + g_pipeworks_dir_y[direction];
        if (!pipeworks_in_bounds(session, next_x, next_y)) {
            continue;
        }
        if (pipeworks_get_cell(session, next_x, next_y) != 0U) {
            continue;
        }

        directions_out[count++] = direction;
    }

    return count;
}

static int pipeworks_choose_direction(
    screensave_saver_session *session,
    const pipeworks_head *head,
    const int *directions,
    unsigned int direction_count
)
{
    unsigned int index;

    if (session == NULL || head == NULL || directions == NULL || direction_count == 0U) {
        return -1;
    }

    if (direction_count > 1U) {
        for (index = 0U; index < direction_count; ++index) {
            if (
                directions[index] == head->direction &&
                pipeworks_rng_range(&session->rng, 256UL) < pipeworks_continue_chance(session)
            ) {
                return directions[index];
            }
        }
    }

    return directions[pipeworks_rng_range(&session->rng, direction_count)];
}

static void pipeworks_maybe_branch_head(
    screensave_saver_session *session,
    const pipeworks_head *head,
    const int *directions,
    unsigned int direction_count,
    int chosen_direction
)
{
    int branch_directions[3];
    unsigned int branch_count;
    unsigned int index;

    if (
        session == NULL ||
        head == NULL ||
        directions == NULL ||
        direction_count < 2U ||
        pipeworks_rng_range(&session->rng, 256UL) >= pipeworks_branch_chance(session)
    ) {
        return;
    }

    branch_count = 0U;
    for (index = 0U; index < direction_count; ++index) {
        if (directions[index] != chosen_direction) {
            branch_directions[branch_count++] = directions[index];
        }
    }
    if (branch_count == 0U) {
        return;
    }

    (void)pipeworks_spawn_head(
        session,
        head->x,
        head->y,
        branch_directions[pipeworks_rng_range(&session->rng, branch_count)]
    );
}

static void pipeworks_carve_step(
    screensave_saver_session *session,
    pipeworks_head *head,
    int direction
)
{
    int next_x;
    int next_y;

    if (session == NULL || head == NULL || direction < 0 || direction > 3) {
        return;
    }

    next_x = head->x + g_pipeworks_dir_x[direction];
    next_y = head->y + g_pipeworks_dir_y[direction];
    if (!pipeworks_in_bounds(session, next_x, next_y)) {
        head->active = 0;
        return;
    }

    pipeworks_add_connection(session, head->x, head->y, g_pipeworks_dir_mask[direction]);
    pipeworks_add_connection(session, next_x, next_y, g_pipeworks_opposite_mask[direction]);
    pipeworks_boost_glow(session, head->x, head->y, 200U);
    pipeworks_boost_glow(session, next_x, next_y, 255U);

    head->x = next_x;
    head->y = next_y;
    head->direction = direction;
    head->age += 1U;
}

static void pipeworks_step_growth(screensave_saver_session *session)
{
    unsigned int index;
    int directions[4];
    unsigned int direction_count;
    int chosen_direction;
    unsigned int active_heads;
    unsigned int total_cells;
    unsigned int target_fill;
    int progress_made;

    if (session == NULL) {
        return;
    }

    pipeworks_decay_glow(session, 176U);
    progress_made = 0;
    for (index = 0U; index < session->head_count; ++index) {
        if (!session->heads[index].active) {
            continue;
        }

        direction_count = pipeworks_available_directions(session, &session->heads[index], directions);
        if (direction_count == 0U) {
            session->heads[index].active = 0;
            continue;
        }

        chosen_direction = pipeworks_choose_direction(
            session,
            &session->heads[index],
            directions,
            direction_count
        );
        pipeworks_maybe_branch_head(
            session,
            &session->heads[index],
            directions,
            direction_count,
            chosen_direction
        );
        pipeworks_carve_step(session, &session->heads[index], chosen_direction);
        progress_made = 1;
    }

    active_heads = pipeworks_active_head_count(session);
    total_cells = pipeworks_total_cells(session);
    target_fill = (total_cells * pipeworks_target_fill_percent(session)) / 100U;
    if (target_fill == 0U) {
        target_fill = total_cells / 2U;
    }

    if (progress_made) {
        session->clear_stall_ticks = 0U;
    } else {
        session->clear_stall_ticks += 1U;
    }

    if (
        active_heads == 0U ||
        session->occupied_cells >= target_fill ||
        session->stage_millis >= pipeworks_growth_limit(session) ||
        session->clear_stall_ticks > 12U
    ) {
        session->stage = PIPEWORKS_STAGE_FLOW;
        session->stage_millis = 0UL;
        session->pulse_count = 0U;
        session->clear_stall_ticks = 0U;
    }
}

static unsigned int pipeworks_collect_cell_directions(
    unsigned char mask,
    int reverse_direction,
    int directions_out[4]
)
{
    unsigned int count;
    int direction;

    count = 0U;
    for (direction = 0; direction < 4; ++direction) {
        if ((mask & g_pipeworks_dir_mask[direction]) == 0U) {
            continue;
        }
        if (direction == reverse_direction) {
            continue;
        }
        directions_out[count++] = direction;
    }

    if (count == 0U) {
        for (direction = 0; direction < 4; ++direction) {
            if ((mask & g_pipeworks_dir_mask[direction]) != 0U) {
                directions_out[count++] = direction;
            }
        }
    }

    return count;
}

static int pipeworks_spawn_random_pulse(screensave_saver_session *session)
{
    unsigned int total_cells;
    unsigned int start_index;
    unsigned int step;
    unsigned int index;
    unsigned int pulse_index;
    unsigned char mask;
    int directions[4];
    unsigned int direction_count;
    int x;
    int y;

    if (session == NULL || session->pulse_count >= PIPEWORKS_MAX_PULSES) {
        return 0;
    }

    total_cells = pipeworks_total_cells(session);
    if (total_cells == 0U || session->cells.cells == NULL) {
        return 0;
    }

    start_index = (unsigned int)pipeworks_rng_range(&session->rng, total_cells);
    for (step = 0U; step < total_cells; ++step) {
        index = (start_index + step) % total_cells;
        mask = session->cells.cells[index];
        if (mask == 0U) {
            continue;
        }

        direction_count = pipeworks_collect_cell_directions(mask, -1, directions);
        if (direction_count == 0U) {
            continue;
        }

        for (pulse_index = 0U; pulse_index < PIPEWORKS_MAX_PULSES; ++pulse_index) {
            if (!session->pulses[pulse_index].active) {
                x = (int)(index % (unsigned int)session->grid_size.width);
                y = (int)(index / (unsigned int)session->grid_size.width);
                session->pulses[pulse_index].x = x;
                session->pulses[pulse_index].y = y;
                session->pulses[pulse_index].direction =
                    directions[pipeworks_rng_range(&session->rng, direction_count)];
                session->pulses[pulse_index].life = 10U + (unsigned int)pipeworks_rng_range(&session->rng, 14UL);
                session->pulses[pulse_index].active = 1;
                if (pulse_index + 1U > session->pulse_count) {
                    session->pulse_count = pulse_index + 1U;
                }
                pipeworks_boost_glow(session, x, y, 255U);
                return 1;
            }
        }
    }

    return 0;
}

static unsigned int pipeworks_target_pulses(const screensave_saver_session *session)
{
    unsigned int count;

    count = 4U;
    if (session == NULL) {
        return count;
    }

    if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW) {
        count = 3U;
    } else if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH) {
        count = 6U;
    }

    if (session->preview_mode && count > 3U) {
        count = 3U;
    }

    return count;
}

static void pipeworks_advance_pulse(
    screensave_saver_session *session,
    pipeworks_pulse *pulse
)
{
    unsigned char mask;
    int reverse_direction;
    int directions[4];
    unsigned int direction_count;
    int direction;
    int next_x;
    int next_y;

    if (session == NULL || pulse == NULL || !pulse->active) {
        return;
    }

    pipeworks_boost_glow(session, pulse->x, pulse->y, 255U);
    mask = pipeworks_get_cell(session, pulse->x, pulse->y);
    reverse_direction = pulse->direction >= 0 ? (pulse->direction + 2) % 4 : -1;
    direction_count = pipeworks_collect_cell_directions(mask, reverse_direction, directions);
    if (direction_count == 0U) {
        pulse->active = 0;
        pulse->life = 0U;
        return;
    }

    direction = directions[pipeworks_rng_range(&session->rng, direction_count)];
    next_x = pulse->x + g_pipeworks_dir_x[direction];
    next_y = pulse->y + g_pipeworks_dir_y[direction];
    if (!pipeworks_in_bounds(session, next_x, next_y)) {
        pulse->active = 0;
        pulse->life = 0U;
        return;
    }

    pulse->x = next_x;
    pulse->y = next_y;
    pulse->direction = direction;
    pipeworks_boost_glow(session, pulse->x, pulse->y, 255U);
    if (pulse->life > 0U) {
        pulse->life -= 1U;
    }
    if (pulse->life == 0U) {
        pulse->active = 0;
    }
}

static void pipeworks_step_flow(screensave_saver_session *session)
{
    unsigned int index;
    unsigned int active_count;
    unsigned int target_pulses;

    if (session == NULL) {
        return;
    }

    pipeworks_decay_glow(session, 228U);
    target_pulses = pipeworks_target_pulses(session);
    active_count = 0U;

    for (index = 0U; index < session->pulse_count; ++index) {
        if (!session->pulses[index].active) {
            continue;
        }

        pipeworks_advance_pulse(session, &session->pulses[index]);
        if (session->pulses[index].active) {
            active_count += 1U;
        }
    }

    while (active_count < target_pulses && pipeworks_spawn_random_pulse(session)) {
        active_count += 1U;
    }

    if (session->stage_millis >= pipeworks_flow_limit(session)) {
        session->stage = PIPEWORKS_STAGE_CLEAR;
        session->stage_millis = 0UL;
        session->clear_scan_start = 0U;
    }
}

static unsigned int pipeworks_clear_batch_size(const screensave_saver_session *session)
{
    unsigned int batch_size;

    batch_size = 18U;
    if (session == NULL) {
        return batch_size;
    }

    if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_LOW) {
        batch_size = 12U;
    } else if (session->detail_level == SCREENSAVE_DETAIL_LEVEL_HIGH) {
        batch_size = 28U;
    }

    if (session->config.speed_mode == PIPEWORKS_SPEED_BRISK) {
        batch_size += 8U;
    } else if (session->config.speed_mode == PIPEWORKS_SPEED_PATIENT && batch_size > 4U) {
        batch_size -= 4U;
    }

    if (session->preview_mode && batch_size > 10U) {
        batch_size = 10U;
    }

    return batch_size;
}

static void pipeworks_step_clear(screensave_saver_session *session)
{
    unsigned int batch_size;
    unsigned int total_cells;
    unsigned int removed;
    unsigned int index;

    if (session == NULL) {
        return;
    }

    pipeworks_decay_glow(session, 148U);
    batch_size = pipeworks_clear_batch_size(session);
    total_cells = pipeworks_total_cells(session);
    removed = 0U;

    while (session->clear_scan_start < total_cells && removed < batch_size) {
        index = session->clear_scan_start;
        if (session->cells.cells != NULL && session->cells.cells[index] != 0U) {
            if (session->occupied_cells > 0U) {
                session->occupied_cells -= 1U;
            }
            session->cells.cells[index] = 0U;
            if (session->glow.cells != NULL) {
                session->glow.cells[index] = 0U;
            }
            removed += 1U;
        }
        session->clear_scan_start += 1U;
    }

    if (session->clear_scan_start >= total_cells || session->occupied_cells == 0U) {
        pipeworks_reset_world(session);
    }
}

static int pipeworks_resize_buffers(
    screensave_saver_session *session,
    const screensave_sizei *drawable_size
)
{
    screensave_sizei grid_size;

    if (session == NULL || drawable_size == NULL) {
        return 0;
    }

    session->drawable_size = *drawable_size;
    session->cell_size = pipeworks_compute_cell_size(session, drawable_size);
    grid_size.width = drawable_size->width / session->cell_size;
    grid_size.height = drawable_size->height / session->cell_size;
    if (grid_size.width < 1) {
        grid_size.width = 1;
    }
    if (grid_size.height < 1) {
        grid_size.height = 1;
    }

    if (
        !screensave_byte_grid_resize(&session->cells, &grid_size) ||
        !screensave_byte_grid_resize(&session->glow, &grid_size)
    ) {
        return 0;
    }

    session->grid_size = grid_size;
    pipeworks_reset_world(session);
    return 1;
}

static void pipeworks_copy_bound_config(
    screensave_saver_session *session,
    const screensave_saver_environment *environment
)
{
    const screensave_config_binding *binding;

    if (session == NULL) {
        return;
    }

    session->config.density_mode = PIPEWORKS_DENSITY_STANDARD;
    session->config.speed_mode = PIPEWORKS_SPEED_STANDARD;
    session->config.branch_mode = PIPEWORKS_BRANCH_BALANCED;
    session->config.rebuild_mode = PIPEWORKS_REBUILD_CYCLE;
    session->detail_level = SCREENSAVE_DETAIL_LEVEL_STANDARD;

    binding = environment != NULL ? environment->config_binding : NULL;
    if (
        binding != NULL &&
        binding->product_config != NULL &&
        binding->product_config_size == sizeof(pipeworks_config)
    ) {
        session->config = *(const pipeworks_config *)binding->product_config;
    }
    if (binding != NULL && binding->common_config != NULL) {
        session->detail_level = binding->common_config->detail_level;
    }

    if (session->config.density_mode < PIPEWORKS_DENSITY_TIGHT || session->config.density_mode > PIPEWORKS_DENSITY_OPEN) {
        session->config.density_mode = PIPEWORKS_DENSITY_STANDARD;
    }
    if (session->config.speed_mode < PIPEWORKS_SPEED_PATIENT || session->config.speed_mode > PIPEWORKS_SPEED_BRISK) {
        session->config.speed_mode = PIPEWORKS_SPEED_STANDARD;
    }
    if (session->config.branch_mode < PIPEWORKS_BRANCH_ORDERLY || session->config.branch_mode > PIPEWORKS_BRANCH_WILD) {
        session->config.branch_mode = PIPEWORKS_BRANCH_BALANCED;
    }
    if (
        session->config.rebuild_mode < PIPEWORKS_REBUILD_PATIENT ||
        session->config.rebuild_mode > PIPEWORKS_REBUILD_FREQUENT
    ) {
        session->config.rebuild_mode = PIPEWORKS_REBUILD_CYCLE;
    }
}

int pipeworks_create_session(
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

    pipeworks_copy_bound_config(session, environment);
    session->preview_mode = environment->mode == SCREENSAVE_SESSION_MODE_PREVIEW;
    session->theme = pipeworks_resolve_theme(environment);
    pipeworks_rng_seed(&session->rng, environment->seed.stream_seed ^ 0x50777057UL);

    if (!pipeworks_resize_buffers(session, &environment->drawable_size)) {
        pipeworks_destroy_session(session);
        return 0;
    }

    *session_out = session;
    return 1;
}

void pipeworks_destroy_session(screensave_saver_session *session)
{
    if (session == NULL) {
        return;
    }

    screensave_byte_grid_dispose(&session->cells);
    screensave_byte_grid_dispose(&session->glow);
    free(session);
}

void pipeworks_resize_session(
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
    session->theme = pipeworks_resolve_theme(environment);
    (void)pipeworks_resize_buffers(session, &environment->drawable_size);
}

void pipeworks_step_session(
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

    session->theme = pipeworks_resolve_theme(environment);
    session->step_accumulator += environment->clock.delta_millis;
    session->stage_millis += environment->clock.delta_millis;

    step_interval = pipeworks_step_interval(session);
    iteration_count = 0U;
    while (session->step_accumulator >= step_interval && iteration_count < 8U) {
        if (session->stage == PIPEWORKS_STAGE_GROW) {
            pipeworks_step_growth(session);
        } else if (session->stage == PIPEWORKS_STAGE_FLOW) {
            pipeworks_step_flow(session);
        } else {
            pipeworks_step_clear(session);
        }

        session->step_accumulator -= step_interval;
        iteration_count += 1U;
    }
}
