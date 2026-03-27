ROOT := ../../..
PROFILE ?= debug

CC ?= gcc
AR ?= ar
RC ?= windres

OUTROOT := $(ROOT)/out
INT_ROOT := $(OUTROOT)/intermediate/mingw/i686/$(PROFILE)

PLATFORM_DIR := $(INT_ROOT)/screensave_platform
NOCTURNE_DIR := $(INT_ROOT)/nocturne
RICOCHET_DIR := $(INT_ROOT)/ricochet
DEEPFIELD_DIR := $(INT_ROOT)/deepfield
BENCHLAB_DIR := $(INT_ROOT)/benchlab

NOCTURNE_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/nocturne
RICOCHET_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/ricochet
DEEPFIELD_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/deepfield
BENCHLAB_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/benchlab

PLATFORM_LIB := $(PLATFORM_DIR)/libscreensave_platform.a
NOCTURNE_TARGET := $(NOCTURNE_BIN_DIR)/nocturne.scr
RICOCHET_TARGET := $(RICOCHET_BIN_DIR)/ricochet.scr
DEEPFIELD_TARGET := $(DEEPFIELD_BIN_DIR)/deepfield.scr
BENCHLAB_TARGET := $(BENCHLAB_BIN_DIR)/benchlab.exe

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
	$(PLATFORM_DIR)/rng.o \
	$(PLATFORM_DIR)/timing.o \
	$(PLATFORM_DIR)/version.o \
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

NOCTURNE_COMMON_OBJS := \
	$(NOCTURNE_DIR)/nocturne_config.o \
	$(NOCTURNE_DIR)/nocturne_module.o \
	$(NOCTURNE_DIR)/nocturne_presets.o \
	$(NOCTURNE_DIR)/nocturne_render.o \
	$(NOCTURNE_DIR)/nocturne_sim.o \
	$(NOCTURNE_DIR)/nocturne_themes.o \
	$(NOCTURNE_DIR)/ricochet_config.o \
	$(NOCTURNE_DIR)/ricochet_module.o \
	$(NOCTURNE_DIR)/ricochet_presets.o \
	$(NOCTURNE_DIR)/ricochet_render.o \
	$(NOCTURNE_DIR)/ricochet_sim.o \
	$(NOCTURNE_DIR)/ricochet_themes.o \
	$(NOCTURNE_DIR)/deepfield_config.o \
	$(NOCTURNE_DIR)/deepfield_module.o \
	$(NOCTURNE_DIR)/deepfield_presets.o \
	$(NOCTURNE_DIR)/deepfield_render.o \
	$(NOCTURNE_DIR)/deepfield_sim.o \
	$(NOCTURNE_DIR)/deepfield_themes.o

RICOCHET_COMMON_OBJS := \
	$(RICOCHET_DIR)/nocturne_config.o \
	$(RICOCHET_DIR)/nocturne_module.o \
	$(RICOCHET_DIR)/nocturne_presets.o \
	$(RICOCHET_DIR)/nocturne_render.o \
	$(RICOCHET_DIR)/nocturne_sim.o \
	$(RICOCHET_DIR)/nocturne_themes.o \
	$(RICOCHET_DIR)/ricochet_config.o \
	$(RICOCHET_DIR)/ricochet_module.o \
	$(RICOCHET_DIR)/ricochet_presets.o \
	$(RICOCHET_DIR)/ricochet_render.o \
	$(RICOCHET_DIR)/ricochet_sim.o \
	$(RICOCHET_DIR)/ricochet_themes.o \
	$(RICOCHET_DIR)/deepfield_config.o \
	$(RICOCHET_DIR)/deepfield_module.o \
	$(RICOCHET_DIR)/deepfield_presets.o \
	$(RICOCHET_DIR)/deepfield_render.o \
	$(RICOCHET_DIR)/deepfield_sim.o \
	$(RICOCHET_DIR)/deepfield_themes.o

DEEPFIELD_COMMON_OBJS := \
	$(DEEPFIELD_DIR)/nocturne_config.o \
	$(DEEPFIELD_DIR)/nocturne_module.o \
	$(DEEPFIELD_DIR)/nocturne_presets.o \
	$(DEEPFIELD_DIR)/nocturne_render.o \
	$(DEEPFIELD_DIR)/nocturne_sim.o \
	$(DEEPFIELD_DIR)/nocturne_themes.o \
	$(DEEPFIELD_DIR)/ricochet_config.o \
	$(DEEPFIELD_DIR)/ricochet_module.o \
	$(DEEPFIELD_DIR)/ricochet_presets.o \
	$(DEEPFIELD_DIR)/ricochet_render.o \
	$(DEEPFIELD_DIR)/ricochet_sim.o \
	$(DEEPFIELD_DIR)/ricochet_themes.o \
	$(DEEPFIELD_DIR)/deepfield_config.o \
	$(DEEPFIELD_DIR)/deepfield_module.o \
	$(DEEPFIELD_DIR)/deepfield_presets.o \
	$(DEEPFIELD_DIR)/deepfield_render.o \
	$(DEEPFIELD_DIR)/deepfield_sim.o \
	$(DEEPFIELD_DIR)/deepfield_themes.o

BENCHLAB_SAVER_OBJS := \
	$(BENCHLAB_DIR)/nocturne_config.o \
	$(BENCHLAB_DIR)/nocturne_module.o \
	$(BENCHLAB_DIR)/nocturne_presets.o \
	$(BENCHLAB_DIR)/nocturne_render.o \
	$(BENCHLAB_DIR)/nocturne_sim.o \
	$(BENCHLAB_DIR)/nocturne_themes.o \
	$(BENCHLAB_DIR)/ricochet_config.o \
	$(BENCHLAB_DIR)/ricochet_module.o \
	$(BENCHLAB_DIR)/ricochet_presets.o \
	$(BENCHLAB_DIR)/ricochet_render.o \
	$(BENCHLAB_DIR)/ricochet_sim.o \
	$(BENCHLAB_DIR)/ricochet_themes.o \
	$(BENCHLAB_DIR)/deepfield_config.o \
	$(BENCHLAB_DIR)/deepfield_module.o \
	$(BENCHLAB_DIR)/deepfield_presets.o \
	$(BENCHLAB_DIR)/deepfield_render.o \
	$(BENCHLAB_DIR)/deepfield_sim.o \
	$(BENCHLAB_DIR)/deepfield_themes.o

NOCTURNE_OBJS := $(NOCTURNE_COMMON_OBJS) \
	$(NOCTURNE_DIR)/nocturne_entry.o \
	$(NOCTURNE_DIR)/nocturne_config_res.o \
	$(NOCTURNE_DIR)/ricochet_config_res.o \
	$(NOCTURNE_DIR)/deepfield_config_res.o \
	$(NOCTURNE_DIR)/screensave_host_res.o

RICOCHET_OBJS := $(RICOCHET_COMMON_OBJS) \
	$(RICOCHET_DIR)/ricochet_entry.o \
	$(RICOCHET_DIR)/nocturne_config_res.o \
	$(RICOCHET_DIR)/ricochet_config_res.o \
	$(RICOCHET_DIR)/deepfield_config_res.o \
	$(RICOCHET_DIR)/screensave_host_res.o

DEEPFIELD_OBJS := $(DEEPFIELD_COMMON_OBJS) \
	$(DEEPFIELD_DIR)/deepfield_entry.o \
	$(DEEPFIELD_DIR)/nocturne_config_res.o \
	$(DEEPFIELD_DIR)/ricochet_config_res.o \
	$(DEEPFIELD_DIR)/deepfield_config_res.o \
	$(DEEPFIELD_DIR)/screensave_host_res.o

BENCHLAB_OBJS := \
	$(BENCHLAB_DIR)/benchlab_app.o \
	$(BENCHLAB_DIR)/benchlab_config.o \
	$(BENCHLAB_DIR)/benchlab_diag.o \
	$(BENCHLAB_DIR)/benchlab_main.o \
	$(BENCHLAB_DIR)/benchlab_overlay.o \
	$(BENCHLAB_DIR)/benchlab_session.o \
	$(BENCHLAB_SAVER_OBJS) \
	$(BENCHLAB_DIR)/nocturne_config_res.o \
	$(BENCHLAB_DIR)/ricochet_config_res.o \
	$(BENCHLAB_DIR)/deepfield_config_res.o

.PHONY: all clean

all: $(NOCTURNE_TARGET) $(RICOCHET_TARGET) $(DEEPFIELD_TARGET) $(BENCHLAB_TARGET)

$(NOCTURNE_TARGET): $(PLATFORM_LIB) $(NOCTURNE_OBJS)
	mkdir -p $(NOCTURNE_BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(NOCTURNE_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(RICOCHET_TARGET): $(PLATFORM_LIB) $(RICOCHET_OBJS)
	mkdir -p $(RICOCHET_BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(RICOCHET_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(DEEPFIELD_TARGET): $(PLATFORM_LIB) $(DEEPFIELD_OBJS)
	mkdir -p $(DEEPFIELD_BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(DEEPFIELD_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(BENCHLAB_TARGET): $(PLATFORM_LIB) $(BENCHLAB_OBJS)
	mkdir -p $(BENCHLAB_BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(BENCHLAB_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

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

$(PLATFORM_DIR)/rng.o: $(ROOT)/platform/src/core/rng/rng.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/timing.o: $(ROOT)/platform/src/core/timing/timing.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/version.o: $(ROOT)/platform/src/core/version/version.c
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

$(NOCTURNE_DIR)/nocturne_%.o: $(ROOT)/products/savers/nocturne/src/nocturne_%.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/ricochet_%.o: $(ROOT)/products/savers/ricochet/src/ricochet_%.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/deepfield_%.o: $(ROOT)/products/savers/deepfield/src/deepfield_%.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(RICOCHET_DIR)/nocturne_%.o: $(ROOT)/products/savers/nocturne/src/nocturne_%.c
	mkdir -p $(RICOCHET_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(RICOCHET_DIR)/ricochet_%.o: $(ROOT)/products/savers/ricochet/src/ricochet_%.c
	mkdir -p $(RICOCHET_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(RICOCHET_DIR)/deepfield_%.o: $(ROOT)/products/savers/deepfield/src/deepfield_%.c
	mkdir -p $(RICOCHET_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEEPFIELD_DIR)/nocturne_%.o: $(ROOT)/products/savers/nocturne/src/nocturne_%.c
	mkdir -p $(DEEPFIELD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEEPFIELD_DIR)/ricochet_%.o: $(ROOT)/products/savers/ricochet/src/ricochet_%.c
	mkdir -p $(DEEPFIELD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEEPFIELD_DIR)/deepfield_%.o: $(ROOT)/products/savers/deepfield/src/deepfield_%.c
	mkdir -p $(DEEPFIELD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/benchlab_%.o: $(ROOT)/products/apps/benchlab/src/benchlab_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/nocturne_%.o: $(ROOT)/products/savers/nocturne/src/nocturne_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/ricochet_%.o: $(ROOT)/products/savers/ricochet/src/ricochet_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/deepfield_%.o: $(ROOT)/products/savers/deepfield/src/deepfield_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc $(ROOT)/products/savers/nocturne/src/nocturne_resource.h
	mkdir -p $(NOCTURNE_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/nocturne/src -i $< -o $@

$(NOCTURNE_DIR)/ricochet_config_res.o: $(ROOT)/products/savers/ricochet/src/ricochet_config.rc $(ROOT)/products/savers/ricochet/src/ricochet_resource.h
	mkdir -p $(NOCTURNE_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ricochet/src -i $< -o $@

$(NOCTURNE_DIR)/deepfield_config_res.o: $(ROOT)/products/savers/deepfield/src/deepfield_config.rc $(ROOT)/products/savers/deepfield/src/deepfield_resource.h
	mkdir -p $(NOCTURNE_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/deepfield/src -i $< -o $@

$(RICOCHET_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc $(ROOT)/products/savers/nocturne/src/nocturne_resource.h
	mkdir -p $(RICOCHET_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/nocturne/src -i $< -o $@

$(RICOCHET_DIR)/ricochet_config_res.o: $(ROOT)/products/savers/ricochet/src/ricochet_config.rc $(ROOT)/products/savers/ricochet/src/ricochet_resource.h
	mkdir -p $(RICOCHET_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ricochet/src -i $< -o $@

$(RICOCHET_DIR)/deepfield_config_res.o: $(ROOT)/products/savers/deepfield/src/deepfield_config.rc $(ROOT)/products/savers/deepfield/src/deepfield_resource.h
	mkdir -p $(RICOCHET_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/deepfield/src -i $< -o $@

$(DEEPFIELD_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc $(ROOT)/products/savers/nocturne/src/nocturne_resource.h
	mkdir -p $(DEEPFIELD_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/nocturne/src -i $< -o $@

$(DEEPFIELD_DIR)/ricochet_config_res.o: $(ROOT)/products/savers/ricochet/src/ricochet_config.rc $(ROOT)/products/savers/ricochet/src/ricochet_resource.h
	mkdir -p $(DEEPFIELD_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ricochet/src -i $< -o $@

$(DEEPFIELD_DIR)/deepfield_config_res.o: $(ROOT)/products/savers/deepfield/src/deepfield_config.rc $(ROOT)/products/savers/deepfield/src/deepfield_resource.h
	mkdir -p $(DEEPFIELD_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/deepfield/src -i $< -o $@

$(BENCHLAB_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc $(ROOT)/products/savers/nocturne/src/nocturne_resource.h
	mkdir -p $(BENCHLAB_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/nocturne/src -i $< -o $@

$(BENCHLAB_DIR)/ricochet_config_res.o: $(ROOT)/products/savers/ricochet/src/ricochet_config.rc $(ROOT)/products/savers/ricochet/src/ricochet_resource.h
	mkdir -p $(BENCHLAB_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ricochet/src -i $< -o $@

$(BENCHLAB_DIR)/deepfield_config_res.o: $(ROOT)/products/savers/deepfield/src/deepfield_config.rc $(ROOT)/products/savers/deepfield/src/deepfield_resource.h
	mkdir -p $(BENCHLAB_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/deepfield/src -i $< -o $@

$(NOCTURNE_DIR)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc $(ROOT)/platform/src/host/win32_scr/resource.h
	mkdir -p $(NOCTURNE_DIR)
	$(RC) --include-dir=$(ROOT)/platform/src/host/win32_scr -i $< -o $@

$(RICOCHET_DIR)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc $(ROOT)/platform/src/host/win32_scr/resource.h
	mkdir -p $(RICOCHET_DIR)
	$(RC) --include-dir=$(ROOT)/platform/src/host/win32_scr -i $< -o $@

$(DEEPFIELD_DIR)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc $(ROOT)/platform/src/host/win32_scr/resource.h
	mkdir -p $(DEEPFIELD_DIR)
	$(RC) --include-dir=$(ROOT)/platform/src/host/win32_scr -i $< -o $@

clean:
	rm -rf $(NOCTURNE_BIN_DIR) $(RICOCHET_BIN_DIR) $(DEEPFIELD_BIN_DIR) $(BENCHLAB_BIN_DIR) \
		$(PLATFORM_DIR) $(NOCTURNE_DIR) $(RICOCHET_DIR) $(DEEPFIELD_DIR) $(BENCHLAB_DIR)
