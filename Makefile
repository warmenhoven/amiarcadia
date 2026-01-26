STATIC_LINKING := 0
AR             ?= ar

ifeq ($(platform),)
	platform = unix
	ifeq ($(shell uname -s),)
		platform = win
	else ifneq ($(findstring Darwin,$(shell uname -s)),)
		platform = osx
		arch = intel
		ifeq ($(shell uname -p),arm)
			arch = arm
		endif
	else ifneq ($(findstring MINGW,$(shell uname -s)),)
		platform = win
	endif
endif

# system platform
system_platform = unix
ifeq ($(shell uname -s),)
	EXE_EXT = .exe
	system_platform = win
else ifneq ($(findstring Darwin,$(shell uname -s)),)
	system_platform = osx
else ifneq ($(findstring MINGW,$(shell uname -s)),)
	system_platform = win
endif

TARGET_NAME := amiarcadia
LIBRETRO_DIR := $(CURDIR)
CORE_DIR := DroidArcadia/app/src/main/cpp
LIBRETRO_COMMON_DIR := $(CURDIR)/libretro-common

CC  ?= gcc
CXX ?= g++
DEBUG ?= 0
GIT_VERSION ?= $(shell git rev-parse --short HEAD 2>/dev/null)

ifneq ($(GIT_VERSION),)
	CFLAGS += -DGIT_VERSION=\"$(GIT_VERSION)\"
endif

LIBS := -lm

###############################################################################
# Platform detection
###############################################################################

# Unix / Linux
ifneq (,$(findstring unix,$(platform)))
	TARGET := $(TARGET_NAME)_libretro.so
	fpic := -fPIC
	SHARED := -shared -Wl,--no-undefined

# macOS
else ifneq (,$(findstring osx,$(platform)))
	TARGET := $(TARGET_NAME)_libretro.dylib
	fpic := -fPIC
	SHARED := -dynamiclib
	OSXVER := $(shell sw_vers -productVersion | cut -d. -f 1)
	OSX_LT_MAVERICKS = $(shell [ $(OSXVER) -lt 10 ] && echo "NO")
	ifeq ($(OSX_LT_MAVERICKS), NO)
		fpic += -mmacosx-version-min=10.1
	endif
	ifeq ($(arch),arm)
		CFLAGS += -arch arm64
		LDFLAGS += -arch arm64
	endif
	ifeq ($(CROSS_COMPILE),1)
		TARGET_RULE = -target $(LIBRETRO_APPLE_PLATFORM) -isysroot $(LIBRETRO_APPLE_ISYSROOT)
		CFLAGS   += $(TARGET_RULE)
		LDFLAGS  += $(TARGET_RULE)
	endif

# iOS arm64
else ifeq ($(platform), ios-arm64)
	TARGET := $(TARGET_NAME)_libretro_ios.dylib
	fpic := -fPIC
	SHARED := -dynamiclib
	ifeq ($(IOSSDK),)
		IOSSDK := $(shell xcodebuild -version -sdk iphoneos Path)
	endif
	CC = clang -arch arm64 -isysroot $(IOSSDK)
	CXX = clang++ -arch arm64 -isysroot $(IOSSDK)
	CFLAGS += -DIOS -miphoneos-version-min=8.0
	LDFLAGS += -arch arm64 -miphoneos-version-min=8.0

# iOS9 (armv7)
else ifeq ($(platform), ios9)
	TARGET := $(TARGET_NAME)_libretro_ios.dylib
	fpic := -fPIC
	SHARED := -dynamiclib
	ifeq ($(IOSSDK),)
		IOSSDK := $(shell xcodebuild -version -sdk iphoneos Path)
	endif
	CC = clang -arch armv7 -isysroot $(IOSSDK)
	CXX = clang++ -arch armv7 -isysroot $(IOSSDK)
	CFLAGS += -DIOS -miphoneos-version-min=8.0
	LDFLAGS += -arch armv7 -miphoneos-version-min=8.0

# iOS (legacy armv7)
else ifeq ($(platform), ios)
	TARGET := $(TARGET_NAME)_libretro_ios.dylib
	fpic := -fPIC
	SHARED := -dynamiclib
	ifeq ($(IOSSDK),)
		IOSSDK := $(shell xcodebuild -version -sdk iphoneos Path)
	endif
	CC = clang -arch armv7 -isysroot $(IOSSDK)
	CXX = clang++ -arch armv7 -isysroot $(IOSSDK)
	CFLAGS += -DIOS

# tvOS
else ifeq ($(platform), tvos-arm64)
	TARGET := $(TARGET_NAME)_libretro_tvos.dylib
	fpic := -fPIC
	SHARED := -dynamiclib
	ifeq ($(TVOSSDK),)
		TVOSSDK := $(shell xcodebuild -version -sdk appletvos Path)
	endif
	CC = clang -arch arm64 -isysroot $(TVOSSDK)
	CXX = clang++ -arch arm64 -isysroot $(TVOSSDK)
	CFLAGS += -DIOS -mappletvos-version-min=11.0
	LDFLAGS += -arch arm64

# Nintendo Switch (libnx)
else ifeq ($(platform), libnx)
	include $(DEVKITPRO)/libnx/switch_rules
	TARGET := $(TARGET_NAME)_libretro_$(platform).a
	CFLAGS += -D__SWITCH__ -DHAVE_LIBNX -I$(LIBNX)/include/ -specs=$(LIBNX)/switch.specs
	CFLAGS += -march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE
	CFLAGS += -ffunction-sections -fdata-sections
	STATIC_LINKING := 1

# Nintendo WiiU
else ifeq ($(platform), wiiu)
	TARGET := $(TARGET_NAME)_libretro_$(platform).a
	CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
	AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT)
	CFLAGS += -DGEKKO -DWIIU -DHW_RVL -mcpu=750 -meabi -mhard-float
	CFLAGS += -ffunction-sections -fdata-sections -D__wiiu__ -D__wut__
	CFLAGS += -DMSB_FIRST
	STATIC_LINKING := 1

# Nintendo Wii
else ifeq ($(platform), wii)
	TARGET := $(TARGET_NAME)_libretro_$(platform).a
	CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
	AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT)
	CFLAGS += -DGEKKO -DHW_RVL -mrvl -mcpu=750 -meabi -mhard-float
	CFLAGS += -DMSB_FIRST
	STATIC_LINKING := 1

# Nintendo GameCube
else ifeq ($(platform), ngc)
	TARGET := $(TARGET_NAME)_libretro_$(platform).a
	CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
	AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT)
	CFLAGS += -DGEKKO -DHW_DOL -mrvl -mcpu=750 -meabi -mhard-float
	CFLAGS += -DMSB_FIRST
	STATIC_LINKING := 1

# Nintendo 3DS
else ifeq ($(platform), ctr)
	TARGET := $(TARGET_NAME)_libretro_$(platform).a
	CC = $(DEVKITARM)/bin/arm-none-eabi-gcc$(EXE_EXT)
	AR = $(DEVKITARM)/bin/arm-none-eabi-ar$(EXE_EXT)
	CFLAGS += -DARM11 -D_3DS
	CFLAGS += -march=armv6k -mtune=mpcore -mfloat-abi=hard
	CFLAGS += -mword-relocations -ffast-math
	STATIC_LINKING := 1

# PlayStation Vita
else ifeq ($(platform), vita)
	TARGET := $(TARGET_NAME)_libretro_$(platform).a
	CC = arm-vita-eabi-gcc
	AR = arm-vita-eabi-ar
	CFLAGS += -O3
	STATIC_LINKING := 1

# PlayStation Portable
else ifeq ($(platform), psp1)
	TARGET := $(TARGET_NAME)_libretro_$(platform).a
	CC = psp-gcc
	AR = psp-ar
	CFLAGS += -G0
	STATIC_LINKING := 1

# PlayStation 2
else ifeq ($(platform), ps2)
	TARGET := $(TARGET_NAME)_libretro_$(platform).a
	CC = mips64r5900el-ps2-elf-gcc
	AR = mips64r5900el-ps2-elf-ar
	CFLAGS += -G0 -DPS2
	STATIC_LINKING := 1

# GCW0 / OpenDingux (MIPS)
else ifeq ($(platform), gcw0)
	TARGET := $(TARGET_NAME)_libretro.so
	CC = /opt/gcw0-toolchain/usr/bin/mipsel-linux-gcc
	AR = /opt/gcw0-toolchain/usr/bin/mipsel-linux-ar
	fpic := -fPIC
	SHARED := -shared -Wl,--no-undefined
	CFLAGS += -march=mips32 -mtune=mips32r2 -mhard-float

# RetroFW (MIPS)
else ifeq ($(platform), retrofw)
	TARGET := $(TARGET_NAME)_libretro.so
	CC = /opt/retrofw-toolchain/usr/bin/mipsel-linux-gcc
	AR = /opt/retrofw-toolchain/usr/bin/mipsel-linux-ar
	fpic := -fPIC
	SHARED := -shared -Wl,--no-undefined
	CFLAGS += -march=mips32 -mtune=mips32 -mhard-float

# Miyoo (ARM)
else ifeq ($(platform), miyoo)
	TARGET := $(TARGET_NAME)_libretro.so
	CC = /opt/miyoo/usr/bin/arm-linux-gcc
	AR = /opt/miyoo/usr/bin/arm-linux-ar
	fpic := -fPIC
	SHARED := -shared -Wl,--no-undefined
	CFLAGS += -mcpu=arm926ej-s -ffast-math

# Windows (MinGW)
else ifeq ($(platform), win)
	TARGET := $(TARGET_NAME)_libretro.dll
	SHARED := -shared -static-libgcc -static-libstdc++ -Wl,--no-undefined
	LDFLAGS += -lws2_32
	LIBS :=

# Windows 32-bit (MinGW)
else ifeq ($(platform), win32)
	TARGET := $(TARGET_NAME)_libretro.dll
	SHARED := -shared -static-libgcc -static-libstdc++ -Wl,--no-undefined
	LDFLAGS += -lws2_32
	LIBS :=

# Default fallback (unix-like)
else
	TARGET := $(TARGET_NAME)_libretro.so
	fpic := -fPIC
	SHARED := -shared -Wl,--no-undefined
endif

###############################################################################
# Compiler flags
###############################################################################

ifeq ($(DEBUG), 1)
	CFLAGS += -O0 -g -DDEBUG
else
	CFLAGS += -O2 -DNDEBUG
endif

CFLAGS += -DLIBRETRO
CFLAGS += -I$(LIBRETRO_COMMON_DIR)/include
CFLAGS += -I$(CORE_DIR)
CFLAGS += $(fpic)
CFLAGS += -Wall

LDFLAGS += $(fpic) $(LIBS)

###############################################################################
# Sources
###############################################################################

SOURCES := \
	src/libretro.c \
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

OBJECTS := $(SOURCES:.c=.o)

###############################################################################
# Targets
###############################################################################

all: $(TARGET)

$(TARGET): $(OBJECTS)
ifeq ($(STATIC_LINKING), 1)
	$(AR) rcs $@ $(OBJECTS)
else
	$(CC) -o $@ $(SHARED) $(OBJECTS) $(LDFLAGS)
endif

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
