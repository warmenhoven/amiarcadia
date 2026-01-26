LOCAL_PATH := $(call my-dir)

ROOT_DIR     := $(LOCAL_PATH)/..
CORE_DIR     := $(ROOT_DIR)/DroidArcadia/app/src/main/cpp
LIBRETRO_DIR := $(ROOT_DIR)/libretro-common

COREFLAGS := -DLIBRETRO -DFRONTEND_SUPPORTS_RGB565

GIT_VERSION := " $(shell git rev-parse --short HEAD || echo unknown)"
ifneq ($(GIT_VERSION)," unknown")
	COREFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
endif

include $(CLEAR_VARS)
LOCAL_MODULE    := retro
LOCAL_SRC_FILES := \
	$(ROOT_DIR)/src/libretro.c \
	$(CORE_DIR)/android.c \
	$(CORE_DIR)/2650.c \
	$(CORE_DIR)/arcadia.c \
	$(CORE_DIR)/coin-ops.c \
	$(CORE_DIR)/elektor.c \
	$(CORE_DIR)/formats.c \
	$(CORE_DIR)/interton.c \
	$(CORE_DIR)/malzak.c \
	$(CORE_DIR)/pvi.c \
	$(CORE_DIR)/zaccaria.c
LOCAL_C_INCLUDES := \
	$(LIBRETRO_DIR)/include \
	$(CORE_DIR)
LOCAL_CFLAGS    := $(COREFLAGS)
LOCAL_LDLIBS    := -llog -lm
include $(BUILD_SHARED_LIBRARY)
