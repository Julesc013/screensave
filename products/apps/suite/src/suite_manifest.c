#include <stdio.h>
#include <string.h>

#include "../../../../platform/src/core/base/ini_file.h"
#include "../../../../platform/src/core/base/saver_registry.h"
#include "suite_internal.h"

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
const screensave_saver_module *anthology_get_module(void);

typedef const screensave_saver_module *(*suite_module_getter)(void);

typedef struct suite_manifest_load_context_tag {
    suite_manifest_info *manifest;
} suite_manifest_load_context;

static const suite_module_getter g_suite_module_getters[] = {
    nocturne_get_module,
    ricochet_get_module,
    deepfield_get_module,
    plasma_get_module,
    phosphor_get_module,
    pipeworks_get_module,
    lifeforms_get_module,
    signals_get_module,
    mechanize_get_module,
    ecosystems_get_module,
    stormglass_get_module,
    transit_get_module,
    observatory_get_module,
    vector_get_module,
    explorer_get_module,
    city_get_module,
    atlas_get_module,
    gallery_get_module,
    anthology_get_module
};

static int suite_text_copy(char *buffer, unsigned int buffer_size, const char *text)
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

static int suite_append_text(char *buffer, unsigned int buffer_size, const char *text)
{
    unsigned int used;
    unsigned int added;

    if (buffer == NULL || text == NULL || buffer_size == 0U) {
        return 0;
    }

    used = (unsigned int)lstrlenA(buffer);
    added = (unsigned int)lstrlenA(text);
    if (used + added + 1U > buffer_size) {
        return 0;
    }

    lstrcpyA(buffer + used, text);
    return 1;
}

static int suite_file_exists(const char *path)
{
    DWORD attributes;

    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    attributes = GetFileAttributesA(path);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0U;
}

static int suite_directory_exists(const char *path)
{
    DWORD attributes;

    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    attributes = GetFileAttributesA(path);
    return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0U;
}

static int suite_join_path(
    const char *base_path,
    const char *leaf_name,
    char *buffer,
    unsigned int buffer_size
)
{
    char base_copy[SUITE_PATH_LENGTH];
    unsigned int base_length;
    unsigned int leaf_length;
    int needs_separator;

    if (
        base_path == NULL ||
        leaf_name == NULL ||
        buffer == NULL ||
        buffer_size == 0U ||
        !suite_text_copy(base_copy, sizeof(base_copy), base_path)
    ) {
        return 0;
    }

    base_length = (unsigned int)lstrlenA(base_copy);
    leaf_length = (unsigned int)lstrlenA(leaf_name);
    needs_separator = base_length > 0U &&
        base_copy[base_length - 1U] != '\\' &&
        base_copy[base_length - 1U] != '/';

    if (base_length + leaf_length + (needs_separator ? 2U : 1U) > buffer_size) {
        buffer[0] = '\0';
        return 0;
    }

    lstrcpyA(buffer, base_copy);
    if (needs_separator) {
        lstrcatA(buffer, "\\");
    }
    lstrcatA(buffer, leaf_name);
    return 1;
}

static int suite_trim_to_parent(char *path)
{
    char *cursor;

    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    cursor = path + lstrlenA(path);
    while (cursor > path) {
        if (cursor[-1] == '\\' || cursor[-1] == '/') {
            if (cursor - 1 == path && path[1] == ':') {
                return 0;
            }
            cursor[-1] = '\0';
            return path[0] != '\0';
        }
        --cursor;
    }

    return 0;
}

static int suite_copy_parent_directory(const char *path, char *buffer, unsigned int buffer_size)
{
    if (
        path == NULL ||
        buffer == NULL ||
        !suite_text_copy(buffer, buffer_size, path)
    ) {
        return 0;
    }

    return suite_trim_to_parent(buffer);
}

static int suite_looks_like_repo_root(const char *candidate_root)
{
    char test_path[SUITE_PATH_LENGTH];

    if (!suite_join_path(candidate_root, "products\\savers", test_path, sizeof(test_path))) {
        return 0;
    }
    if (!suite_directory_exists(test_path)) {
        return 0;
    }

    if (!suite_join_path(candidate_root, "platform\\include\\screensave", test_path, sizeof(test_path))) {
        return 0;
    }

    return suite_directory_exists(test_path);
}

static int suite_find_repo_root_from(const char *start_path, char *buffer, unsigned int buffer_size)
{
    char candidate[SUITE_PATH_LENGTH];
    unsigned int depth;

    if (
        start_path == NULL ||
        buffer == NULL ||
        !suite_text_copy(candidate, sizeof(candidate), start_path)
    ) {
        return 0;
    }

    for (depth = 0U; depth < 12U; ++depth) {
        if (suite_looks_like_repo_root(candidate)) {
            return suite_text_copy(buffer, buffer_size, candidate);
        }
        if (!suite_trim_to_parent(candidate)) {
            break;
        }
    }

    return 0;
}

static int suite_detect_repo_root(char *buffer, unsigned int buffer_size)
{
    char module_path[SUITE_PATH_LENGTH];
    char current_directory[SUITE_PATH_LENGTH];

    if (buffer == NULL || buffer_size == 0U) {
        return 0;
    }

    buffer[0] = '\0';
    module_path[0] = '\0';
    if (
        GetModuleFileNameA(NULL, module_path, (DWORD)sizeof(module_path)) > 0U &&
        suite_copy_parent_directory(module_path, current_directory, sizeof(current_directory)) &&
        suite_find_repo_root_from(current_directory, buffer, buffer_size)
    ) {
        return 1;
    }

    current_directory[0] = '\0';
    if (
        GetCurrentDirectoryA((DWORD)sizeof(current_directory), current_directory) > 0U &&
        suite_find_repo_root_from(current_directory, buffer, buffer_size)
    ) {
        return 1;
    }

    return 0;
}

static void suite_manifest_set_defaults(
    suite_manifest_info *manifest,
    const screensave_saver_module *module
)
{
    if (manifest == NULL || module == NULL) {
        return;
    }

    ZeroMemory(manifest, sizeof(*manifest));
    suite_text_copy(manifest->key, sizeof(manifest->key), module->identity.product_key);
    suite_text_copy(manifest->name, sizeof(manifest->name), module->identity.display_name);
    suite_text_copy(manifest->summary, sizeof(manifest->summary), module->identity.description);
    if (module->preset_count > 0U && module->presets != NULL && module->presets[0].preset_key != NULL) {
        suite_text_copy(manifest->default_preset, sizeof(manifest->default_preset), module->presets[0].preset_key);
    }
    if (module->theme_count > 0U && module->themes != NULL && module->themes[0].theme_key != NULL) {
        suite_text_copy(manifest->default_theme, sizeof(manifest->default_theme), module->themes[0].theme_key);
    }
    manifest->supports_gdi = 1;
    manifest->supports_gl11 = (module->capability_flags & SCREENSAVE_SAVER_CAP_GL11) != 0UL;
    manifest->supports_gl21 = (module->capability_flags & SCREENSAVE_SAVER_CAP_GL21) != 0UL;
    manifest->supports_gl33 = 0;
    manifest->long_run_stable = (module->capability_flags & SCREENSAVE_SAVER_CAP_LONG_RUN_STABLE) != 0UL;
    manifest->preview_safe = 1;
}

static int suite_manifest_load_callback(
    void *context,
    const char *section,
    const char *key,
    const char *value
)
{
    suite_manifest_load_context *load_context;
    suite_manifest_info *manifest;

    load_context = (suite_manifest_load_context *)context;
    if (load_context == NULL || load_context->manifest == NULL) {
        return 0;
    }

    manifest = load_context->manifest;
    if (lstrcmpiA(section, "product") == 0) {
        if (lstrcmpiA(key, "key") == 0) {
            return suite_text_copy(manifest->key, sizeof(manifest->key), value);
        }
        if (lstrcmpiA(key, "name") == 0) {
            return suite_text_copy(manifest->name, sizeof(manifest->name), value);
        }
        if (lstrcmpiA(key, "version") == 0) {
            return suite_text_copy(manifest->version, sizeof(manifest->version), value);
        }
        return 1;
    }

    if (lstrcmpiA(section, "identity") == 0) {
        if (lstrcmpiA(key, "summary") == 0) {
            return suite_text_copy(manifest->summary, sizeof(manifest->summary), value);
        }
        if (lstrcmpiA(key, "default_preset") == 0) {
            return suite_text_copy(manifest->default_preset, sizeof(manifest->default_preset), value);
        }
        if (lstrcmpiA(key, "default_theme") == 0) {
            return suite_text_copy(manifest->default_theme, sizeof(manifest->default_theme), value);
        }
        return 1;
    }

    if (lstrcmpiA(section, "capabilities") == 0) {
        if (lstrcmpiA(key, "gdi") == 0) {
            manifest->supports_gdi = lstrcmpiA(value, "0") != 0;
            return 1;
        }
        if (lstrcmpiA(key, "gl11") == 0) {
            manifest->supports_gl11 = lstrcmpiA(value, "0") != 0;
            return 1;
        }
        if (lstrcmpiA(key, "gl21") == 0) {
            manifest->supports_gl21 = lstrcmpiA(value, "0") != 0;
            return 1;
        }
        if (lstrcmpiA(key, "gl33") == 0) {
            manifest->supports_gl33 = lstrcmpiA(value, "0") != 0;
            return 1;
        }
        if (lstrcmpiA(key, "preview_safe") == 0) {
            manifest->preview_safe = lstrcmpiA(value, "0") != 0;
            return 1;
        }
        if (lstrcmpiA(key, "long_run_stable") == 0) {
            manifest->long_run_stable = lstrcmpiA(value, "0") != 0;
            return 1;
        }
        return 1;
    }

    if (lstrcmpiA(section, "notes") == 0) {
        if (lstrcmpiA(key, "scope") == 0) {
            return suite_text_copy(manifest->scope, sizeof(manifest->scope), value);
        }
        if (lstrcmpiA(key, "renderer") == 0) {
            return suite_text_copy(manifest->renderer, sizeof(manifest->renderer), value);
        }
        return 1;
    }

    return 1;
}

static int suite_load_manifest_file(const char *path, suite_manifest_info *manifest)
{
    suite_manifest_load_context context;

    if (path == NULL || manifest == NULL) {
        return 0;
    }

    context.manifest = manifest;
    return screensave_ini_parse_file(path, suite_manifest_load_callback, &context);
}

static void suite_build_renderer_string_from_manifest(suite_manifest_info *manifest)
{
    if (manifest == NULL) {
        return;
    }

    manifest->renderer[0] = '\0';
    if (manifest->supports_gdi) {
        suite_append_text(manifest->renderer, sizeof(manifest->renderer), "GDI floor");
    }
    if (manifest->supports_gl11) {
        if (manifest->renderer[0] != '\0') {
            suite_append_text(manifest->renderer, sizeof(manifest->renderer), ", ");
        }
        suite_append_text(manifest->renderer, sizeof(manifest->renderer), "OpenGL 1.1 optional");
    }
    if (manifest->supports_gl21) {
        if (manifest->renderer[0] != '\0') {
            suite_append_text(manifest->renderer, sizeof(manifest->renderer), ", ");
        }
        suite_append_text(manifest->renderer, sizeof(manifest->renderer), "OpenGL 2.1 optional");
    }
    if (manifest->supports_gl33) {
        if (manifest->renderer[0] != '\0') {
            suite_append_text(manifest->renderer, sizeof(manifest->renderer), ", ");
        }
        suite_append_text(manifest->renderer, sizeof(manifest->renderer), "OpenGL 3.3 modern optional");
    }
}

static const char *suite_scope_display_name(const char *scope)
{
    if (scope == NULL || scope[0] == '\0') {
        return "General";
    }

    if (lstrcmpiA(scope, "first_real_product") == 0) {
        return "Quiet and Foundation";
    }
    if (lstrcmpiA(scope, "motion_family") == 0) {
        return "Motion Family";
    }
    if (lstrcmpiA(scope, "framebuffer_vector_family") == 0) {
        return "Framebuffer and Vector";
    }
    if (lstrcmpiA(scope, "grid_simulation_family") == 0) {
        return "Grid and Simulation";
    }
    if (lstrcmpiA(scope, "systems_ambient_family") == 0) {
        return "Systems and Ambient";
    }
    if (lstrcmpiA(scope, "places_atmosphere_family") == 0) {
        return "Places and Atmosphere";
    }
    if (lstrcmpiA(scope, "heavyweight_worlds_family") == 0) {
        return "Heavyweight Worlds";
    }
    if (lstrcmpiA(scope, "heavyweight_worlds_renderer_showcase") == 0) {
        return "Renderer Showcase";
    }
    if (lstrcmpiA(scope, "suite_meta_saver") == 0) {
        return "Suite Meta-Saver";
    }

    return scope;
}

static const char *suite_role_display_name(const char *scope)
{
    if (scope != NULL && lstrcmpiA(scope, "suite_meta_saver") == 0) {
        return "Suite Meta-Saver";
    }

    return "Standalone Saver";
}

static void suite_build_browser_label(suite_catalog_entry *entry)
{
    if (entry == NULL) {
        return;
    }

    entry->browser_label[0] = '\0';
    suite_append_text(entry->browser_label, sizeof(entry->browser_label), entry->manifest.name);
    if (!entry->artifact_available) {
        suite_append_text(entry->browser_label, sizeof(entry->browser_label), " [module only]");
    }
}

static void suite_build_pack_summaries(
    suite_catalog_entry *entry,
    const char *repo_root,
    screensave_diag_context *diagnostics
)
{
    char pack_root[SUITE_PATH_LENGTH];
    screensave_pack_manifest manifests[SUITE_PACK_LIMIT];
    unsigned int manifest_count;
    unsigned int index;

    if (entry == NULL || repo_root == NULL || repo_root[0] == '\0') {
        return;
    }

    if (
        !suite_join_path(repo_root, "products\\savers", pack_root, sizeof(pack_root)) ||
        !suite_join_path(pack_root, entry->manifest.key, pack_root, sizeof(pack_root)) ||
        !suite_join_path(pack_root, "packs", pack_root, sizeof(pack_root)) ||
        !suite_directory_exists(pack_root)
    ) {
        return;
    }

    manifest_count = 0U;
    if (
        !screensave_pack_manifest_discover(
            pack_root,
            SCREENSAVE_PACK_SOURCE_BUILTIN,
            entry->manifest.key,
            manifests,
            SUITE_PACK_LIMIT,
            &manifest_count,
            diagnostics
        )
    ) {
        return;
    }

    entry->pack_count = manifest_count;
    for (index = 0U; index < manifest_count && index < SUITE_PACK_LIMIT; ++index) {
        suite_text_copy(entry->packs[index].pack_key, sizeof(entry->packs[index].pack_key), manifests[index].pack_key);
        suite_text_copy(
            entry->packs[index].display_name,
            sizeof(entry->packs[index].display_name),
            manifests[index].display_name
        );
    }
}

static int suite_locate_artifact_in_directory(
    const char *directory_path,
    const char *product_key,
    char *artifact_path,
    unsigned int artifact_path_size
)
{
    char candidate[SUITE_PATH_LENGTH];

    if (
        directory_path == NULL ||
        product_key == NULL ||
        artifact_path == NULL ||
        !suite_join_path(directory_path, product_key, candidate, sizeof(candidate)) ||
        !suite_append_text(candidate, sizeof(candidate), ".scr")
    ) {
        return 0;
    }

    if (!suite_file_exists(candidate)) {
        return 0;
    }

    return suite_text_copy(artifact_path, artifact_path_size, candidate);
}

static void suite_locate_artifact(
    suite_catalog_entry *entry,
    const char *repo_root
)
{
    char module_path[SUITE_PATH_LENGTH];
    char executable_directory[SUITE_PATH_LENGTH];
    char candidate_directory[SUITE_PATH_LENGTH];

    if (entry == NULL) {
        return;
    }

    entry->artifact_available = 0;
    entry->artifact_path[0] = '\0';

    module_path[0] = '\0';
    executable_directory[0] = '\0';
    if (
        GetModuleFileNameA(NULL, module_path, (DWORD)sizeof(module_path)) > 0U &&
        suite_copy_parent_directory(module_path, executable_directory, sizeof(executable_directory))
    ) {
        if (
            suite_join_path(executable_directory, "SAVERS", candidate_directory, sizeof(candidate_directory)) &&
            suite_locate_artifact_in_directory(
                candidate_directory,
                entry->manifest.key,
                entry->artifact_path,
                sizeof(entry->artifact_path)
            )
        ) {
            entry->artifact_available = 1;
            return;
        }
        if (
            suite_locate_artifact_in_directory(
                executable_directory,
                entry->manifest.key,
                entry->artifact_path,
                sizeof(entry->artifact_path)
            )
        ) {
            entry->artifact_available = 1;
            return;
        }
    }

    if (repo_root == NULL || repo_root[0] == '\0') {
        return;
    }

    if (
        suite_join_path(repo_root, "out\\msvc\\vs2022\\Debug", candidate_directory, sizeof(candidate_directory)) &&
        suite_join_path(candidate_directory, entry->manifest.key, candidate_directory, sizeof(candidate_directory)) &&
        suite_locate_artifact_in_directory(candidate_directory, entry->manifest.key, entry->artifact_path, sizeof(entry->artifact_path))
    ) {
        entry->artifact_available = 1;
        return;
    }
    if (
        suite_join_path(repo_root, "out\\msvc\\vs2022\\Release", candidate_directory, sizeof(candidate_directory)) &&
        suite_join_path(candidate_directory, entry->manifest.key, candidate_directory, sizeof(candidate_directory)) &&
        suite_locate_artifact_in_directory(candidate_directory, entry->manifest.key, entry->artifact_path, sizeof(entry->artifact_path))
    ) {
        entry->artifact_available = 1;
        return;
    }
    if (
        suite_join_path(repo_root, "out\\mingw\\i686\\debug", candidate_directory, sizeof(candidate_directory)) &&
        suite_join_path(candidate_directory, entry->manifest.key, candidate_directory, sizeof(candidate_directory)) &&
        suite_locate_artifact_in_directory(candidate_directory, entry->manifest.key, entry->artifact_path, sizeof(entry->artifact_path))
    ) {
        entry->artifact_available = 1;
        return;
    }
    if (
        suite_join_path(repo_root, "out\\mingw\\i686\\release", candidate_directory, sizeof(candidate_directory)) &&
        suite_join_path(candidate_directory, entry->manifest.key, candidate_directory, sizeof(candidate_directory)) &&
        suite_locate_artifact_in_directory(candidate_directory, entry->manifest.key, entry->artifact_path, sizeof(entry->artifact_path))
    ) {
        entry->artifact_available = 1;
        return;
    }
    if (
        suite_join_path(repo_root, "out\\portable\\screensave-portable-c06\\SAVERS", candidate_directory, sizeof(candidate_directory)) &&
        suite_locate_artifact_in_directory(candidate_directory, entry->manifest.key, entry->artifact_path, sizeof(entry->artifact_path))
    ) {
        entry->artifact_available = 1;
        return;
    }
    if (
        suite_join_path(
            repo_root,
            "out\\installer\\screensave-installer-c07\\PAYLOAD\\SAVERS",
            candidate_directory,
            sizeof(candidate_directory)
        ) &&
        suite_locate_artifact_in_directory(candidate_directory, entry->manifest.key, entry->artifact_path, sizeof(entry->artifact_path))
    ) {
        entry->artifact_available = 1;
    }
}

unsigned int suite_get_available_module_count(void)
{
    return (unsigned int)(sizeof(g_suite_module_getters) / sizeof(g_suite_module_getters[0]));
}

const screensave_saver_module *suite_get_available_module(unsigned int index)
{
    if (index >= suite_get_available_module_count()) {
        return NULL;
    }

    return g_suite_module_getters[index]();
}

const screensave_saver_module *suite_find_target_module(const char *product_key)
{
    const screensave_saver_module *modules[SUITE_PRODUCT_LIMIT];
    unsigned int index;

    for (index = 0U; index < suite_get_available_module_count(); ++index) {
        modules[index] = suite_get_available_module(index);
    }

    return screensave_saver_registry_find(modules, suite_get_available_module_count(), product_key);
}

int suite_catalog_build(suite_app *app)
{
    char repo_root[SUITE_PATH_LENGTH];
    unsigned int index;

    if (app == NULL) {
        return 0;
    }

    repo_root[0] = '\0';
    (void)suite_detect_repo_root(repo_root, sizeof(repo_root));

    ZeroMemory(app->catalog, sizeof(app->catalog));
    app->catalog_count = 0U;

    for (index = 0U; index < suite_get_available_module_count() && index < SUITE_PRODUCT_LIMIT; ++index) {
        suite_catalog_entry *entry;
        const screensave_saver_module *module;
        char manifest_path[SUITE_PATH_LENGTH];

        module = suite_get_available_module(index);
        if (!screensave_saver_module_is_valid(module)) {
            continue;
        }

        entry = &app->catalog[app->catalog_count];
        entry->module = module;
        suite_manifest_set_defaults(&entry->manifest, module);

        if (
            repo_root[0] != '\0' &&
            suite_join_path(repo_root, "products\\savers", manifest_path, sizeof(manifest_path)) &&
            suite_join_path(manifest_path, entry->manifest.key, manifest_path, sizeof(manifest_path)) &&
            suite_join_path(manifest_path, "manifest.ini", manifest_path, sizeof(manifest_path)) &&
            suite_file_exists(manifest_path)
        ) {
            (void)suite_load_manifest_file(manifest_path, &entry->manifest);
        }

        suite_build_renderer_string_from_manifest(&entry->manifest);
        suite_text_copy(entry->family_label, sizeof(entry->family_label), suite_scope_display_name(entry->manifest.scope));
        suite_text_copy(entry->role_label, sizeof(entry->role_label), suite_role_display_name(entry->manifest.scope));
        suite_build_pack_summaries(entry, repo_root, &app->diagnostics);
        suite_locate_artifact(entry, repo_root);
        suite_build_browser_label(entry);
        app->catalog_count += 1U;
    }

    return app->catalog_count > 0U;
}

int suite_find_catalog_index(const suite_app *app, const char *product_key)
{
    const char *canonical_key;
    unsigned int index;

    if (app == NULL || product_key == NULL || product_key[0] == '\0') {
        return -1;
    }

    canonical_key = screensave_saver_registry_canonical_product_key(product_key);
    if (canonical_key == NULL) {
        return -1;
    }

    for (index = 0U; index < app->catalog_count; ++index) {
        if (
            app->catalog[index].manifest.key[0] != '\0' &&
            lstrcmpiA(app->catalog[index].manifest.key, canonical_key) == 0
        ) {
            return (int)index;
        }
    }

    return -1;
}

const suite_catalog_entry *suite_get_selected_entry(const suite_app *app)
{
    if (
        app == NULL ||
        app->catalog_count == 0U ||
        app->selected_index >= app->catalog_count
    ) {
        return NULL;
    }

    return &app->catalog[app->selected_index];
}

const screensave_saver_module *suite_get_selected_module(const suite_app *app)
{
    const suite_catalog_entry *entry;

    entry = suite_get_selected_entry(app);
    return entry != NULL ? entry->module : NULL;
}
