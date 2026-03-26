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
	$(ROOT)/platform/src/core/_series02_core_stub.c \
	$(ROOT)/platform/src/host/win32_scr/scr_args.c \
	$(ROOT)/platform/src/host/win32_scr/scr_config_dialog.c \
	$(ROOT)/platform/src/host/win32_scr/scr_diagnostics.c \
	$(ROOT)/platform/src/host/win32_scr/scr_entry.c \
	$(ROOT)/platform/src/host/win32_scr/scr_settings.c \
	$(ROOT)/platform/src/host/win32_scr/scr_window.c

NOCTURNE_SRCS := \
	$(ROOT)/products/savers/nocturne/src/nocturne_entry.c

PLATFORM_OBJS := \
	$(PLATFORM_DIR)/_series02_core_stub.o \
	$(PLATFORM_DIR)/scr_args.o \
	$(PLATFORM_DIR)/scr_config_dialog.o \
	$(PLATFORM_DIR)/scr_diagnostics.o \
	$(PLATFORM_DIR)/scr_entry.o \
	$(PLATFORM_DIR)/scr_settings.o \
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

$(PLATFORM_DIR)/_series02_core_stub.o: $(ROOT)/platform/src/core/_series02_core_stub.c
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
