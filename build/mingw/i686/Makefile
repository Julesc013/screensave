ROOT := ../../..
PROFILE ?= debug

CC ?= gcc
AR ?= ar
RC ?= windres

OUTROOT := $(ROOT)/out
BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/nocturne
INT_ROOT := $(OUTROOT)/intermediate/mingw/i686/$(PROFILE)
PLATFORM_DIR := $(INT_ROOT)/screensave_platform
NOCTURNE_DIR := $(INT_ROOT)/nocturne

PLATFORM_LIB := $(PLATFORM_DIR)/libscreensave_platform.a
TARGET := $(BIN_DIR)/nocturne.scr

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
	$(ROOT)/products/savers/nocturne/src/nocturne_entry.c

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
	$(NOCTURNE_DIR)/nocturne_entry.o \
	$(NOCTURNE_DIR)/screensave_host_res.o

CFLAGS_COMMON := -std=c89 -Wall -Wextra -pedantic -I$(ROOT)/platform/include
LDLIBS_COMMON := -ladvapi32 -lgdi32 -luser32

ifeq ($(PROFILE),release)
	CFLAGS := $(CFLAGS_COMMON) -O2
else
	CFLAGS := $(CFLAGS_COMMON) -g
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(PLATFORM_LIB) $(NOCTURNE_OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -mwindows -o $@ $(NOCTURNE_OBJS) $(PLATFORM_LIB) $(LDLIBS_COMMON)

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

$(NOCTURNE_DIR)/nocturne_entry.o: $(ROOT)/products/savers/nocturne/src/nocturne_entry.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/screensave_host_res.o: $(ROOT)/platform/src/host/win32_scr/screensave_host.rc $(ROOT)/platform/src/host/win32_scr/resource.h
	mkdir -p $(NOCTURNE_DIR)
	$(RC) --include-dir=$(ROOT)/platform/src/host/win32_scr -i $< -o $@

clean:
	rm -rf $(BIN_DIR) $(PLATFORM_DIR) $(NOCTURNE_DIR)
