ROOT := ../../..
PROFILE ?= debug

CC ?= gcc
AR ?= ar
RC ?= windres

OUTROOT := $(ROOT)/out
INT_ROOT := $(OUTROOT)/intermediate/mingw/i686/$(PROFILE)
PLATFORM_DIR := $(INT_ROOT)/screensave_platform
BENCHLAB_DIR := $(INT_ROOT)/benchlab
SUITE_DIR := $(INT_ROOT)/suite
INNER_SAVERS := nocturne ricochet deepfield plasma phosphor pipeworks lifeforms signals mechanize ecosystems stormglass transit observatory vector explorer city atlas gallery
SAVERS := $(INNER_SAVERS) anthology
SAVER_UNITS := config module presets render sim themes
SUITE_APP_UNITS := app browser config launch main manifest state
SAVER_TARGETS := $(foreach saver,$(SAVERS),$(OUTROOT)/mingw/i686/$(PROFILE)/$(saver)/$(saver).scr)
ANTHOLOGY_TARGET := $(OUTROOT)/mingw/i686/$(PROFILE)/anthology/anthology.scr
ANTHOLOGY_INNER_OBJS = $(foreach owner,$(INNER_SAVERS),$(call saver_objs,$(owner)))
ANTHOLOGY_OBJS = $(call saver_objs,anthology)
ANTHOLOGY_RESOURCE_OBJS = $(call saver_resource_objs,anthology)
BENCHLAB_TARGET := $(OUTROOT)/mingw/i686/$(PROFILE)/benchlab/benchlab.exe
SUITE_TARGET := $(OUTROOT)/mingw/i686/$(PROFILE)/suite/suite.exe
PLATFORM_LIB := $(PLATFORM_DIR)/libscreensave_platform.a
CFLAGS_COMMON := -std=c89 -Wall -Wextra -pedantic -I$(ROOT)/platform/include
RCFLAGS_COMMON := -I$(ROOT)/platform/include -I$(ROOT)/platform/src/host/win32_scr
LDLIBS_COMMON := -ladvapi32 -lgdi32 -luser32 -lopengl32

ifeq ($(PROFILE),release)
	CFLAGS := $(CFLAGS_COMMON) -O2
else
	CFLAGS := $(CFLAGS_COMMON) -g
endif

PLATFORM_OBJS := \
	$(PLATFORM_DIR)/display_text.o \
	$(PLATFORM_DIR)/ini_file.o \
	$(PLATFORM_DIR)/renderer_dispatch.o \
	$(PLATFORM_DIR)/saver_contract.o \
	$(PLATFORM_DIR)/saver_registry.o \
	$(PLATFORM_DIR)/backend_caps.o \
	$(PLATFORM_DIR)/backend_kind.o \
	$(PLATFORM_DIR)/backend_loader.o \
	$(PLATFORM_DIR)/backend_registry.o \
	$(PLATFORM_DIR)/present_path.o \
	$(PLATFORM_DIR)/render_band.o \
	$(PLATFORM_DIR)/service_seams.o \
	$(PLATFORM_DIR)/config.o \
	$(PLATFORM_DIR)/settings.o \
	$(PLATFORM_DIR)/diagnostics.o \
	$(PLATFORM_DIR)/grid_buffer.o \
	$(PLATFORM_DIR)/rng.o \
	$(PLATFORM_DIR)/timing.o \
	$(PLATFORM_DIR)/version.o \
	$(PLATFORM_DIR)/visual_buffer.o \
	$(PLATFORM_DIR)/gdi_capture.o \
	$(PLATFORM_DIR)/gdi_backend.o \
	$(PLATFORM_DIR)/gdi_bitmap.o \
	$(PLATFORM_DIR)/gdi_present.o \
	$(PLATFORM_DIR)/gdi_primitives.o \
	$(PLATFORM_DIR)/gdi_state.o \
	$(PLATFORM_DIR)/gdi_surface.o \
	$(PLATFORM_DIR)/gl11_backend.o \
	$(PLATFORM_DIR)/gl11_bitmap.o \
	$(PLATFORM_DIR)/gl11_caps.o \
	$(PLATFORM_DIR)/gl11_capture.o \
	$(PLATFORM_DIR)/gl11_context.o \
	$(PLATFORM_DIR)/gl11_present.o \
	$(PLATFORM_DIR)/gl11_primitives.o \
	$(PLATFORM_DIR)/gl11_state.o \
	$(PLATFORM_DIR)/gl21_backend.o \
	$(PLATFORM_DIR)/gl21_caps.o \
	$(PLATFORM_DIR)/gl21_capture.o \
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

saver_objs = $(foreach unit,$(SAVER_UNITS),$(INT_ROOT)/$(1)/$(1)_$(unit).o)
saver_resource_objs = $(INT_ROOT)/$(1)/$(1)_config_res.o $(INT_ROOT)/$(1)/$(1)_version_res.o $(INT_ROOT)/$(1)/screensave_host_res.o
benchlab_saver_objs = $(foreach owner,$(SAVERS),$(foreach unit,$(SAVER_UNITS),$(BENCHLAB_DIR)/$(owner)_$(unit).o))
benchlab_resource_objs = $(foreach owner,$(SAVERS),$(BENCHLAB_DIR)/$(owner)_config_res.o)
suite_saver_objs = $(foreach owner,$(SAVERS),$(foreach unit,$(SAVER_UNITS),$(SUITE_DIR)/$(owner)_$(unit).o))
suite_resource_objs = $(foreach owner,$(SAVERS),$(SUITE_DIR)/$(owner)_config_res.o)
suite_app_objs = $(foreach unit,$(SUITE_APP_UNITS),$(SUITE_DIR)/suite_$(unit).o)

.PHONY: all clean
all: $(SAVER_TARGETS) $(BENCHLAB_TARGET) $(SUITE_TARGET)

$(PLATFORM_LIB): $(PLATFORM_OBJS)
	mkdir -p $(PLATFORM_DIR)
	$(AR) rcs $@ $(PLATFORM_OBJS)

$(PLATFORM_DIR)/%.o: $(ROOT)/platform/src/core/base/%.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/%.o: $(ROOT)/platform/src/core/substrate/%.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/config.o: $(ROOT)/platform/src/core/config/config.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/settings.o: $(ROOT)/platform/src/core/config/settings.c
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
$(OUTROOT)/mingw/i686/$(PROFILE)/$(1)/$(1).scr: $(PLATFORM_LIB) $(call saver_objs,$(1)) $(INT_ROOT)/$(1)/$(1)_entry.o $(call saver_resource_objs,$(1))
	mkdir -p $(OUTROOT)/mingw/i686/$(PROFILE)/$(1)
	$(CC) $(CFLAGS) -mwindows -o $$@ $(call saver_objs,$(1)) $(INT_ROOT)/$(1)/$(1)_entry.o $(call saver_resource_objs,$(1)) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(INT_ROOT)/$(1)/$(1)_%.o: $(ROOT)/products/savers/$(1)/src/$(1)_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/$(1)_config_res.o: $(ROOT)/products/savers/$(1)/src/$(1)_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) $(RCFLAGS_COMMON) -I$(ROOT)/products/savers/$(1)/src -i $$< -o $$@

$(INT_ROOT)/$(1)/$(1)_version_res.o: $(ROOT)/products/savers/$(1)/src/$(1)_version.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) $(RCFLAGS_COMMON) -I$(ROOT)/products/savers/$(1)/src -i $$< -o $$@

$(INT_ROOT)/$(1)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) $(RCFLAGS_COMMON) -i $$< -o $$@
endef

$(foreach saver,$(INNER_SAVERS),$(eval $(call SAVER_template,$(saver))))

$(ANTHOLOGY_TARGET): $(PLATFORM_LIB) $(ANTHOLOGY_INNER_OBJS) $(ANTHOLOGY_OBJS) $(INT_ROOT)/anthology/anthology_entry.o $(ANTHOLOGY_RESOURCE_OBJS)
	mkdir -p $(OUTROOT)/mingw/i686/$(PROFILE)/anthology
	$(CC) $(CFLAGS) -mwindows -o $@ $(ANTHOLOGY_INNER_OBJS) $(ANTHOLOGY_OBJS) $(INT_ROOT)/anthology/anthology_entry.o $(ANTHOLOGY_RESOURCE_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(INT_ROOT)/anthology/anthology_%.o: $(ROOT)/products/savers/anthology/src/anthology_%.c
	mkdir -p $(INT_ROOT)/anthology
	$(CC) $(CFLAGS) -c $< -o $@

$(INT_ROOT)/anthology/anthology_config_res.o: $(ROOT)/products/savers/anthology/src/anthology_config.rc
	mkdir -p $(INT_ROOT)/anthology
	$(RC) $(RCFLAGS_COMMON) -I$(ROOT)/products/savers/anthology/src -i $< -o $@

$(INT_ROOT)/anthology/anthology_version_res.o: $(ROOT)/products/savers/anthology/src/anthology_version.rc
	mkdir -p $(INT_ROOT)/anthology
	$(RC) $(RCFLAGS_COMMON) -I$(ROOT)/products/savers/anthology/src -i $< -o $@

$(INT_ROOT)/anthology/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc
	mkdir -p $(INT_ROOT)/anthology
	$(RC) $(RCFLAGS_COMMON) -i $< -o $@

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

define BENCHLAB_SAVER_template
$(BENCHLAB_DIR)/$(1)_%.o: $(ROOT)/products/savers/$(1)/src/$(1)_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(BENCHLAB_DIR)/$(1)_config_res.o: $(ROOT)/products/savers/$(1)/src/$(1)_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) $(RCFLAGS_COMMON) -I$(ROOT)/products/savers/$(1)/src -i $$< -o $$@
endef

$(foreach saver,$(SAVERS),$(eval $(call BENCHLAB_SAVER_template,$(saver))))

$(SUITE_TARGET): $(PLATFORM_LIB) $(suite_saver_objs) $(suite_resource_objs) $(suite_app_objs)
	mkdir -p $(OUTROOT)/mingw/i686/$(PROFILE)/suite
	$(CC) $(CFLAGS) -mwindows -o $@ $(suite_saver_objs) $(suite_resource_objs) $(suite_app_objs) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(SUITE_DIR)/suite_app.o: $(ROOT)/products/apps/suite/src/suite_app.c
	mkdir -p $(SUITE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(SUITE_DIR)/suite_browser.o: $(ROOT)/products/apps/suite/src/suite_browser.c
	mkdir -p $(SUITE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(SUITE_DIR)/suite_config.o: $(ROOT)/products/apps/suite/src/suite_config.c
	mkdir -p $(SUITE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(SUITE_DIR)/suite_launch.o: $(ROOT)/products/apps/suite/src/suite_launch.c
	mkdir -p $(SUITE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(SUITE_DIR)/suite_main.o: $(ROOT)/products/apps/suite/src/suite_main.c
	mkdir -p $(SUITE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(SUITE_DIR)/suite_manifest.o: $(ROOT)/products/apps/suite/src/suite_manifest.c
	mkdir -p $(SUITE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(SUITE_DIR)/suite_state.o: $(ROOT)/products/apps/suite/src/suite_state.c
	mkdir -p $(SUITE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

define SUITE_SAVER_template
$(SUITE_DIR)/$(1)_%.o: $(ROOT)/products/savers/$(1)/src/$(1)_%.c
	mkdir -p $(SUITE_DIR)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(SUITE_DIR)/$(1)_config_res.o: $(ROOT)/products/savers/$(1)/src/$(1)_config.rc
	mkdir -p $(SUITE_DIR)
	$(RC) $(RCFLAGS_COMMON) -I$(ROOT)/products/savers/$(1)/src -i $$< -o $$@
endef

$(foreach saver,$(SAVERS),$(eval $(call SUITE_SAVER_template,$(saver))))

clean:
	rm -rf $(OUTROOT)/mingw/i686/$(PROFILE) $(INT_ROOT)
