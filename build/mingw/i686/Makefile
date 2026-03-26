ROOT := ../../..
PROFILE ?= debug

CC ?= gcc
AR ?= ar
RC ?= windres

OUTROOT := $(ROOT)/out
NOCTURNE_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/nocturne
BENCHLAB_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/benchlab
INT_ROOT := $(OUTROOT)/intermediate/mingw/i686/$(PROFILE)
PLATFORM_DIR := $(INT_ROOT)/screensave_platform
NOCTURNE_DIR := $(INT_ROOT)/nocturne
BENCHLAB_DIR := $(INT_ROOT)/benchlab

PLATFORM_LIB := $(PLATFORM_DIR)/libscreensave_platform.a
NOCTURNE_TARGET := $(NOCTURNE_BIN_DIR)/nocturne.scr
BENCHLAB_TARGET := $(BENCHLAB_BIN_DIR)/benchlab.exe

PLATFORM_SRCS := \
	$(ROOT)/platform/src/core/base/renderer_dispatch.c \
	$(ROOT)/platform/src/core/base/saver_contract.c \
	$(ROOT)/platform/src/core/config/config.c \
	$(ROOT)/platform/src/core/diagnostics/diagnostics.c \
	$(ROOT)/platform/src/core/rng/rng.c \
	$(ROOT)/platform/src/core/timing/timing.c \
	$(ROOT)/platform/src/core/version/version.c \
	$(ROOT)/platform/src/render/gdi/gdi_backend.c \
	$(ROOT)/platform/src/render/gdi/gdi_bitmap.c \
	$(ROOT)/platform/src/render/gdi/gdi_present.c \
	$(ROOT)/platform/src/render/gdi/gdi_primitives.c \
	$(ROOT)/platform/src/render/gdi/gdi_state.c \
	$(ROOT)/platform/src/render/gdi/gdi_surface.c \
	$(ROOT)/platform/src/host/win32_scr/scr_args.c \
	$(ROOT)/platform/src/host/win32_scr/scr_config_dialog.c \
	$(ROOT)/platform/src/host/win32_scr/scr_diagnostics.c \
	$(ROOT)/platform/src/host/win32_scr/scr_entry.c \
	$(ROOT)/platform/src/host/win32_scr/scr_settings.c \
	$(ROOT)/platform/src/host/win32_scr/scr_validation_scene.c \
	$(ROOT)/platform/src/host/win32_scr/scr_window.c

NOCTURNE_SRCS := \
	$(ROOT)/products/savers/nocturne/src/nocturne_config.c \
	$(ROOT)/products/savers/nocturne/src/nocturne_entry.c \
	$(ROOT)/products/savers/nocturne/src/nocturne_module.c \
	$(ROOT)/products/savers/nocturne/src/nocturne_presets.c \
	$(ROOT)/products/savers/nocturne/src/nocturne_render.c \
	$(ROOT)/products/savers/nocturne/src/nocturne_sim.c \
	$(ROOT)/products/savers/nocturne/src/nocturne_themes.c

BENCHLAB_SRCS := \
	$(ROOT)/products/apps/benchlab/src/benchlab_app.c \
	$(ROOT)/products/apps/benchlab/src/benchlab_config.c \
	$(ROOT)/products/apps/benchlab/src/benchlab_diag.c \
	$(ROOT)/products/apps/benchlab/src/benchlab_main.c \
	$(ROOT)/products/apps/benchlab/src/benchlab_overlay.c \
	$(ROOT)/products/apps/benchlab/src/benchlab_session.c \
	$(ROOT)/products/savers/nocturne/src/nocturne_config.c \
	$(ROOT)/products/savers/nocturne/src/nocturne_module.c \
	$(ROOT)/products/savers/nocturne/src/nocturne_presets.c \
	$(ROOT)/products/savers/nocturne/src/nocturne_render.c \
	$(ROOT)/products/savers/nocturne/src/nocturne_sim.c \
	$(ROOT)/products/savers/nocturne/src/nocturne_themes.c

PLATFORM_OBJS := \
	$(PLATFORM_DIR)/renderer_dispatch.o \
	$(PLATFORM_DIR)/saver_contract.o \
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
	$(PLATFORM_DIR)/scr_args.o \
	$(PLATFORM_DIR)/scr_config_dialog.o \
	$(PLATFORM_DIR)/scr_diagnostics.o \
	$(PLATFORM_DIR)/scr_entry.o \
	$(PLATFORM_DIR)/scr_settings.o \
	$(PLATFORM_DIR)/scr_validation_scene.o \
	$(PLATFORM_DIR)/scr_window.o

NOCTURNE_OBJS := \
	$(NOCTURNE_DIR)/nocturne_config.o \
	$(NOCTURNE_DIR)/nocturne_entry.o \
	$(NOCTURNE_DIR)/nocturne_module.o \
	$(NOCTURNE_DIR)/nocturne_presets.o \
	$(NOCTURNE_DIR)/nocturne_render.o \
	$(NOCTURNE_DIR)/nocturne_sim.o \
	$(NOCTURNE_DIR)/nocturne_themes.o \
	$(NOCTURNE_DIR)/nocturne_config_res.o \
	$(NOCTURNE_DIR)/screensave_host_res.o

BENCHLAB_OBJS := \
	$(BENCHLAB_DIR)/benchlab_app.o \
	$(BENCHLAB_DIR)/benchlab_config.o \
	$(BENCHLAB_DIR)/benchlab_diag.o \
	$(BENCHLAB_DIR)/benchlab_main.o \
	$(BENCHLAB_DIR)/benchlab_overlay.o \
	$(BENCHLAB_DIR)/benchlab_session.o \
	$(BENCHLAB_DIR)/nocturne_config.o \
	$(BENCHLAB_DIR)/nocturne_module.o \
	$(BENCHLAB_DIR)/nocturne_presets.o \
	$(BENCHLAB_DIR)/nocturne_render.o \
	$(BENCHLAB_DIR)/nocturne_sim.o \
	$(BENCHLAB_DIR)/nocturne_themes.o \
	$(BENCHLAB_DIR)/nocturne_config_res.o

CFLAGS_COMMON := -std=c89 -Wall -Wextra -pedantic -I$(ROOT)/platform/include
LDLIBS_COMMON := -ladvapi32 -lgdi32 -luser32

ifeq ($(PROFILE),release)
	CFLAGS := $(CFLAGS_COMMON) -O2
else
	CFLAGS := $(CFLAGS_COMMON) -g
endif

.PHONY: all clean

all: $(NOCTURNE_TARGET) $(BENCHLAB_TARGET)

$(NOCTURNE_TARGET): $(PLATFORM_LIB) $(NOCTURNE_OBJS)
	mkdir -p $(NOCTURNE_BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(NOCTURNE_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(BENCHLAB_TARGET): $(PLATFORM_LIB) $(BENCHLAB_OBJS)
	mkdir -p $(BENCHLAB_BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(BENCHLAB_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(PLATFORM_LIB): $(PLATFORM_OBJS)
	mkdir -p $(PLATFORM_DIR)
	$(AR) rcs $@ $(PLATFORM_OBJS)

$(PLATFORM_DIR)/renderer_dispatch.o: $(ROOT)/platform/src/core/base/renderer_dispatch.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/saver_contract.o: $(ROOT)/platform/src/core/base/saver_contract.c
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

$(PLATFORM_DIR)/gdi_backend.o: $(ROOT)/platform/src/render/gdi/gdi_backend.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/gdi_bitmap.o: $(ROOT)/platform/src/render/gdi/gdi_bitmap.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/gdi_present.o: $(ROOT)/platform/src/render/gdi/gdi_present.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/gdi_primitives.o: $(ROOT)/platform/src/render/gdi/gdi_primitives.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/gdi_state.o: $(ROOT)/platform/src/render/gdi/gdi_state.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/gdi_surface.o: $(ROOT)/platform/src/render/gdi/gdi_surface.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/scr_args.o: $(ROOT)/platform/src/host/win32_scr/scr_args.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/scr_config_dialog.o: $(ROOT)/platform/src/host/win32_scr/scr_config_dialog.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/scr_diagnostics.o: $(ROOT)/platform/src/host/win32_scr/scr_diagnostics.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/scr_entry.o: $(ROOT)/platform/src/host/win32_scr/scr_entry.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/scr_settings.o: $(ROOT)/platform/src/host/win32_scr/scr_settings.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/scr_validation_scene.o: $(ROOT)/platform/src/host/win32_scr/scr_validation_scene.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/scr_window.o: $(ROOT)/platform/src/host/win32_scr/scr_window.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/nocturne_config.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/nocturne_entry.o: $(ROOT)/products/savers/nocturne/src/nocturne_entry.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/nocturne_module.o: $(ROOT)/products/savers/nocturne/src/nocturne_module.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/nocturne_presets.o: $(ROOT)/products/savers/nocturne/src/nocturne_presets.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/nocturne_render.o: $(ROOT)/products/savers/nocturne/src/nocturne_render.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/nocturne_sim.o: $(ROOT)/products/savers/nocturne/src/nocturne_sim.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/nocturne_themes.o: $(ROOT)/products/savers/nocturne/src/nocturne_themes.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc $(ROOT)/products/savers/nocturne/src/nocturne_resource.h
	mkdir -p $(NOCTURNE_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/nocturne/src -i $< -o $@

$(NOCTURNE_DIR)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc $(ROOT)/platform/src/host/win32_scr/resource.h
	mkdir -p $(NOCTURNE_DIR)
	$(RC) --include-dir=$(ROOT)/platform/src/host/win32_scr -i $< -o $@

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

$(BENCHLAB_DIR)/nocturne_config.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/nocturne_module.o: $(ROOT)/products/savers/nocturne/src/nocturne_module.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/nocturne_presets.o: $(ROOT)/products/savers/nocturne/src/nocturne_presets.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/nocturne_render.o: $(ROOT)/products/savers/nocturne/src/nocturne_render.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/nocturne_sim.o: $(ROOT)/products/savers/nocturne/src/nocturne_sim.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/nocturne_themes.o: $(ROOT)/products/savers/nocturne/src/nocturne_themes.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc $(ROOT)/products/savers/nocturne/src/nocturne_resource.h
	mkdir -p $(BENCHLAB_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/nocturne/src -i $< -o $@

clean:
	rm -rf $(NOCTURNE_BIN_DIR) $(BENCHLAB_BIN_DIR) $(PLATFORM_DIR) $(NOCTURNE_DIR) $(BENCHLAB_DIR)
