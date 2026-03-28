ROOT := ../../..
PROFILE ?= debug

CC ?= gcc
AR ?= ar
RC ?= windres

OUTROOT := $(ROOT)/out
INT_ROOT := $(OUTROOT)/intermediate/mingw/i686/$(PROFILE)
PLATFORM_DIR := $(INT_ROOT)/screensave_platform
BENCHLAB_DIR := $(INT_ROOT)/benchlab
SAVERS := nocturne ricochet deepfield ember oscilloscope_dreams pipeworks lifeforms signal_lab mechanical_dreams ecosystems
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

$(INT_ROOT)/$(1)/ember_%.o: $(ROOT)/products/savers/ember/src/ember_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/ember_config_res.o: $(ROOT)/products/savers/ember/src/ember_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/ember/src -i $$< -o $$@

$(INT_ROOT)/$(1)/oscilloscope_dreams_%.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/oscilloscope_dreams_config_res.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/oscilloscope_dreams/src -i $$< -o $$@

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

$(INT_ROOT)/$(1)/signal_lab_%.o: $(ROOT)/products/savers/signal_lab/src/signal_lab_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/signal_lab_config_res.o: $(ROOT)/products/savers/signal_lab/src/signal_lab_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/signal_lab/src -i $$< -o $$@

$(INT_ROOT)/$(1)/mechanical_dreams_%.o: $(ROOT)/products/savers/mechanical_dreams/src/mechanical_dreams_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/mechanical_dreams_config_res.o: $(ROOT)/products/savers/mechanical_dreams/src/mechanical_dreams_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/mechanical_dreams/src -i $$< -o $$@

$(INT_ROOT)/$(1)/ecosystems_%.o: $(ROOT)/products/savers/ecosystems/src/ecosystems_%.c
	mkdir -p $(INT_ROOT)/$(1)
	$(CC) $(CFLAGS) -c $$< -o $$@

$(INT_ROOT)/$(1)/ecosystems_config_res.o: $(ROOT)/products/savers/ecosystems/src/ecosystems_config.rc
	mkdir -p $(INT_ROOT)/$(1)
	$(RC) -I$(ROOT)/products/savers/ecosystems/src -i $$< -o $$@

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

$(BENCHLAB_DIR)/ember_%.o: $(ROOT)/products/savers/ember/src/ember_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/ember_config_res.o: $(ROOT)/products/savers/ember/src/ember_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/ember/src -i $< -o $@

$(BENCHLAB_DIR)/oscilloscope_dreams_%.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/oscilloscope_dreams_config_res.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/oscilloscope_dreams/src -i $< -o $@

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

$(BENCHLAB_DIR)/signal_lab_%.o: $(ROOT)/products/savers/signal_lab/src/signal_lab_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/signal_lab_config_res.o: $(ROOT)/products/savers/signal_lab/src/signal_lab_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/signal_lab/src -i $< -o $@

$(BENCHLAB_DIR)/mechanical_dreams_%.o: $(ROOT)/products/savers/mechanical_dreams/src/mechanical_dreams_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/mechanical_dreams_config_res.o: $(ROOT)/products/savers/mechanical_dreams/src/mechanical_dreams_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/mechanical_dreams/src -i $< -o $@

$(BENCHLAB_DIR)/ecosystems_%.o: $(ROOT)/products/savers/ecosystems/src/ecosystems_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/ecosystems_config_res.o: $(ROOT)/products/savers/ecosystems/src/ecosystems_config.rc
	mkdir -p $(BENCHLAB_DIR)
	$(RC) -I$(ROOT)/products/savers/ecosystems/src -i $< -o $@

clean:
	rm -rf $(OUTROOT)/mingw/i686/$(PROFILE) $(INT_ROOT)

