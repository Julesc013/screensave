ROOT := ../../..
PROFILE ?= debug

CC ?= gcc
AR ?= ar

OUTROOT := $(ROOT)/out
BIN_DIR := $(OUTROOT)/mingw/i686/$(PROFILE)/nocturne_stub
INT_ROOT := $(OUTROOT)/intermediate/mingw/i686/$(PROFILE)
PLATFORM_DIR := $(INT_ROOT)/screensave_platform_stub
NOCTURNE_DIR := $(INT_ROOT)/nocturne_stub

PLATFORM_LIB := $(PLATFORM_DIR)/libscreensave_platform_stub.a
TARGET := $(BIN_DIR)/nocturne.scr

PLATFORM_SRCS := \
	$(ROOT)/platform/src/core/_series02_core_stub.c \
	$(ROOT)/platform/src/host/win32_scr/_series02_host_stub.c

NOCTURNE_SRCS := \
	$(ROOT)/products/savers/nocturne/src/_series02_nocturne_stub.c

PLATFORM_OBJS := \
	$(PLATFORM_DIR)/_series02_core_stub.o \
	$(PLATFORM_DIR)/_series02_host_stub.o

NOCTURNE_OBJS := \
	$(NOCTURNE_DIR)/_series02_nocturne_stub.o

CFLAGS_COMMON := -std=c89 -Wall -Wextra -pedantic -I$(ROOT)/platform/include
LDFLAGS_COMMON := -mwindows

ifeq ($(PROFILE),release)
	CFLAGS := $(CFLAGS_COMMON) -O2
else
	CFLAGS := $(CFLAGS_COMMON) -g
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(PLATFORM_LIB) $(NOCTURNE_OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS_COMMON) -o $@ $(NOCTURNE_OBJS) $(PLATFORM_LIB)

$(PLATFORM_LIB): $(PLATFORM_OBJS)
	mkdir -p $(PLATFORM_DIR)
	$(AR) rcs $@ $(PLATFORM_OBJS)

$(PLATFORM_DIR)/_series02_core_stub.o: $(ROOT)/platform/src/core/_series02_core_stub.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(PLATFORM_DIR)/_series02_host_stub.o: $(ROOT)/platform/src/host/win32_scr/_series02_host_stub.c
	mkdir -p $(PLATFORM_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NOCTURNE_DIR)/_series02_nocturne_stub.o: $(ROOT)/products/savers/nocturne/src/_series02_nocturne_stub.c
	mkdir -p $(NOCTURNE_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(PLATFORM_DIR) $(NOCTURNE_DIR)
