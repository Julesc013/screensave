ROOT := ../../..
PROFILE ?= debug

CC ?= gcc
AR ?= ar
RC ?= windres

OUTROOT := $(ROOT)/out
INT_ROOT := $(OUTROOT)/intermediate/mingw/i686/$(PROFILE)
PLATFORM_DIR := $(INT_ROOT)/screensave_platform
BENCHLAB_DIR := $(INT_ROOT)/benchlab
SAVERS := nocturne ricochet deepfield plasma phosphor pipeworks lifeforms signals mechanize ecosystems stormglass transit observatory vector explorer city atlas gallery
SAVER_UNITS := config module presets render sim themes
SAVER_TARGETS := $(foreach saver,$(SAVERS),$(OUTROOT)/mingw/i686/$(PROFILE)/$(saver)/$(saver).scr)
BENCHLAB_TARGET := $(OUTROOT)/mingw/i686/$(PROFILE)/benchlab/benchlab.exe
PLATFORM_LIB := $(PLATFORM_DIR)/libscreensave_platform.a
CFLAGS_COMMON := -std=c89 -Wall -Wextra -pedantic -I$(ROOT)/platform/include
LDLIBS_COMMON := -ladvapi32 -lgdi32 -luser32 -lopengl32

ifeq ($(PROFILE),release)
	CFLAGS := $(CFLAGS_COMMON) -O2
else
	CFLAGS := $(CFLAGS_COMMON) -g
endif

PLATFORM_OBJS := \
	$(PLATFORM_DIR)/renderer_dispatch.o \
	$(PLATFORM_DIR)/saver_contract.o \
	$(PLATFORM_DIR)/saver_registry.o \
	$(PLATFORM_DIR)/config.o \
	$(PLATFORM_DIR)/diagnostics.o \
	$(PLATFORM_DIR)/grid_buffer.o \
	$(PLATFORM_DIR)/rng.o \
	$(PLATFORM_DIR)/timing.o \
	$(PLATFORM_DIR)/version.o \
	$(PLATFORM_DIR)/visual_buffer.o \
	$(PLATFORM_DIR)/gdi_backend.o \
	$(PLATFORM_DIR)/gdi_bitmap.o \
	$(PLATFORM_DIR)/gdi_present.o \
	$(PLATFORM_DIR)/gdi_primitives.o \
	$(PLATFORM_DIR)/gdi_state.o \
	$(PLATFORM_DIR)/gdi_surface.o \
	$(PLATFORM_DIR)/gl11_backend.o \
	$(PLATFORM_DIR)/gl11_bitmap.o \
	$(PLATFORM_DIR)/gl11_caps.o \
	$(PLATFORM_DIR)/gl11_context.o \
	$(PLATFORM_DIR)/gl11_present.o \
	$(PLATFORM_DIR)/gl11_primitives.o \
	$(PLATFORM_DIR)/gl11_state.o \
	$(PLATFORM_DIR)/gl21_backend.o \
	$(PLATFORM_DIR)/gl21_caps.o \
	$(PLATFORM_DIR)/gl21_context.o \
	$(PLATFORM_DIR)/gl21_present.o \
	$(PLATFORM_DIR)/gl21_primitives.o \
	$(PLATFORM_DIR)/gl21_state.o \
	$(PLATFORM_DIR)/gl33_backend.o \
	$(PLATFORM_DIR)/gl46_backend.o \
	$(PLATFORM_DIR)/null_backend.o \
	$(PLATFORM_DIR)/scr_args.o \
	$(PLATFORM_DIR)/scr_config_dialog.o \
	$(PLATFORM_DIR)/scr_diagnostics.o \
	$(PLATFORM_DIR)/scr_entry.o \
	$(PLATFORM_DIR)/scr_settings.o \
	$(PLATFORM_DIR)/scr_validation_scene.o \
	$(PLATFORM_DIR)/scr_window.o

saver_common_objs = $(foreach owner,$(SAVERS),$(foreach unit,$(SAVER_UNITS),$(INT_ROOT)/$(1)/$(owner)_$(unit).o))
saver_resource_objs = $(foreach owner,$(SAVERS),$(INT_ROOT)/$(1)/$(owner)_config_res.o) $(INT_ROOT)/$(1)/screensave_host_res.o
benchlab_saver_objs = $(foreach owner,$(SAVERS),$(foreach unit,$(SAVER_UNITS),$(BENCHLAB_DIR)/$(owner)_$(unit).o))
benchlab_resource_objs = $(foreach owner,$(SAVERS),$(BENCHLAB_DIR)/$(owner)_config_res.o)

.PHONY: all clean
all: $(SAVER_TARGETS) $(BENCHLAB_TARGET)

$(PLATFORM_LIB): $(PLATFORM_OBJS)
	mkdir -p $(PLATFORM_DIR)
	$(AR) rcs $@ $(PLATFORM_OBJS)

$(PLATFORM_DIR)/%.o: $(ROOT)/platform/src/core/base/%.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/config.o: $(ROOT)/platform/src/core/config/config.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/diagnostics.o: $(ROOT)/platform/src/core/diagnostics/diagnostics.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/grid_buffer.o: $(ROOT)/platform/src/core/grid/grid_buffer.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/rng.o: $(ROOT)/platform/src/core/rng/rng.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/timing.o: $(ROOT)/platform/src/core/timing/timing.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/version.o: $(ROOT)/platform/src/core/version/version.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/visual_buffer.o: $(ROOT)/platform/src/core/visual/visual_buffer.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/gdi_%.o: $(ROOT)/platform/src/render/gdi/gdi_%.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/gl11_%.o: $(ROOT)/platform/src/render/gl11/gl11_%.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/gl21_%.o: $(ROOT)/platform/src/render/gl21/gl21_%.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/gl33_%.o: $(ROOT)/platform/src/render/gl33/gl33_%.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/gl46_%.o: $(ROOT)/platform/src/render/gl46/gl46_%.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/null_%.o: $(ROOT)/platform/src/render/null/null_%.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/scr_%.o: $(ROOT)/platform/src/host/win32_scr/scr_%.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

define SAVER_template
$(OUTROOT)/mingw/i686/$(PROFILE)/$(1)/$(1).scr: $(PLATFORM_LIB) $(call saver_common_objs,$(1)) $(INT_ROOT)/$(1)/$(1)_entry.o $(call saver_resource_objs,$(1))
	mkdir -p $(OUTROOT)/mingw/i686/$(PROFILE)/$(1)
	$(CC) $(CFLAGS) -mwindows -o $$@ $(call saver_common_objs,$(1)) $(INT_ROOT)/$(1)/$(1)_entry.o $(call saver_resource_objs,$(1)) $(PLATFORM_LIB) $(LDLIBS_COMMON)
$(INT_ROOT)/$(1)/nocturne_%.o: $(ROOT)/products/savers/nocturne/src/nocturne_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/nocturne/src -i $$< -o $$@

$(INT_ROOT)/$(1)/ricochet_%.o: $(ROOT)/products/savers/ricochet/src/ricochet_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/ricochet_config_res.o: $(ROOT)/products/savers/ricochet/src/ricochet_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/ricochet/src -i $$< -o $$@

$(INT_ROOT)/$(1)/deepfield_%.o: $(ROOT)/products/savers/deepfield/src/deepfield_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/deepfield_config_res.o: $(ROOT)/products/savers/deepfield/src/deepfield_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/deepfield/src -i $$< -o $$@

$(INT_ROOT)/$(1)/plasma_%.o: $(ROOT)/products/savers/plasma/src/plasma_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/plasma_config_res.o: $(ROOT)/products/savers/plasma/src/plasma_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/plasma/src -i $$< -o $$@

$(INT_ROOT)/$(1)/phosphor_%.o: $(ROOT)/products/savers/phosphor/src/phosphor_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/phosphor_config_res.o: $(ROOT)/products/savers/phosphor/src/phosphor_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/phosphor/src -i $$< -o $$@

$(INT_ROOT)/$(1)/pipeworks_%.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/pipeworks_config_res.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/pipeworks/src -i $$< -o $$@

$(INT_ROOT)/$(1)/lifeforms_%.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/lifeforms_config_res.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/lifeforms/src -i $$< -o $$@

$(INT_ROOT)/$(1)/signals_%.o: $(ROOT)/products/savers/signals/src/signals_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/signals_config_res.o: $(ROOT)/products/savers/signals/src/signals_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/signals/src -i $$< -o $$@

$(INT_ROOT)/$(1)/mechanize_%.o: $(ROOT)/products/savers/mechanize/src/mechanize_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/mechanize_config_res.o: $(ROOT)/products/savers/mechanize/src/mechanize_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/mechanize/src -i $$< -o $$@

$(INT_ROOT)/$(1)/ecosystems_%.o: $(ROOT)/products/savers/ecosystems/src/ecosystems_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/ecosystems_config_res.o: $(ROOT)/products/savers/ecosystems/src/ecosystems_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/ecosystems/src -i $$< -o $$@

$(INT_ROOT)/$(1)/stormglass_%.o: $(ROOT)/products/savers/stormglass/src/stormglass_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/stormglass_config_res.o: $(ROOT)/products/savers/stormglass/src/stormglass_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/stormglass/src -i $$< -o $$@

$(INT_ROOT)/$(1)/transit_%.o: $(ROOT)/products/savers/transit/src/transit_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/transit_config_res.o: $(ROOT)/products/savers/transit/src/transit_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/transit/src -i $$< -o $$@

$(INT_ROOT)/$(1)/observatory_%.o: $(ROOT)/products/savers/observatory/src/observatory_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/observatory_config_res.o: $(ROOT)/products/savers/observatory/src/observatory_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/observatory/src -i $$< -o $$@

$(INT_ROOT)/$(1)/vector_%.o: $(ROOT)/products/savers/vector/src/vector_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/vector_config_res.o: $(ROOT)/products/savers/vector/src/vector_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/vector/src -i $$< -o $$@

$(INT_ROOT)/$(1)/explorer_%.o: $(ROOT)/products/savers/explorer/src/explorer_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/explorer_config_res.o: $(ROOT)/products/savers/explorer/src/explorer_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/explorer/src -i $$< -o $$@

$(INT_ROOT)/$(1)/city_%.o: $(ROOT)/products/savers/city/src/city_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/city_config_res.o: $(ROOT)/products/savers/city/src/city_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/city/src -i $$< -o $$@

$(INT_ROOT)/$(1)/atlas_%.o: $(ROOT)/products/savers/atlas/src/atlas_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/atlas_config_res.o: $(ROOT)/products/savers/atlas/src/atlas_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/atlas/src -i $$< -o $$@

$(INT_ROOT)/$(1)/gallery_%.o: $(ROOT)/products/savers/gallery/src/gallery_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/gallery_config_res.o: $(ROOT)/products/savers/gallery/src/gallery_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/gallery/src -i $$< -o $$@

$(INT_ROOT)/$(1)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/platform/src/host/win32_scr -i $$< -o $$@
endef

$(foreach saver,$(SAVERS),$(eval $(call SAVER_template,$(saver))))

$(BENCHLAB_TARGET): $(PLATFORM_LIB) $(benchlab_saver_objs) $(benchlab_resource_objs) $(BENCHLAB_DIR)/benchlab_app.o $(BENCHLAB_DIR)/benchlab_config.o $(BENCHLAB_DIR)/benchlab_diag.o $(BENCHLAB_DIR)/benchlab_main.o $(BENCHLAB_DIR)/benchlab_overlay.o $(BENCHLAB_DIR)/benchlab_session.o
	mkdir -p $(OUTROOT)/mingw/i686/$(PROFILE)/benchlab
	$(CC) $(CFLAGS) -mwindows -o $@ $(benchlab_saver_objs) $(benchlab_resource_objs) $(BENCHLAB_DIR)/benchlab_app.o $(BENCHLAB_DIR)/benchlab_config.o $(BENCHLAB_DIR)/benchlab_diag.o $(BENCHLAB_DIR)/benchlab_main.o $(BENCHLAB_DIR)/benchlab_overlay.o $(BENCHLAB_DIR)/benchlab_session.o $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(BENCHLAB_DIR)/benchlab_app.o: $(ROOT)/products/apps/benchlab/src/benchlab_app.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/benchlab_config.o: $(ROOT)/products/apps/benchlab/src/benchlab_config.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/benchlab_diag.o: $(ROOT)/products/apps/benchlab/src/benchlab_diag.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/benchlab_main.o: $(ROOT)/products/apps/benchlab/src/benchlab_main.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/benchlab_overlay.o: $(ROOT)/products/apps/benchlab/src/benchlab_overlay.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/benchlab_session.o: $(ROOT)/products/apps/benchlab/src/benchlab_session.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/nocturne_%.o: $(ROOT)/products/savers/nocturne/src/nocturne_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/nocturne/src -i $< -o $@

$(BENCHLAB_DIR)/ricochet_%.o: $(ROOT)/products/savers/ricochet/src/ricochet_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/ricochet_config_res.o: $(ROOT)/products/savers/ricochet/src/ricochet_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/ricochet/src -i $< -o $@

$(BENCHLAB_DIR)/deepfield_%.o: $(ROOT)/products/savers/deepfield/src/deepfield_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/deepfield_config_res.o: $(ROOT)/products/savers/deepfield/src/deepfield_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/deepfield/src -i $< -o $@

$(BENCHLAB_DIR)/plasma_%.o: $(ROOT)/products/savers/plasma/src/plasma_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/plasma_config_res.o: $(ROOT)/products/savers/plasma/src/plasma_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/plasma/src -i $< -o $@

$(BENCHLAB_DIR)/phosphor_%.o: $(ROOT)/products/savers/phosphor/src/phosphor_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/phosphor_config_res.o: $(ROOT)/products/savers/phosphor/src/phosphor_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/phosphor/src -i $< -o $@

$(BENCHLAB_DIR)/pipeworks_%.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/pipeworks_config_res.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/pipeworks/src -i $< -o $@

$(BENCHLAB_DIR)/lifeforms_%.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/lifeforms_config_res.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/lifeforms/src -i $< -o $@

$(BENCHLAB_DIR)/signals_%.o: $(ROOT)/products/savers/signals/src/signals_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/signals_config_res.o: $(ROOT)/products/savers/signals/src/signals_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/signals/src -i $< -o $@

$(BENCHLAB_DIR)/mechanize_%.o: $(ROOT)/products/savers/mechanize/src/mechanize_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/mechanize_config_res.o: $(ROOT)/products/savers/mechanize/src/mechanize_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/mechanize/src -i $< -o $@

$(BENCHLAB_DIR)/ecosystems_%.o: $(ROOT)/products/savers/ecosystems/src/ecosystems_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/ecosystems_config_res.o: $(ROOT)/products/savers/ecosystems/src/ecosystems_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/ecosystems/src -i $< -o $@

$(BENCHLAB_DIR)/stormglass_%.o: $(ROOT)/products/savers/stormglass/src/stormglass_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/stormglass_config_res.o: $(ROOT)/products/savers/stormglass/src/stormglass_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/stormglass/src -i $< -o $@

$(BENCHLAB_DIR)/transit_%.o: $(ROOT)/products/savers/transit/src/transit_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/transit_config_res.o: $(ROOT)/products/savers/transit/src/transit_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/transit/src -i $< -o $@

$(BENCHLAB_DIR)/observatory_%.o: $(ROOT)/products/savers/observatory/src/observatory_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/observatory_config_res.o: $(ROOT)/products/savers/observatory/src/observatory_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/observatory/src -i $< -o $@

$(BENCHLAB_DIR)/vector_%.o: $(ROOT)/products/savers/vector/src/vector_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/vector_config_res.o: $(ROOT)/products/savers/vector/src/vector_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/vector/src -i $< -o $@

$(BENCHLAB_DIR)/explorer_%.o: $(ROOT)/products/savers/explorer/src/explorer_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/explorer_config_res.o: $(ROOT)/products/savers/explorer/src/explorer_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/explorer/src -i $< -o $@

$(BENCHLAB_DIR)/city_%.o: $(ROOT)/products/savers/city/src/city_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/city_config_res.o: $(ROOT)/products/savers/city/src/city_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/city/src -i $< -o $@

$(BENCHLAB_DIR)/atlas_%.o: $(ROOT)/products/savers/atlas/src/atlas_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/atlas_config_res.o: $(ROOT)/products/savers/atlas/src/atlas_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/atlas/src -i $< -o $@

$(BENCHLAB_DIR)/gallery_%.o: $(ROOT)/products/savers/gallery/src/gallery_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/gallery_config_res.o: $(ROOT)/products/savers/gallery/src/gallery_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/gallery/src -i $< -o $@

clean:
	rm -rf $(OUTROOT)/mingw/i686/$(PROFILE) $(INT_ROOT)
