#include <string.h>

#include "plasma_internal.h"

static int plasma_text_copy(char *buffer, unsigned int buffer_size, const char *text)
{
    unsigned int text_length;

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    if (text == NULL) {
        return 1;
    }

    text_length = (unsigned int)lstrlenA(text);
    if (text_length + 1U > buffer_size) {
        return 0;
    }

    lstrcpyA(buffer, text);
    return 1;
}

static char *plasma_trim_text(char *text)
{
    char *end;

    if (text == NULL) {
        return NULL;
    }

    while (*text != '\0' && (*text == ' ' || *text == '\t' || *text == '\r' || *text == '\n')) {
        ++text;
    }

    end = text + lstrlenA(text);
    while (end > text) {
        if (end[-1] != ' ' && end[-1] != '\t' && end[-1] != '\r' && end[-1] != '\n') {
            break;
        }
        --end;
    }

    *end = '\0';
    return text;
}

static int plasma_append_list_token(
    char *buffer,
    unsigned int buffer_size,
    const char *token,
    int *used
)
{
    unsigned int token_length;

    if (buffer == NULL || token == NULL || used == NULL) {
        return 0;
    }

    token_length = (unsigned int)lstrlenA(token);
    if (*used > 0) {
        if ((unsigned int)(*used + 1) >= buffer_size) {
            return 0;
        }
        buffer[*used] = ',';
        buffer[*used + 1] = '\0';
        *used += 1;
    }

    if ((unsigned int)(*used + (int)token_length + 1) > buffer_size) {
        return 0;
    }

    lstrcpyA(buffer + *used, token);
    *used += (int)token_length;
    return 1;
}

static unsigned long plasma_parse_preset_key_mask(const char *text)
{
    char buffer[PLASMA_CONTENT_KEY_LIST_LENGTH];
    char *cursor;
    unsigned long mask;

    if (text == NULL || text[0] == '\0' || !plasma_text_copy(buffer, sizeof(buffer), text)) {
        return 0UL;
    }

    cursor = plasma_trim_text(buffer);
    if (cursor == NULL || cursor[0] == '\0' || lstrcmpiA(cursor, "none") == 0) {
        return 0UL;
    }

    mask = 0UL;
    while (*cursor != '\0') {
        char *token;
        const plasma_content_preset_entry *entry;
        int index;

        token = cursor;
        while (*cursor != '\0' && *cursor != ',') {
            ++cursor;
        }
        if (*cursor == ',') {
            *cursor = '\0';
            ++cursor;
        }

        token = plasma_trim_text(token);
        if (token[0] == '\0') {
            continue;
        }

        entry = plasma_content_find_preset_entry(token);
        index = plasma_content_preset_index(entry);
        if (index >= 0) {
            mask |= (1UL << index);
        }
    }

    return mask;
}

static unsigned long plasma_parse_theme_key_mask(const char *text)
{
    char buffer[PLASMA_CONTENT_KEY_LIST_LENGTH];
    char *cursor;
    unsigned long mask;

    if (text == NULL || text[0] == '\0' || !plasma_text_copy(buffer, sizeof(buffer), text)) {
        return 0UL;
    }

    cursor = plasma_trim_text(buffer);
    if (cursor == NULL || cursor[0] == '\0' || lstrcmpiA(cursor, "none") == 0) {
        return 0UL;
    }

    mask = 0UL;
    while (*cursor != '\0') {
        char *token;
        const plasma_content_theme_entry *entry;
        int index;

        token = cursor;
        while (*cursor != '\0' && *cursor != ',') {
            ++cursor;
        }
        if (*cursor == ',') {
            *cursor = '\0';
            ++cursor;
        }

        token = plasma_trim_text(token);
        if (token[0] == '\0') {
            continue;
        }

        entry = plasma_content_find_theme_entry(token);
        index = plasma_content_theme_index(entry);
        if (index >= 0) {
            mask |= (1UL << index);
        }
    }

    return mask;
}

static int plasma_build_preset_key_list(
    char *buffer,
    unsigned int buffer_size,
    unsigned long mask
)
{
    const plasma_content_registry *registry;
    unsigned int index;
    int used;

    registry = plasma_content_get_registry();
    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    if (mask == 0UL) {
        return plasma_text_copy(buffer, buffer_size, "none");
    }

    used = 0;
    for (index = 0U; index < registry->preset_count; ++index) {
        if ((mask & (1UL << index)) == 0UL) {
            continue;
        }
        if (!plasma_append_list_token(buffer, buffer_size, registry->preset_entries[index].preset_key, &used)) {
            return 0;
        }
    }

    return 1;
}

static int plasma_build_theme_key_list(
    char *buffer,
    unsigned int buffer_size,
    unsigned long mask
)
{
    const plasma_content_registry *registry;
    unsigned int index;
    int used;

    registry = plasma_content_get_registry();
    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    if (mask == 0UL) {
        return plasma_text_copy(buffer, buffer_size, "none");
    }

    used = 0;
    for (index = 0U; index < registry->theme_count; ++index) {
        if ((mask & (1UL << index)) == 0UL) {
            continue;
        }
        if (!plasma_append_list_token(buffer, buffer_size, registry->theme_entries[index].theme_key, &used)) {
            return 0;
        }
    }

    return 1;
}

static int plasma_filter_allows_channel(
    plasma_content_filter filter,
    plasma_content_channel channel
)
{
    switch (filter) {
    case PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL:
        return 1;

    case PLASMA_CONTENT_FILTER_EXPERIMENTAL_ONLY:
        return channel == PLASMA_CONTENT_CHANNEL_EXPERIMENTAL;

    case PLASMA_CONTENT_FILTER_STABLE_ONLY:
    default:
        return channel == PLASMA_CONTENT_CHANNEL_STABLE;
    }
}

static int plasma_preset_mask_has_candidates(
    unsigned long favorite_mask,
    unsigned long excluded_mask,
    const plasma_content_preset_set_entry *active_set,
    plasma_content_filter filter
)
{
    const plasma_content_registry *registry;
    unsigned int index;

    registry = plasma_content_get_registry();
    for (index = 0U; index < registry->preset_count; ++index) {
        const plasma_content_preset_entry *entry;

        if ((favorite_mask & (1UL << index)) == 0UL || (excluded_mask & (1UL << index)) != 0UL) {
            continue;
        }
        entry = &registry->preset_entries[index];
        if (
            plasma_filter_allows_channel(filter, entry->channel) &&
            (active_set == NULL || plasma_content_preset_in_set(active_set, entry))
        ) {
            return 1;
        }
    }

    return 0;
}

static int plasma_theme_mask_has_candidates(
    unsigned long favorite_mask,
    unsigned long excluded_mask,
    const plasma_content_theme_set_entry *active_set,
    plasma_content_filter filter
)
{
    const plasma_content_registry *registry;
    unsigned int index;

    registry = plasma_content_get_registry();
    for (index = 0U; index < registry->theme_count; ++index) {
        const plasma_content_theme_entry *entry;

        if ((favorite_mask & (1UL << index)) == 0UL || (excluded_mask & (1UL << index)) != 0UL) {
            continue;
        }
        entry = &registry->theme_entries[index];
        if (
            plasma_filter_allows_channel(filter, entry->channel) &&
            (active_set == NULL || plasma_content_theme_in_set(active_set, entry))
        ) {
            return 1;
        }
    }

    return 0;
}

static int plasma_preset_matches_filters(
    const plasma_content_preset_entry *entry,
    const plasma_content_preset_set_entry *active_set,
    plasma_content_filter filter,
    unsigned long favorite_mask,
    unsigned long excluded_mask,
    int favorites_only_requested,
    int *favorites_only_applied_out
)
{
    int index;
    int favorites_only_applied;

    if (entry == NULL) {
        return 0;
    }

    index = plasma_content_preset_index(entry);
    if (index < 0) {
        return 0;
    }

    favorites_only_applied = favorites_only_requested &&
        plasma_preset_mask_has_candidates(favorite_mask, excluded_mask, active_set, filter);
    if (favorites_only_applied_out != NULL) {
        *favorites_only_applied_out = favorites_only_applied;
    }

    if (
        !plasma_filter_allows_channel(filter, entry->channel) ||
        (active_set != NULL && !plasma_content_preset_in_set(active_set, entry)) ||
        (excluded_mask & (1UL << index)) != 0UL
    ) {
        return 0;
    }

    if (favorites_only_applied && (favorite_mask & (1UL << index)) == 0UL) {
        return 0;
    }

    return 1;
}

static int plasma_theme_matches_filters(
    const plasma_content_theme_entry *entry,
    const plasma_content_theme_set_entry *active_set,
    plasma_content_filter filter,
    unsigned long favorite_mask,
    unsigned long excluded_mask,
    int favorites_only_requested,
    int *favorites_only_applied_out
)
{
    int index;
    int favorites_only_applied;

    if (entry == NULL) {
        return 0;
    }

    index = plasma_content_theme_index(entry);
    if (index < 0) {
        return 0;
    }

    favorites_only_applied = favorites_only_requested &&
        plasma_theme_mask_has_candidates(favorite_mask, excluded_mask, active_set, filter);
    if (favorites_only_applied_out != NULL) {
        *favorites_only_applied_out = favorites_only_applied;
    }

    if (
        !plasma_filter_allows_channel(filter, entry->channel) ||
        (active_set != NULL && !plasma_content_theme_in_set(active_set, entry)) ||
        (excluded_mask & (1UL << index)) != 0UL
    ) {
        return 0;
    }

    if (favorites_only_applied && (favorite_mask & (1UL << index)) == 0UL) {
        return 0;
    }

    return 1;
}

static const plasma_content_preset_entry *plasma_select_fallback_preset(
    const plasma_content_preset_set_entry *active_set,
    plasma_content_filter filter,
    unsigned long favorite_mask,
    unsigned long excluded_mask,
    int favorites_only_requested,
    int *favorites_only_applied_out
)
{
    const plasma_content_registry *registry;
    unsigned int index;

    registry = plasma_content_get_registry();
    if (favorites_only_applied_out != NULL) {
        *favorites_only_applied_out = 0;
    }

    if (active_set != NULL) {
        for (index = 0U; index < active_set->member_count; ++index) {
            const plasma_content_preset_entry *entry;
            int applied;

            entry = plasma_content_find_preset_entry(active_set->members[index].content_key);
            if (
                plasma_preset_matches_filters(
                    entry,
                    active_set,
                    filter,
                    favorite_mask,
                    excluded_mask,
                    favorites_only_requested,
                    &applied
                )
            ) {
                if (favorites_only_applied_out != NULL) {
                    *favorites_only_applied_out = applied;
                }
                return entry;
            }
        }
    }

    for (index = 0U; index < registry->preset_count; ++index) {
        const plasma_content_preset_entry *entry;
        int applied;

        entry = &registry->preset_entries[index];
        if (
            plasma_preset_matches_filters(
                entry,
                active_set,
                filter,
                favorite_mask,
                excluded_mask,
                favorites_only_requested,
                &applied
            )
        ) {
            if (favorites_only_applied_out != NULL) {
                *favorites_only_applied_out = applied;
            }
            return entry;
        }
    }

    return plasma_content_find_preset_entry(PLASMA_DEFAULT_PRESET_KEY);
}

static const plasma_content_theme_entry *plasma_select_fallback_theme(
    const plasma_content_theme_set_entry *active_set,
    plasma_content_filter filter,
    unsigned long favorite_mask,
    unsigned long excluded_mask,
    int favorites_only_requested,
    const char *preferred_theme_key,
    int *favorites_only_applied_out
)
{
    const plasma_content_theme_entry *entry;
    const plasma_content_registry *registry;
    unsigned int index;

    entry = plasma_content_find_theme_entry(preferred_theme_key);
    if (
        plasma_theme_matches_filters(
            entry,
            active_set,
            filter,
            favorite_mask,
            excluded_mask,
            favorites_only_requested,
            favorites_only_applied_out
        )
    ) {
        return entry;
    }

    registry = plasma_content_get_registry();
    if (favorites_only_applied_out != NULL) {
        *favorites_only_applied_out = 0;
    }

    if (active_set != NULL) {
        for (index = 0U; index < active_set->member_count; ++index) {
            int applied;

            entry = plasma_content_find_theme_entry(active_set->members[index].content_key);
            if (
                plasma_theme_matches_filters(
                    entry,
                    active_set,
                    filter,
                    favorite_mask,
                    excluded_mask,
                    favorites_only_requested,
                    &applied
                )
            ) {
                if (favorites_only_applied_out != NULL) {
                    *favorites_only_applied_out = applied;
                }
                return entry;
            }
        }
    }

    for (index = 0U; index < registry->theme_count; ++index) {
        int applied;

        entry = &registry->theme_entries[index];
        if (
            plasma_theme_matches_filters(
                entry,
                active_set,
                filter,
                favorite_mask,
                excluded_mask,
                favorites_only_requested,
                &applied
            )
        ) {
            if (favorites_only_applied_out != NULL) {
                *favorites_only_applied_out = applied;
            }
            return entry;
        }
    }

    return plasma_content_find_theme_entry(PLASMA_DEFAULT_THEME_KEY);
}

const char *plasma_selection_content_filter_name(plasma_content_filter filter)
{
    switch (filter) {
    case PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL:
        return "stable_and_experimental";

    case PLASMA_CONTENT_FILTER_EXPERIMENTAL_ONLY:
        return "experimental_only";

    case PLASMA_CONTENT_FILTER_STABLE_ONLY:
    default:
        return "stable_only";
    }
}

int plasma_selection_parse_content_filter(const char *text, plasma_content_filter *filter_out)
{
    if (text == NULL || filter_out == NULL) {
        return 0;
    }

    if (lstrcmpiA(text, "stable_only") == 0) {
        *filter_out = PLASMA_CONTENT_FILTER_STABLE_ONLY;
        return 1;
    }
    if (lstrcmpiA(text, "stable_and_experimental") == 0) {
        *filter_out = PLASMA_CONTENT_FILTER_STABLE_AND_EXPERIMENTAL;
        return 1;
    }
    if (lstrcmpiA(text, "experimental_only") == 0) {
        *filter_out = PLASMA_CONTENT_FILTER_EXPERIMENTAL_ONLY;
        return 1;
    }

    return 0;
}

void plasma_selection_preferences_set_defaults(plasma_selection_preferences *preferences)
{
    if (preferences == NULL) {
        return;
    }

    ZeroMemory(preferences, sizeof(*preferences));
    preferences->content_filter = PLASMA_CONTENT_FILTER_STABLE_ONLY;
    preferences->favorites_only = 0;
    lstrcpyA(preferences->favorite_preset_keys, "none");
    lstrcpyA(preferences->excluded_preset_keys, "none");
    lstrcpyA(preferences->favorite_theme_keys, "none");
    lstrcpyA(preferences->excluded_theme_keys, "none");
}

void plasma_selection_preferences_clamp(plasma_selection_preferences *preferences)
{
    unsigned long mask;

    if (preferences == NULL) {
        return;
    }

    if (
        preferences->content_filter < PLASMA_CONTENT_FILTER_STABLE_ONLY ||
        preferences->content_filter > PLASMA_CONTENT_FILTER_EXPERIMENTAL_ONLY
    ) {
        preferences->content_filter = PLASMA_CONTENT_FILTER_STABLE_ONLY;
    }
    if (
        preferences->content_filter == PLASMA_CONTENT_FILTER_EXPERIMENTAL_ONLY &&
        !plasma_content_registry_has_channel(PLASMA_CONTENT_CHANNEL_EXPERIMENTAL)
    ) {
        preferences->content_filter = PLASMA_CONTENT_FILTER_STABLE_ONLY;
    }

    preferences->favorites_only = preferences->favorites_only ? 1 : 0;

    if (plasma_content_find_preset_set(preferences->preset_set_key) == NULL) {
        preferences->preset_set_key[0] = '\0';
    }
    if (plasma_content_find_theme_set(preferences->theme_set_key) == NULL) {
        preferences->theme_set_key[0] = '\0';
    }

    mask = plasma_parse_preset_key_mask(preferences->favorite_preset_keys);
    if (!plasma_build_preset_key_list(preferences->favorite_preset_keys, sizeof(preferences->favorite_preset_keys), mask)) {
        lstrcpyA(preferences->favorite_preset_keys, "none");
    }
    mask = plasma_parse_preset_key_mask(preferences->excluded_preset_keys);
    if (!plasma_build_preset_key_list(preferences->excluded_preset_keys, sizeof(preferences->excluded_preset_keys), mask)) {
        lstrcpyA(preferences->excluded_preset_keys, "none");
    }
    mask = plasma_parse_theme_key_mask(preferences->favorite_theme_keys);
    if (!plasma_build_theme_key_list(preferences->favorite_theme_keys, sizeof(preferences->favorite_theme_keys), mask)) {
        lstrcpyA(preferences->favorite_theme_keys, "none");
    }
    mask = plasma_parse_theme_key_mask(preferences->excluded_theme_keys);
    if (!plasma_build_theme_key_list(preferences->excluded_theme_keys, sizeof(preferences->excluded_theme_keys), mask)) {
        lstrcpyA(preferences->excluded_theme_keys, "none");
    }
}

int plasma_selection_resolve(
    plasma_selection_state *state,
    screensave_common_config *common_config,
    plasma_selection_preferences *preferences
)
{
    const plasma_content_preset_entry *requested_preset;
    const plasma_content_theme_entry *requested_theme;
    int favorites_applied;

    if (
        state == NULL ||
        common_config == NULL ||
        preferences == NULL ||
        !plasma_content_registry_validate()
    ) {
        return 0;
    }

    ZeroMemory(state, sizeof(*state));
    plasma_selection_preferences_clamp(preferences);

    state->content_filter = preferences->content_filter;
    state->favorites_only_requested = preferences->favorites_only;
    state->active_preset_set = plasma_content_find_preset_set(preferences->preset_set_key);
    state->active_theme_set = plasma_content_find_theme_set(preferences->theme_set_key);
    state->favorite_preset_mask = plasma_parse_preset_key_mask(preferences->favorite_preset_keys);
    state->excluded_preset_mask = plasma_parse_preset_key_mask(preferences->excluded_preset_keys);
    state->favorite_theme_mask = plasma_parse_theme_key_mask(preferences->favorite_theme_keys);
    state->excluded_theme_mask = plasma_parse_theme_key_mask(preferences->excluded_theme_keys);

    requested_preset = plasma_content_find_preset_entry(common_config->preset_key);
    if (requested_preset == NULL) {
        requested_preset = plasma_content_find_preset_entry(PLASMA_DEFAULT_PRESET_KEY);
    }
    if (
        !plasma_preset_matches_filters(
            requested_preset,
            state->active_preset_set,
            state->content_filter,
            state->favorite_preset_mask,
            state->excluded_preset_mask,
            state->favorites_only_requested,
            &favorites_applied
        )
    ) {
        requested_preset = plasma_select_fallback_preset(
            state->active_preset_set,
            state->content_filter,
            state->favorite_preset_mask,
            state->excluded_preset_mask,
            state->favorites_only_requested,
            &favorites_applied
        );
    }
    state->selected_preset = requested_preset;
    state->favorites_only_applied = favorites_applied;

    requested_theme = plasma_content_find_theme_entry(common_config->theme_key);
    if (requested_theme == NULL && state->selected_preset != NULL && state->selected_preset->descriptor != NULL) {
        requested_theme = plasma_content_find_theme_entry(state->selected_preset->descriptor->theme_key);
    }
    if (
        !plasma_theme_matches_filters(
            requested_theme,
            state->active_theme_set,
            state->content_filter,
            state->favorite_theme_mask,
            state->excluded_theme_mask,
            state->favorites_only_requested,
            &favorites_applied
        )
    ) {
        requested_theme = plasma_select_fallback_theme(
            state->active_theme_set,
            state->content_filter,
            state->favorite_theme_mask,
            state->excluded_theme_mask,
            state->favorites_only_requested,
            state->selected_preset != NULL && state->selected_preset->descriptor != NULL
                ? state->selected_preset->descriptor->theme_key
                : PLASMA_DEFAULT_THEME_KEY,
            &favorites_applied
        );
    }
    state->selected_theme = requested_theme;
    if (favorites_applied) {
        state->favorites_only_applied = 1;
    }

    if (!plasma_selection_state_validate(state)) {
        return 0;
    }

    common_config->preset_key = state->selected_preset->preset_key;
    common_config->theme_key = state->selected_theme->theme_key;
    return 1;
}

int plasma_selection_state_validate(const plasma_selection_state *state)
{
    int preset_index;
    int theme_index;

    if (
        state == NULL ||
        state->selected_preset == NULL ||
        state->selected_theme == NULL ||
        state->selected_preset->descriptor == NULL ||
        state->selected_theme->descriptor == NULL
    ) {
        return 0;
    }

    preset_index = plasma_content_preset_index(state->selected_preset);
    theme_index = plasma_content_theme_index(state->selected_theme);
    if (preset_index < 0 || theme_index < 0) {
        return 0;
    }

    if (
        !plasma_filter_allows_channel(state->content_filter, state->selected_preset->channel) ||
        !plasma_filter_allows_channel(state->content_filter, state->selected_theme->channel)
    ) {
        return 0;
    }

    if (
        state->active_preset_set != NULL &&
        !plasma_content_preset_in_set(state->active_preset_set, state->selected_preset)
    ) {
        return 0;
    }
    if (
        state->active_theme_set != NULL &&
        !plasma_content_theme_in_set(state->active_theme_set, state->selected_theme)
    ) {
        return 0;
    }

    if (
        (state->excluded_preset_mask & (1UL << preset_index)) != 0UL ||
        (state->excluded_theme_mask & (1UL << theme_index)) != 0UL
    ) {
        return 0;
    }

    return 1;
}
