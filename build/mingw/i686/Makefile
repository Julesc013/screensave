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
EMBER_DIR := $(INT_ROOT)/ember
OSCILLOSCOPE_DREAMS_DIR := $(INT_ROOT)/oscilloscope_dreams
PIPEWORKS_DIR := $(INT_ROOT)/pipeworks
LIFEFORMS_DIR := $(INT_ROOT)/lifeforms
BENCHLAB_DIR := $(INT_ROOT)/benchlab

NOCTURNE_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/nocturne
RICOCHET_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/ricochet
DEEPFIELD_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/deepfield
EMBER_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/ember
OSCILLOSCOPE_DREAMS_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/oscilloscope_dreams
PIPEWORKS_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/pipeworks
LIFEFORMS_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/lifeforms
BENCHLAB_BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/benchlab

PLATFORM_LIB := $(PLATFORM_DIR)/libscreensave_platform.a
NOCTURNE_TARGET := $(NOCTURNE_BIN_DIR)/nocturne.scr
RICOCHET_TARGET := $(RICOCHET_BIN_DIR)/ricochet.scr
DEEPFIELD_TARGET := $(DEEPFIELD_BIN_DIR)/deepfield.scr
EMBER_TARGET := $(EMBER_BIN_DIR)/ember.scr
OSCILLOSCOPE_DREAMS_TARGET := $(OSCILLOSCOPE_DREAMS_BIN_DIR)/oscilloscope_dreams.scr
PIPEWORKS_TARGET := $(PIPEWORKS_BIN_DIR)/pipeworks.scr
LIFEFORMS_TARGET := $(LIFEFORMS_BIN_DIR)/lifeforms.scr
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
	$(NOCTURNE_DIR)/deepfield_themes.o \
	$(NOCTURNE_DIR)/ember_config.o \
	$(NOCTURNE_DIR)/ember_module.o \
	$(NOCTURNE_DIR)/ember_presets.o \
	$(NOCTURNE_DIR)/ember_render.o \
	$(NOCTURNE_DIR)/ember_sim.o \
	$(NOCTURNE_DIR)/ember_themes.o \
	$(NOCTURNE_DIR)/oscilloscope_dreams_config.o \
	$(NOCTURNE_DIR)/oscilloscope_dreams_module.o \
	$(NOCTURNE_DIR)/oscilloscope_dreams_presets.o \
	$(NOCTURNE_DIR)/oscilloscope_dreams_render.o \
	$(NOCTURNE_DIR)/oscilloscope_dreams_sim.o \
	$(NOCTURNE_DIR)/oscilloscope_dreams_themes.o \
	$(NOCTURNE_DIR)/pipeworks_config.o \
	$(NOCTURNE_DIR)/pipeworks_module.o \
	$(NOCTURNE_DIR)/pipeworks_presets.o \
	$(NOCTURNE_DIR)/pipeworks_render.o \
	$(NOCTURNE_DIR)/pipeworks_sim.o \
	$(NOCTURNE_DIR)/pipeworks_themes.o \
	$(NOCTURNE_DIR)/lifeforms_config.o \
	$(NOCTURNE_DIR)/lifeforms_module.o \
	$(NOCTURNE_DIR)/lifeforms_presets.o \
	$(NOCTURNE_DIR)/lifeforms_render.o \
	$(NOCTURNE_DIR)/lifeforms_sim.o \
	$(NOCTURNE_DIR)/lifeforms_themes.o

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
	$(RICOCHET_DIR)/deepfield_themes.o \
	$(RICOCHET_DIR)/ember_config.o \
	$(RICOCHET_DIR)/ember_module.o \
	$(RICOCHET_DIR)/ember_presets.o \
	$(RICOCHET_DIR)/ember_render.o \
	$(RICOCHET_DIR)/ember_sim.o \
	$(RICOCHET_DIR)/ember_themes.o \
	$(RICOCHET_DIR)/oscilloscope_dreams_config.o \
	$(RICOCHET_DIR)/oscilloscope_dreams_module.o \
	$(RICOCHET_DIR)/oscilloscope_dreams_presets.o \
	$(RICOCHET_DIR)/oscilloscope_dreams_render.o \
	$(RICOCHET_DIR)/oscilloscope_dreams_sim.o \
	$(RICOCHET_DIR)/oscilloscope_dreams_themes.o \
	$(RICOCHET_DIR)/pipeworks_config.o \
	$(RICOCHET_DIR)/pipeworks_module.o \
	$(RICOCHET_DIR)/pipeworks_presets.o \
	$(RICOCHET_DIR)/pipeworks_render.o \
	$(RICOCHET_DIR)/pipeworks_sim.o \
	$(RICOCHET_DIR)/pipeworks_themes.o \
	$(RICOCHET_DIR)/lifeforms_config.o \
	$(RICOCHET_DIR)/lifeforms_module.o \
	$(RICOCHET_DIR)/lifeforms_presets.o \
	$(RICOCHET_DIR)/lifeforms_render.o \
	$(RICOCHET_DIR)/lifeforms_sim.o \
	$(RICOCHET_DIR)/lifeforms_themes.o

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
	$(DEEPFIELD_DIR)/deepfield_themes.o \
	$(DEEPFIELD_DIR)/ember_config.o \
	$(DEEPFIELD_DIR)/ember_module.o \
	$(DEEPFIELD_DIR)/ember_presets.o \
	$(DEEPFIELD_DIR)/ember_render.o \
	$(DEEPFIELD_DIR)/ember_sim.o \
	$(DEEPFIELD_DIR)/ember_themes.o \
	$(DEEPFIELD_DIR)/oscilloscope_dreams_config.o \
	$(DEEPFIELD_DIR)/oscilloscope_dreams_module.o \
	$(DEEPFIELD_DIR)/oscilloscope_dreams_presets.o \
	$(DEEPFIELD_DIR)/oscilloscope_dreams_render.o \
	$(DEEPFIELD_DIR)/oscilloscope_dreams_sim.o \
	$(DEEPFIELD_DIR)/oscilloscope_dreams_themes.o \
	$(DEEPFIELD_DIR)/pipeworks_config.o \
	$(DEEPFIELD_DIR)/pipeworks_module.o \
	$(DEEPFIELD_DIR)/pipeworks_presets.o \
	$(DEEPFIELD_DIR)/pipeworks_render.o \
	$(DEEPFIELD_DIR)/pipeworks_sim.o \
	$(DEEPFIELD_DIR)/pipeworks_themes.o \
	$(DEEPFIELD_DIR)/lifeforms_config.o \
	$(DEEPFIELD_DIR)/lifeforms_module.o \
	$(DEEPFIELD_DIR)/lifeforms_presets.o \
	$(DEEPFIELD_DIR)/lifeforms_render.o \
	$(DEEPFIELD_DIR)/lifeforms_sim.o \
	$(DEEPFIELD_DIR)/lifeforms_themes.o

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
	$(BENCHLAB_DIR)/deepfield_themes.o \
	$(BENCHLAB_DIR)/ember_config.o \
	$(BENCHLAB_DIR)/ember_module.o \
	$(BENCHLAB_DIR)/ember_presets.o \
	$(BENCHLAB_DIR)/ember_render.o \
	$(BENCHLAB_DIR)/ember_sim.o \
	$(BENCHLAB_DIR)/ember_themes.o \
	$(BENCHLAB_DIR)/oscilloscope_dreams_config.o \
	$(BENCHLAB_DIR)/oscilloscope_dreams_module.o \
	$(BENCHLAB_DIR)/oscilloscope_dreams_presets.o \
	$(BENCHLAB_DIR)/oscilloscope_dreams_render.o \
	$(BENCHLAB_DIR)/oscilloscope_dreams_sim.o \
	$(BENCHLAB_DIR)/oscilloscope_dreams_themes.o \
	$(BENCHLAB_DIR)/pipeworks_config.o \
	$(BENCHLAB_DIR)/pipeworks_module.o \
	$(BENCHLAB_DIR)/pipeworks_presets.o \
	$(BENCHLAB_DIR)/pipeworks_render.o \
	$(BENCHLAB_DIR)/pipeworks_sim.o \
	$(BENCHLAB_DIR)/pipeworks_themes.o \
	$(BENCHLAB_DIR)/lifeforms_config.o \
	$(BENCHLAB_DIR)/lifeforms_module.o \
	$(BENCHLAB_DIR)/lifeforms_presets.o \
	$(BENCHLAB_DIR)/lifeforms_render.o \
	$(BENCHLAB_DIR)/lifeforms_sim.o \
	$(BENCHLAB_DIR)/lifeforms_themes.o

NOCTURNE_OBJS := $(NOCTURNE_COMMON_OBJS) \
	$(NOCTURNE_DIR)/nocturne_entry.o \
	$(NOCTURNE_DIR)/nocturne_config_res.o \
	$(NOCTURNE_DIR)/ricochet_config_res.o \
	$(NOCTURNE_DIR)/deepfield_config_res.o \
	$(NOCTURNE_DIR)/ember_config_res.o \
	$(NOCTURNE_DIR)/oscilloscope_dreams_config_res.o \
	$(NOCTURNE_DIR)/pipeworks_config_res.o \
	$(NOCTURNE_DIR)/lifeforms_config_res.o \
	$(NOCTURNE_DIR)/screensave_host_res.o

RICOCHET_OBJS := $(RICOCHET_COMMON_OBJS) \
	$(RICOCHET_DIR)/ricochet_entry.o \
	$(RICOCHET_DIR)/nocturne_config_res.o \
	$(RICOCHET_DIR)/ricochet_config_res.o \
	$(RICOCHET_DIR)/deepfield_config_res.o \
	$(RICOCHET_DIR)/ember_config_res.o \
	$(RICOCHET_DIR)/oscilloscope_dreams_config_res.o \
	$(RICOCHET_DIR)/pipeworks_config_res.o \
	$(RICOCHET_DIR)/lifeforms_config_res.o \
	$(RICOCHET_DIR)/screensave_host_res.o

DEEPFIELD_OBJS := $(DEEPFIELD_COMMON_OBJS) \
	$(DEEPFIELD_DIR)/deepfield_entry.o \
	$(DEEPFIELD_DIR)/nocturne_config_res.o \
	$(DEEPFIELD_DIR)/ricochet_config_res.o \
	$(DEEPFIELD_DIR)/deepfield_config_res.o \
	$(DEEPFIELD_DIR)/ember_config_res.o \
	$(DEEPFIELD_DIR)/oscilloscope_dreams_config_res.o \
	$(DEEPFIELD_DIR)/pipeworks_config_res.o \
	$(DEEPFIELD_DIR)/lifeforms_config_res.o \
	$(DEEPFIELD_DIR)/screensave_host_res.o

EMBER_COMMON_OBJS := \
	$(EMBER_DIR)/nocturne_config.o \
	$(EMBER_DIR)/nocturne_module.o \
	$(EMBER_DIR)/nocturne_presets.o \
	$(EMBER_DIR)/nocturne_render.o \
	$(EMBER_DIR)/nocturne_sim.o \
	$(EMBER_DIR)/nocturne_themes.o \
	$(EMBER_DIR)/ricochet_config.o \
	$(EMBER_DIR)/ricochet_module.o \
	$(EMBER_DIR)/ricochet_presets.o \
	$(EMBER_DIR)/ricochet_render.o \
	$(EMBER_DIR)/ricochet_sim.o \
	$(EMBER_DIR)/ricochet_themes.o \
	$(EMBER_DIR)/deepfield_config.o \
	$(EMBER_DIR)/deepfield_module.o \
	$(EMBER_DIR)/deepfield_presets.o \
	$(EMBER_DIR)/deepfield_render.o \
	$(EMBER_DIR)/deepfield_sim.o \
	$(EMBER_DIR)/deepfield_themes.o \
	$(EMBER_DIR)/ember_config.o \
	$(EMBER_DIR)/ember_module.o \
	$(EMBER_DIR)/ember_presets.o \
	$(EMBER_DIR)/ember_render.o \
	$(EMBER_DIR)/ember_sim.o \
	$(EMBER_DIR)/ember_themes.o \
	$(EMBER_DIR)/oscilloscope_dreams_config.o \
	$(EMBER_DIR)/oscilloscope_dreams_module.o \
	$(EMBER_DIR)/oscilloscope_dreams_presets.o \
	$(EMBER_DIR)/oscilloscope_dreams_render.o \
	$(EMBER_DIR)/oscilloscope_dreams_sim.o \
	$(EMBER_DIR)/oscilloscope_dreams_themes.o \
	$(EMBER_DIR)/pipeworks_config.o \
	$(EMBER_DIR)/pipeworks_module.o \
	$(EMBER_DIR)/pipeworks_presets.o \
	$(EMBER_DIR)/pipeworks_render.o \
	$(EMBER_DIR)/pipeworks_sim.o \
	$(EMBER_DIR)/pipeworks_themes.o \
	$(EMBER_DIR)/lifeforms_config.o \
	$(EMBER_DIR)/lifeforms_module.o \
	$(EMBER_DIR)/lifeforms_presets.o \
	$(EMBER_DIR)/lifeforms_render.o \
	$(EMBER_DIR)/lifeforms_sim.o \
	$(EMBER_DIR)/lifeforms_themes.o

EMBER_OBJS := $(EMBER_COMMON_OBJS) \
	$(EMBER_DIR)/ember_entry.o \
	$(EMBER_DIR)/nocturne_config_res.o \
	$(EMBER_DIR)/ricochet_config_res.o \
	$(EMBER_DIR)/deepfield_config_res.o \
	$(EMBER_DIR)/ember_config_res.o \
	$(EMBER_DIR)/oscilloscope_dreams_config_res.o \
	$(EMBER_DIR)/pipeworks_config_res.o \
	$(EMBER_DIR)/lifeforms_config_res.o \
	$(EMBER_DIR)/screensave_host_res.o

OSCILLOSCOPE_DREAMS_COMMON_OBJS := \
	$(OSCILLOSCOPE_DREAMS_DIR)/nocturne_config.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/nocturne_module.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/nocturne_presets.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/nocturne_render.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/nocturne_sim.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/nocturne_themes.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ricochet_config.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ricochet_module.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ricochet_presets.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ricochet_render.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ricochet_sim.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ricochet_themes.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/deepfield_config.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/deepfield_module.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/deepfield_presets.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/deepfield_render.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/deepfield_sim.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/deepfield_themes.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ember_config.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ember_module.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ember_presets.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ember_render.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ember_sim.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ember_themes.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/oscilloscope_dreams_config.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/oscilloscope_dreams_module.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/oscilloscope_dreams_presets.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/oscilloscope_dreams_render.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/oscilloscope_dreams_sim.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/oscilloscope_dreams_themes.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/pipeworks_config.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/pipeworks_module.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/pipeworks_presets.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/pipeworks_render.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/pipeworks_sim.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/pipeworks_themes.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/lifeforms_config.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/lifeforms_module.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/lifeforms_presets.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/lifeforms_render.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/lifeforms_sim.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/lifeforms_themes.o

PIPEWORKS_COMMON_OBJS := \
	$(PIPEWORKS_DIR)/nocturne_config.o \
	$(PIPEWORKS_DIR)/nocturne_module.o \
	$(PIPEWORKS_DIR)/nocturne_presets.o \
	$(PIPEWORKS_DIR)/nocturne_render.o \
	$(PIPEWORKS_DIR)/nocturne_sim.o \
	$(PIPEWORKS_DIR)/nocturne_themes.o \
	$(PIPEWORKS_DIR)/ricochet_config.o \
	$(PIPEWORKS_DIR)/ricochet_module.o \
	$(PIPEWORKS_DIR)/ricochet_presets.o \
	$(PIPEWORKS_DIR)/ricochet_render.o \
	$(PIPEWORKS_DIR)/ricochet_sim.o \
	$(PIPEWORKS_DIR)/ricochet_themes.o \
	$(PIPEWORKS_DIR)/deepfield_config.o \
	$(PIPEWORKS_DIR)/deepfield_module.o \
	$(PIPEWORKS_DIR)/deepfield_presets.o \
	$(PIPEWORKS_DIR)/deepfield_render.o \
	$(PIPEWORKS_DIR)/deepfield_sim.o \
	$(PIPEWORKS_DIR)/deepfield_themes.o \
	$(PIPEWORKS_DIR)/ember_config.o \
	$(PIPEWORKS_DIR)/ember_module.o \
	$(PIPEWORKS_DIR)/ember_presets.o \
	$(PIPEWORKS_DIR)/ember_render.o \
	$(PIPEWORKS_DIR)/ember_sim.o \
	$(PIPEWORKS_DIR)/ember_themes.o \
	$(PIPEWORKS_DIR)/oscilloscope_dreams_config.o \
	$(PIPEWORKS_DIR)/oscilloscope_dreams_module.o \
	$(PIPEWORKS_DIR)/oscilloscope_dreams_presets.o \
	$(PIPEWORKS_DIR)/oscilloscope_dreams_render.o \
	$(PIPEWORKS_DIR)/oscilloscope_dreams_sim.o \
	$(PIPEWORKS_DIR)/oscilloscope_dreams_themes.o \
	$(PIPEWORKS_DIR)/pipeworks_config.o \
	$(PIPEWORKS_DIR)/pipeworks_module.o \
	$(PIPEWORKS_DIR)/pipeworks_presets.o \
	$(PIPEWORKS_DIR)/pipeworks_render.o \
	$(PIPEWORKS_DIR)/pipeworks_sim.o \
	$(PIPEWORKS_DIR)/pipeworks_themes.o \
	$(PIPEWORKS_DIR)/lifeforms_config.o \
	$(PIPEWORKS_DIR)/lifeforms_module.o \
	$(PIPEWORKS_DIR)/lifeforms_presets.o \
	$(PIPEWORKS_DIR)/lifeforms_render.o \
	$(PIPEWORKS_DIR)/lifeforms_sim.o \
	$(PIPEWORKS_DIR)/lifeforms_themes.o

LIFEFORMS_COMMON_OBJS := \
	$(LIFEFORMS_DIR)/nocturne_config.o \
	$(LIFEFORMS_DIR)/nocturne_module.o \
	$(LIFEFORMS_DIR)/nocturne_presets.o \
	$(LIFEFORMS_DIR)/nocturne_render.o \
	$(LIFEFORMS_DIR)/nocturne_sim.o \
	$(LIFEFORMS_DIR)/nocturne_themes.o \
	$(LIFEFORMS_DIR)/ricochet_config.o \
	$(LIFEFORMS_DIR)/ricochet_module.o \
	$(LIFEFORMS_DIR)/ricochet_presets.o \
	$(LIFEFORMS_DIR)/ricochet_render.o \
	$(LIFEFORMS_DIR)/ricochet_sim.o \
	$(LIFEFORMS_DIR)/ricochet_themes.o \
	$(LIFEFORMS_DIR)/deepfield_config.o \
	$(LIFEFORMS_DIR)/deepfield_module.o \
	$(LIFEFORMS_DIR)/deepfield_presets.o \
	$(LIFEFORMS_DIR)/deepfield_render.o \
	$(LIFEFORMS_DIR)/deepfield_sim.o \
	$(LIFEFORMS_DIR)/deepfield_themes.o \
	$(LIFEFORMS_DIR)/ember_config.o \
	$(LIFEFORMS_DIR)/ember_module.o \
	$(LIFEFORMS_DIR)/ember_presets.o \
	$(LIFEFORMS_DIR)/ember_render.o \
	$(LIFEFORMS_DIR)/ember_sim.o \
	$(LIFEFORMS_DIR)/ember_themes.o \
	$(LIFEFORMS_DIR)/oscilloscope_dreams_config.o \
	$(LIFEFORMS_DIR)/oscilloscope_dreams_module.o \
	$(LIFEFORMS_DIR)/oscilloscope_dreams_presets.o \
	$(LIFEFORMS_DIR)/oscilloscope_dreams_render.o \
	$(LIFEFORMS_DIR)/oscilloscope_dreams_sim.o \
	$(LIFEFORMS_DIR)/oscilloscope_dreams_themes.o \
	$(LIFEFORMS_DIR)/pipeworks_config.o \
	$(LIFEFORMS_DIR)/pipeworks_module.o \
	$(LIFEFORMS_DIR)/pipeworks_presets.o \
	$(LIFEFORMS_DIR)/pipeworks_render.o \
	$(LIFEFORMS_DIR)/pipeworks_sim.o \
	$(LIFEFORMS_DIR)/pipeworks_themes.o \
	$(LIFEFORMS_DIR)/lifeforms_config.o \
	$(LIFEFORMS_DIR)/lifeforms_module.o \
	$(LIFEFORMS_DIR)/lifeforms_presets.o \
	$(LIFEFORMS_DIR)/lifeforms_render.o \
	$(LIFEFORMS_DIR)/lifeforms_sim.o \
	$(LIFEFORMS_DIR)/lifeforms_themes.o

OSCILLOSCOPE_DREAMS_OBJS := $(OSCILLOSCOPE_DREAMS_COMMON_OBJS) \
	$(OSCILLOSCOPE_DREAMS_DIR)/oscilloscope_dreams_entry.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/nocturne_config_res.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ricochet_config_res.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/deepfield_config_res.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/ember_config_res.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/oscilloscope_dreams_config_res.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/pipeworks_config_res.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/lifeforms_config_res.o \
	$(OSCILLOSCOPE_DREAMS_DIR)/screensave_host_res.o

PIPEWORKS_OBJS := $(PIPEWORKS_COMMON_OBJS) \
	$(PIPEWORKS_DIR)/pipeworks_entry.o \
	$(PIPEWORKS_DIR)/nocturne_config_res.o \
	$(PIPEWORKS_DIR)/ricochet_config_res.o \
	$(PIPEWORKS_DIR)/deepfield_config_res.o \
	$(PIPEWORKS_DIR)/ember_config_res.o \
	$(PIPEWORKS_DIR)/oscilloscope_dreams_config_res.o \
	$(PIPEWORKS_DIR)/pipeworks_config_res.o \
	$(PIPEWORKS_DIR)/lifeforms_config_res.o \
	$(PIPEWORKS_DIR)/screensave_host_res.o

LIFEFORMS_OBJS := $(LIFEFORMS_COMMON_OBJS) \
	$(LIFEFORMS_DIR)/lifeforms_entry.o \
	$(LIFEFORMS_DIR)/nocturne_config_res.o \
	$(LIFEFORMS_DIR)/ricochet_config_res.o \
	$(LIFEFORMS_DIR)/deepfield_config_res.o \
	$(LIFEFORMS_DIR)/ember_config_res.o \
	$(LIFEFORMS_DIR)/oscilloscope_dreams_config_res.o \
	$(LIFEFORMS_DIR)/pipeworks_config_res.o \
	$(LIFEFORMS_DIR)/lifeforms_config_res.o \
	$(LIFEFORMS_DIR)/screensave_host_res.o

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
	$(BENCHLAB_DIR)/deepfield_config_res.o \
	$(BENCHLAB_DIR)/ember_config_res.o \
	$(BENCHLAB_DIR)/oscilloscope_dreams_config_res.o \
	$(BENCHLAB_DIR)/pipeworks_config_res.o \
	$(BENCHLAB_DIR)/lifeforms_config_res.o

.PHONY: all clean

all: $(NOCTURNE_TARGET) $(RICOCHET_TARGET) $(DEEPFIELD_TARGET) $(EMBER_TARGET) $(OSCILLOSCOPE_DREAMS_TARGET) $(PIPEWORKS_TARGET) $(LIFEFORMS_TARGET) $(BENCHLAB_TARGET)

$(NOCTURNE_TARGET): $(PLATFORM_LIB) $(NOCTURNE_OBJS)
	mkdir -p $(NOCTURNE_BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(NOCTURNE_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(RICOCHET_TARGET): $(PLATFORM_LIB) $(RICOCHET_OBJS)
	mkdir -p $(RICOCHET_BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(RICOCHET_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(DEEPFIELD_TARGET): $(PLATFORM_LIB) $(DEEPFIELD_OBJS)
	mkdir -p $(DEEPFIELD_BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(DEEPFIELD_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(EMBER_TARGET): $(PLATFORM_LIB) $(EMBER_OBJS)
	mkdir -p $(EMBER_BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(EMBER_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(OSCILLOSCOPE_DREAMS_TARGET): $(PLATFORM_LIB) $(OSCILLOSCOPE_DREAMS_OBJS)
	mkdir -p $(OSCILLOSCOPE_DREAMS_BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(OSCILLOSCOPE_DREAMS_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(PIPEWORKS_TARGET): $(PLATFORM_LIB) $(PIPEWORKS_OBJS)
	mkdir -p $(PIPEWORKS_BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(PIPEWORKS_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

$(LIFEFORMS_TARGET): $(PLATFORM_LIB) $(LIFEFORMS_OBJS)
	mkdir -p $(LIFEFORMS_BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(LIFEFORMS_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

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

$(NOCTURNE_DIR)/nocturne_%.o: $(ROOT)/products/savers/nocturne/src/nocturne_%.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/ricochet_%.o: $(ROOT)/products/savers/ricochet/src/ricochet_%.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/deepfield_%.o: $(ROOT)/products/savers/deepfield/src/deepfield_%.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/ember_%.o: $(ROOT)/products/savers/ember/src/ember_%.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/oscilloscope_dreams_%.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_%.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/pipeworks_%.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_%.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/lifeforms_%.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_%.c
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

$(RICOCHET_DIR)/ember_%.o: $(ROOT)/products/savers/ember/src/ember_%.c
	mkdir -p $(RICOCHET_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(RICOCHET_DIR)/oscilloscope_dreams_%.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_%.c
	mkdir -p $(RICOCHET_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(RICOCHET_DIR)/pipeworks_%.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_%.c
	mkdir -p $(RICOCHET_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(RICOCHET_DIR)/lifeforms_%.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_%.c
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

$(DEEPFIELD_DIR)/ember_%.o: $(ROOT)/products/savers/ember/src/ember_%.c
	mkdir -p $(DEEPFIELD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEEPFIELD_DIR)/oscilloscope_dreams_%.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_%.c
	mkdir -p $(DEEPFIELD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEEPFIELD_DIR)/pipeworks_%.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_%.c
	mkdir -p $(DEEPFIELD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(DEEPFIELD_DIR)/lifeforms_%.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_%.c
	mkdir -p $(DEEPFIELD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(EMBER_DIR)/nocturne_%.o: $(ROOT)/products/savers/nocturne/src/nocturne_%.c
	mkdir -p $(EMBER_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(EMBER_DIR)/ricochet_%.o: $(ROOT)/products/savers/ricochet/src/ricochet_%.c
	mkdir -p $(EMBER_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(EMBER_DIR)/deepfield_%.o: $(ROOT)/products/savers/deepfield/src/deepfield_%.c
	mkdir -p $(EMBER_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(EMBER_DIR)/ember_%.o: $(ROOT)/products/savers/ember/src/ember_%.c
	mkdir -p $(EMBER_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(EMBER_DIR)/oscilloscope_dreams_%.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_%.c
	mkdir -p $(EMBER_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(EMBER_DIR)/pipeworks_%.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_%.c
	mkdir -p $(EMBER_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(EMBER_DIR)/lifeforms_%.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_%.c
	mkdir -p $(EMBER_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/nocturne_%.o: $(ROOT)/products/savers/nocturne/src/nocturne_%.c
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/ricochet_%.o: $(ROOT)/products/savers/ricochet/src/ricochet_%.c
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/deepfield_%.o: $(ROOT)/products/savers/deepfield/src/deepfield_%.c
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/ember_%.o: $(ROOT)/products/savers/ember/src/ember_%.c
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/oscilloscope_dreams_%.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_%.c
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/pipeworks_%.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_%.c
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/lifeforms_%.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_%.c
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PIPEWORKS_DIR)/nocturne_%.o: $(ROOT)/products/savers/nocturne/src/nocturne_%.c
	mkdir -p $(PIPEWORKS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PIPEWORKS_DIR)/ricochet_%.o: $(ROOT)/products/savers/ricochet/src/ricochet_%.c
	mkdir -p $(PIPEWORKS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PIPEWORKS_DIR)/deepfield_%.o: $(ROOT)/products/savers/deepfield/src/deepfield_%.c
	mkdir -p $(PIPEWORKS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PIPEWORKS_DIR)/ember_%.o: $(ROOT)/products/savers/ember/src/ember_%.c
	mkdir -p $(PIPEWORKS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PIPEWORKS_DIR)/oscilloscope_dreams_%.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_%.c
	mkdir -p $(PIPEWORKS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PIPEWORKS_DIR)/pipeworks_%.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_%.c
	mkdir -p $(PIPEWORKS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PIPEWORKS_DIR)/lifeforms_%.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_%.c
	mkdir -p $(PIPEWORKS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIFEFORMS_DIR)/nocturne_%.o: $(ROOT)/products/savers/nocturne/src/nocturne_%.c
	mkdir -p $(LIFEFORMS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIFEFORMS_DIR)/ricochet_%.o: $(ROOT)/products/savers/ricochet/src/ricochet_%.c
	mkdir -p $(LIFEFORMS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIFEFORMS_DIR)/deepfield_%.o: $(ROOT)/products/savers/deepfield/src/deepfield_%.c
	mkdir -p $(LIFEFORMS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIFEFORMS_DIR)/ember_%.o: $(ROOT)/products/savers/ember/src/ember_%.c
	mkdir -p $(LIFEFORMS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIFEFORMS_DIR)/oscilloscope_dreams_%.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_%.c
	mkdir -p $(LIFEFORMS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIFEFORMS_DIR)/pipeworks_%.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_%.c
	mkdir -p $(LIFEFORMS_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIFEFORMS_DIR)/lifeforms_%.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_%.c
	mkdir -p $(LIFEFORMS_DIR)
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

$(BENCHLAB_DIR)/ember_%.o: $(ROOT)/products/savers/ember/src/ember_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/oscilloscope_dreams_%.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/pipeworks_%.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_%.c
	mkdir -p $(BENCHLAB_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BENCHLAB_DIR)/lifeforms_%.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_%.c
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

$(NOCTURNE_DIR)/ember_config_res.o: $(ROOT)/products/savers/ember/src/ember_config.rc $(ROOT)/products/savers/ember/src/ember_resource.h
	mkdir -p $(NOCTURNE_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ember/src -i $< -o $@

$(NOCTURNE_DIR)/oscilloscope_dreams_config_res.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_config.rc $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_resource.h
	mkdir -p $(NOCTURNE_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/oscilloscope_dreams/src -i $< -o $@

$(NOCTURNE_DIR)/pipeworks_config_res.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_config.rc $(ROOT)/products/savers/pipeworks/src/pipeworks_resource.h
	mkdir -p $(NOCTURNE_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/pipeworks/src -i $< -o $@

$(NOCTURNE_DIR)/lifeforms_config_res.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_config.rc $(ROOT)/products/savers/lifeforms/src/lifeforms_resource.h
	mkdir -p $(NOCTURNE_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/lifeforms/src -i $< -o $@

$(RICOCHET_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc $(ROOT)/products/savers/nocturne/src/nocturne_resource.h
	mkdir -p $(RICOCHET_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/nocturne/src -i $< -o $@

$(RICOCHET_DIR)/ricochet_config_res.o: $(ROOT)/products/savers/ricochet/src/ricochet_config.rc $(ROOT)/products/savers/ricochet/src/ricochet_resource.h
	mkdir -p $(RICOCHET_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ricochet/src -i $< -o $@

$(RICOCHET_DIR)/deepfield_config_res.o: $(ROOT)/products/savers/deepfield/src/deepfield_config.rc $(ROOT)/products/savers/deepfield/src/deepfield_resource.h
	mkdir -p $(RICOCHET_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/deepfield/src -i $< -o $@

$(RICOCHET_DIR)/ember_config_res.o: $(ROOT)/products/savers/ember/src/ember_config.rc $(ROOT)/products/savers/ember/src/ember_resource.h
	mkdir -p $(RICOCHET_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ember/src -i $< -o $@

$(RICOCHET_DIR)/oscilloscope_dreams_config_res.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_config.rc $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_resource.h
	mkdir -p $(RICOCHET_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/oscilloscope_dreams/src -i $< -o $@

$(RICOCHET_DIR)/pipeworks_config_res.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_config.rc $(ROOT)/products/savers/pipeworks/src/pipeworks_resource.h
	mkdir -p $(RICOCHET_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/pipeworks/src -i $< -o $@

$(RICOCHET_DIR)/lifeforms_config_res.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_config.rc $(ROOT)/products/savers/lifeforms/src/lifeforms_resource.h
	mkdir -p $(RICOCHET_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/lifeforms/src -i $< -o $@

$(DEEPFIELD_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc $(ROOT)/products/savers/nocturne/src/nocturne_resource.h
	mkdir -p $(DEEPFIELD_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/nocturne/src -i $< -o $@

$(DEEPFIELD_DIR)/ricochet_config_res.o: $(ROOT)/products/savers/ricochet/src/ricochet_config.rc $(ROOT)/products/savers/ricochet/src/ricochet_resource.h
	mkdir -p $(DEEPFIELD_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ricochet/src -i $< -o $@

$(DEEPFIELD_DIR)/deepfield_config_res.o: $(ROOT)/products/savers/deepfield/src/deepfield_config.rc $(ROOT)/products/savers/deepfield/src/deepfield_resource.h
	mkdir -p $(DEEPFIELD_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/deepfield/src -i $< -o $@

$(DEEPFIELD_DIR)/ember_config_res.o: $(ROOT)/products/savers/ember/src/ember_config.rc $(ROOT)/products/savers/ember/src/ember_resource.h
	mkdir -p $(DEEPFIELD_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ember/src -i $< -o $@

$(DEEPFIELD_DIR)/oscilloscope_dreams_config_res.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_config.rc $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_resource.h
	mkdir -p $(DEEPFIELD_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/oscilloscope_dreams/src -i $< -o $@

$(DEEPFIELD_DIR)/pipeworks_config_res.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_config.rc $(ROOT)/products/savers/pipeworks/src/pipeworks_resource.h
	mkdir -p $(DEEPFIELD_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/pipeworks/src -i $< -o $@

$(DEEPFIELD_DIR)/lifeforms_config_res.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_config.rc $(ROOT)/products/savers/lifeforms/src/lifeforms_resource.h
	mkdir -p $(DEEPFIELD_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/lifeforms/src -i $< -o $@

$(EMBER_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc $(ROOT)/products/savers/nocturne/src/nocturne_resource.h
	mkdir -p $(EMBER_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/nocturne/src -i $< -o $@

$(EMBER_DIR)/ricochet_config_res.o: $(ROOT)/products/savers/ricochet/src/ricochet_config.rc $(ROOT)/products/savers/ricochet/src/ricochet_resource.h
	mkdir -p $(EMBER_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ricochet/src -i $< -o $@

$(EMBER_DIR)/deepfield_config_res.o: $(ROOT)/products/savers/deepfield/src/deepfield_config.rc $(ROOT)/products/savers/deepfield/src/deepfield_resource.h
	mkdir -p $(EMBER_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/deepfield/src -i $< -o $@

$(EMBER_DIR)/ember_config_res.o: $(ROOT)/products/savers/ember/src/ember_config.rc $(ROOT)/products/savers/ember/src/ember_resource.h
	mkdir -p $(EMBER_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ember/src -i $< -o $@

$(EMBER_DIR)/oscilloscope_dreams_config_res.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_config.rc $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_resource.h
	mkdir -p $(EMBER_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/oscilloscope_dreams/src -i $< -o $@

$(EMBER_DIR)/pipeworks_config_res.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_config.rc $(ROOT)/products/savers/pipeworks/src/pipeworks_resource.h
	mkdir -p $(EMBER_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/pipeworks/src -i $< -o $@

$(EMBER_DIR)/lifeforms_config_res.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_config.rc $(ROOT)/products/savers/lifeforms/src/lifeforms_resource.h
	mkdir -p $(EMBER_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/lifeforms/src -i $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc $(ROOT)/products/savers/nocturne/src/nocturne_resource.h
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/nocturne/src -i $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/ricochet_config_res.o: $(ROOT)/products/savers/ricochet/src/ricochet_config.rc $(ROOT)/products/savers/ricochet/src/ricochet_resource.h
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ricochet/src -i $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/deepfield_config_res.o: $(ROOT)/products/savers/deepfield/src/deepfield_config.rc $(ROOT)/products/savers/deepfield/src/deepfield_resource.h
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/deepfield/src -i $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/ember_config_res.o: $(ROOT)/products/savers/ember/src/ember_config.rc $(ROOT)/products/savers/ember/src/ember_resource.h
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ember/src -i $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/oscilloscope_dreams_config_res.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_config.rc $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_resource.h
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/oscilloscope_dreams/src -i $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/pipeworks_config_res.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_config.rc $(ROOT)/products/savers/pipeworks/src/pipeworks_resource.h
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/pipeworks/src -i $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/lifeforms_config_res.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_config.rc $(ROOT)/products/savers/lifeforms/src/lifeforms_resource.h
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/lifeforms/src -i $< -o $@

$(PIPEWORKS_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc $(ROOT)/products/savers/nocturne/src/nocturne_resource.h
	mkdir -p $(PIPEWORKS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/nocturne/src -i $< -o $@

$(PIPEWORKS_DIR)/ricochet_config_res.o: $(ROOT)/products/savers/ricochet/src/ricochet_config.rc $(ROOT)/products/savers/ricochet/src/ricochet_resource.h
	mkdir -p $(PIPEWORKS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ricochet/src -i $< -o $@

$(PIPEWORKS_DIR)/deepfield_config_res.o: $(ROOT)/products/savers/deepfield/src/deepfield_config.rc $(ROOT)/products/savers/deepfield/src/deepfield_resource.h
	mkdir -p $(PIPEWORKS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/deepfield/src -i $< -o $@

$(PIPEWORKS_DIR)/ember_config_res.o: $(ROOT)/products/savers/ember/src/ember_config.rc $(ROOT)/products/savers/ember/src/ember_resource.h
	mkdir -p $(PIPEWORKS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ember/src -i $< -o $@

$(PIPEWORKS_DIR)/oscilloscope_dreams_config_res.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_config.rc $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_resource.h
	mkdir -p $(PIPEWORKS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/oscilloscope_dreams/src -i $< -o $@

$(PIPEWORKS_DIR)/pipeworks_config_res.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_config.rc $(ROOT)/products/savers/pipeworks/src/pipeworks_resource.h
	mkdir -p $(PIPEWORKS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/pipeworks/src -i $< -o $@

$(PIPEWORKS_DIR)/lifeforms_config_res.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_config.rc $(ROOT)/products/savers/lifeforms/src/lifeforms_resource.h
	mkdir -p $(PIPEWORKS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/lifeforms/src -i $< -o $@

$(LIFEFORMS_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc $(ROOT)/products/savers/nocturne/src/nocturne_resource.h
	mkdir -p $(LIFEFORMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/nocturne/src -i $< -o $@

$(LIFEFORMS_DIR)/ricochet_config_res.o: $(ROOT)/products/savers/ricochet/src/ricochet_config.rc $(ROOT)/products/savers/ricochet/src/ricochet_resource.h
	mkdir -p $(LIFEFORMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ricochet/src -i $< -o $@

$(LIFEFORMS_DIR)/deepfield_config_res.o: $(ROOT)/products/savers/deepfield/src/deepfield_config.rc $(ROOT)/products/savers/deepfield/src/deepfield_resource.h
	mkdir -p $(LIFEFORMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/deepfield/src -i $< -o $@

$(LIFEFORMS_DIR)/ember_config_res.o: $(ROOT)/products/savers/ember/src/ember_config.rc $(ROOT)/products/savers/ember/src/ember_resource.h
	mkdir -p $(LIFEFORMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ember/src -i $< -o $@

$(LIFEFORMS_DIR)/oscilloscope_dreams_config_res.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_config.rc $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_resource.h
	mkdir -p $(LIFEFORMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/oscilloscope_dreams/src -i $< -o $@

$(LIFEFORMS_DIR)/pipeworks_config_res.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_config.rc $(ROOT)/products/savers/pipeworks/src/pipeworks_resource.h
	mkdir -p $(LIFEFORMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/pipeworks/src -i $< -o $@

$(LIFEFORMS_DIR)/lifeforms_config_res.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_config.rc $(ROOT)/products/savers/lifeforms/src/lifeforms_resource.h
	mkdir -p $(LIFEFORMS_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/lifeforms/src -i $< -o $@

$(BENCHLAB_DIR)/nocturne_config_res.o: $(ROOT)/products/savers/nocturne/src/nocturne_config.rc $(ROOT)/products/savers/nocturne/src/nocturne_resource.h
	mkdir -p $(BENCHLAB_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/nocturne/src -i $< -o $@

$(BENCHLAB_DIR)/ricochet_config_res.o: $(ROOT)/products/savers/ricochet/src/ricochet_config.rc $(ROOT)/products/savers/ricochet/src/ricochet_resource.h
	mkdir -p $(BENCHLAB_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ricochet/src -i $< -o $@

$(BENCHLAB_DIR)/deepfield_config_res.o: $(ROOT)/products/savers/deepfield/src/deepfield_config.rc $(ROOT)/products/savers/deepfield/src/deepfield_resource.h
	mkdir -p $(BENCHLAB_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/deepfield/src -i $< -o $@

$(BENCHLAB_DIR)/ember_config_res.o: $(ROOT)/products/savers/ember/src/ember_config.rc $(ROOT)/products/savers/ember/src/ember_resource.h
	mkdir -p $(BENCHLAB_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/ember/src -i $< -o $@

$(BENCHLAB_DIR)/oscilloscope_dreams_config_res.o: $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_config.rc $(ROOT)/products/savers/oscilloscope_dreams/src/oscilloscope_dreams_resource.h
	mkdir -p $(BENCHLAB_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/oscilloscope_dreams/src -i $< -o $@

$(BENCHLAB_DIR)/pipeworks_config_res.o: $(ROOT)/products/savers/pipeworks/src/pipeworks_config.rc $(ROOT)/products/savers/pipeworks/src/pipeworks_resource.h
	mkdir -p $(BENCHLAB_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/pipeworks/src -i $< -o $@

$(BENCHLAB_DIR)/lifeforms_config_res.o: $(ROOT)/products/savers/lifeforms/src/lifeforms_config.rc $(ROOT)/products/savers/lifeforms/src/lifeforms_resource.h
	mkdir -p $(BENCHLAB_DIR)
	$(RC) --include-dir=$(ROOT)/products/savers/lifeforms/src -i $< -o $@

$(NOCTURNE_DIR)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc $(ROOT)/platform/src/host/win32_scr/resource.h
	mkdir -p $(NOCTURNE_DIR)
	$(RC) --include-dir=$(ROOT)/platform/src/host/win32_scr -i $< -o $@

$(RICOCHET_DIR)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc $(ROOT)/platform/src/host/win32_scr/resource.h
	mkdir -p $(RICOCHET_DIR)
	$(RC) --include-dir=$(ROOT)/platform/src/host/win32_scr -i $< -o $@

$(DEEPFIELD_DIR)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc $(ROOT)/platform/src/host/win32_scr/resource.h
	mkdir -p $(DEEPFIELD_DIR)
	$(RC) --include-dir=$(ROOT)/platform/src/host/win32_scr -i $< -o $@

$(EMBER_DIR)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc $(ROOT)/platform/src/host/win32_scr/resource.h
	mkdir -p $(EMBER_DIR)
	$(RC) --include-dir=$(ROOT)/platform/src/host/win32_scr -i $< -o $@

$(OSCILLOSCOPE_DREAMS_DIR)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc $(ROOT)/platform/src/host/win32_scr/resource.h
	mkdir -p $(OSCILLOSCOPE_DREAMS_DIR)
	$(RC) --include-dir=$(ROOT)/platform/src/host/win32_scr -i $< -o $@

$(PIPEWORKS_DIR)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc $(ROOT)/platform/src/host/win32_scr/resource.h
	mkdir -p $(PIPEWORKS_DIR)
	$(RC) --include-dir=$(ROOT)/platform/src/host/win32_scr -i $< -o $@

$(LIFEFORMS_DIR)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc $(ROOT)/platform/src/host/win32_scr/resource.h
	mkdir -p $(LIFEFORMS_DIR)
	$(RC) --include-dir=$(ROOT)/platform/src/host/win32_scr -i $< -o $@

clean:
	rm -rf $(NOCTURNE_BIN_DIR) $(RICOCHET_BIN_DIR) $(DEEPFIELD_BIN_DIR) $(EMBER_BIN_DIR) $(OSCILLOSCOPE_DREAMS_BIN_DIR) $(PIPEWORKS_BIN_DIR) $(LIFEFORMS_BIN_DIR) $(BENCHLAB_BIN_DIR) \
		$(PLATFORM_DIR) $(NOCTURNE_DIR) $(RICOCHET_DIR) $(DEEPFIELD_DIR) $(EMBER_DIR) $(OSCILLOSCOPE_DREAMS_DIR) $(PIPEWORKS_DIR) $(LIFEFORMS_DIR) $(BENCHLAB_DIR)
