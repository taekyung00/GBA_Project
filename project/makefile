# # -------------------------------------------------------------------------
# #  Universal GBA Makefile (Final Fix)
# #  설명: .SECONDARY 뒤를 비워서 모든 중간 파일 삭제를 강제로 막음
# # -------------------------------------------------------------------------

# # [0. Make Config]
# MAKEFLAGS += --no-builtin-rules
# .SUFFIXES:
# # 중요: 뒤에 아무것도 안 적으면 "모든 타겟을 보존하라"는 뜻이 됩니다.
# .SECONDARY:

# # [1. Path Settings]
# ifeq ($(OS),Windows_NT)
#     DEVKITPRO ?= C:/devkitPro
# else
#     DEVKITPRO ?= /opt/devkitpro
# endif

# DEVKITARM ?= $(DEVKITPRO)/devkitARM
# BIN_PATH = $(DEVKITARM)/bin
# PREFIX   = arm-none-eabi-

# # [2. Tool Definitions]
# CC      = $(BIN_PATH)/$(PREFIX)gcc
# OBJCOPY = $(BIN_PATH)/$(PREFIX)objcopy
# FIX     = $(DEVKITPRO)/tools/bin/gbafix

# # [3. Compilation Flags]
# ARCH    = -mthumb -mthumb-interwork
# SPECS   = -specs=$(DEVKITARM)/arm-none-eabi/lib/gba.specs
# CFLAGS  = $(ARCH) $(SPECS) -O2 -Wall

# # [4. Build Rules]

# # 기본 타겟
# all: main.gba

# # (1) 바로가기 규칙
# %: %.gba
# 	@echo "Build Complete: $@"

# # (2) .elf -> .gba
# %.gba: %.elf
# 	@echo "  [OBJCOPY] $@"
# 	$(OBJCOPY) -v -O binary $< $@
# 	@echo "  [GBAFIX]  $@"
# 	$(FIX) $@ -p -t$(basename $@)

# # (3) .c -> .elf
# %.elf: %.c
# 	@echo "  [COMPILE] $<"
# 	$(CC) $(CFLAGS) -o $@ $<

# # [5. Clean Up]
# clean:
# 	@echo "Cleaning up..."
# 	rm -f *.elf *.gba *.o *.sav

# =========================================================================
# GBA Project Makefile
# -------------------------------------------------------------------------
# 기능:
# 1. 'make': src 폴더의 모든 코드를 합쳐서 main.gba 생성
# 2. 'make test TARGET=xxx': sandbox 폴더의 xxx.c 하나만 컴파일해서 xxx.gba 생성
# 3. 'make clean': 빌드된 파일들 청소
# =========================================================================

# -------------------------------------------------------------------------
# 1. 툴체인 설정 (Toolchain Setup)
# 환경 변수 DEVKITPRO가 설정되어 있어야 합니다.
# -------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
endif

DEVKITARM  := $(DEVKITPRO)/devkitARM
PREFIX     := $(DEVKITARM)/bin/arm-none-eabi-
CC         := $(PREFIX)gcc
OBJCOPY    := $(PREFIX)objcopy
GBAFIX     := $(DEVKITPRO)/tools/bin/gbafix

# -------------------------------------------------------------------------
# 2. 컴파일 옵션 (Flags)
# -------------------------------------------------------------------------
# -mthumb-interwork: ARM 모드와 Thumb 모드 간 전환 허용 (GBA 필수)
# -specs=gba.specs: GBA용 링커 스크립트 및 라이브러리 사용
# -O2: 최적화 레벨 2 (속도와 크기 균형)
# -Iinclude: 헤더 파일을 include 폴더에서 찾도록 설정
# -------------------------------------------------------------------------
ARCH       := -mthumb -mthumb-interwork
CFLAGS     := $(ARCH) -O2 -Wall -fno-strict-aliasing -Iinclude
LDFLAGS    := $(ARCH) -specs=gba.specs

# -------------------------------------------------------------------------
# 3. 디렉토리 설정 (Directories)
# -------------------------------------------------------------------------
BUILD_DIR  := build
SRC_DIR    := src
SANDBOX_DIR:= sandbox
OUTPUT     := main

# -------------------------------------------------------------------------
# Mode 1: Main Project Build (Default)
# src 폴더 안의 모든 .c 파일을 찾아서 하나로 뭉칩니다.
# -------------------------------------------------------------------------

# src 폴더의 모든 .c 파일 목록 (예: src/main.c src/player.c ...)
SOURCES    := $(wildcard $(SRC_DIR)/*.c)
# .c 파일 목록을 .o 파일 목록으로 변환 (예: build/main.o build/player.o ...)
OBJECTS    := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

.PHONY: all clean test

# 기본 타겟: main.gba 만들기
all: $(BUILD_DIR) $(OUTPUT).gba

# 1단계: .elf 파일 생성 (링킹)
$(OUTPUT).elf: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# 2단계: .gba 파일 추출 (바이너리 카피)
$(OUTPUT).gba: $(OUTPUT).elf
	$(OBJCOPY) -O binary $< $@
	$(GBAFIX) $@

# 3단계: .c -> .o 컴파일
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 빌드 폴더 생성
$(BUILD_DIR):
	@mkdir -p $@

# -------------------------------------------------------------------------
# Mode 2: Sandbox Test Build
# 사용법: make test TARGET=파일명 (확장자 제외)
# 예시: make test TARGET=input_test
# -------------------------------------------------------------------------

test:
ifndef TARGET
	$(error "Usage: make test TARGET=filename (without .c)")
endif
	@mkdir -p $(BUILD_DIR)
	@echo ">> Compiling Sandbox: $(SANDBOX_DIR)/$(TARGET).c"
	$(CC) $(CFLAGS) -c $(SANDBOX_DIR)/$(TARGET).c -o $(BUILD_DIR)/$(TARGET).o
	$(CC) $(BUILD_DIR)/$(TARGET).o -o $(BUILD_DIR)/$(TARGET).elf $(LDFLAGS)
	$(OBJCOPY) -O binary $(BUILD_DIR)/$(TARGET).elf $(TARGET).gba
	$(GBAFIX) $(TARGET).gba
	@echo ">> Build Success: $(TARGET).gba created!"

# -------------------------------------------------------------------------
# Cleanup
# -------------------------------------------------------------------------

clean:
	rm -rf $(BUILD_DIR) *.gba *.elf *.sav