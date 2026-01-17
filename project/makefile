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

# # =========================================================================
# # GBA Project Makefile
# # -------------------------------------------------------------------------
# # 기능:
# # 1. 'make': src 폴더의 모든 코드를 합쳐서 main.gba 생성
# # 2. 'make test TARGET=xxx': sandbox 폴더의 xxx.c 하나만 컴파일해서 xxx.gba 생성
# # 3. 'make clean': 빌드된 파일들 청소
# # =========================================================================

# # -------------------------------------------------------------------------
# # 1. 툴체인 설정 (Toolchain Setup)
# # 환경 변수 DEVKITPRO가 설정되어 있어야 합니다.
# # -------------------------------------------------------------------------
# ifeq ($(strip $(DEVKITPRO)),)
# $(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>devkitPro")
# endif

# DEVKITARM  := $(DEVKITPRO)/devkitARM
# PREFIX     := $(DEVKITARM)/bin/arm-none-eabi-
# CC         := $(PREFIX)gcc
# OBJCOPY    := $(PREFIX)objcopy
# GBAFIX     := $(DEVKITPRO)/tools/bin/gbafix

# # -------------------------------------------------------------------------
# # 2. 컴파일 옵션 (Flags)
# # -------------------------------------------------------------------------
# # -mthumb-interwork: ARM 모드와 Thumb 모드 간 전환 허용 (GBA 필수)
# # -specs=gba.specs: GBA용 링커 스크립트 및 라이브러리 사용
# # -O2: 최적화 레벨 2 (속도와 크기 균형)
# # -Iinclude: 헤더 파일을 include 폴더에서 찾도록 설정
# # -------------------------------------------------------------------------
# ARCH       := -mthumb -mthumb-interwork
# CFLAGS     := $(ARCH) -O2 -Wall -fno-strict-aliasing -Iinclude
# LDFLAGS    := $(ARCH) -specs=gba.specs

# # -------------------------------------------------------------------------
# # 3. 디렉토리 설정 (Directories)
# # -------------------------------------------------------------------------
# BUILD_DIR  := build
# SRC_DIR    := src
# SANDBOX_DIR:= sandbox
# OUTPUT     := main

# # -------------------------------------------------------------------------
# # Mode 1: Main Project Build (Default)
# # src 폴더 안의 모든 .c 파일을 찾아서 하나로 뭉칩니다.
# # -------------------------------------------------------------------------

# # src 폴더의 모든 .c 파일 목록 (예: src/main.c src/player.c ...)
# SOURCES    := $(wildcard $(SRC_DIR)/*.c)
# # .c 파일 목록을 .o 파일 목록으로 변환 (예: build/main.o build/player.o ...)
# OBJECTS    := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

# .PHONY: all clean test

# # 기본 타겟: main.gba 만들기
# all: $(BUILD_DIR) $(OUTPUT).gba

# # 1단계: .elf 파일 생성 (링킹)
# $(OUTPUT).elf: $(OBJECTS)
# 	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# # 2단계: .gba 파일 추출 (바이너리 카피)
# $(OUTPUT).gba: $(OUTPUT).elf
# 	$(OBJCOPY) -O binary $< $@
# 	$(GBAFIX) $@

# # 3단계: .c -> .o 컴파일
# $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# # 빌드 폴더 생성
# $(BUILD_DIR):
# 	@mkdir -p $@

# # -------------------------------------------------------------------------
# # Mode 2: Sandbox Test Build
# # 사용법: make test TARGET=파일명 (확장자 제외)
# # 예시: make test TARGET=input_test
# # -------------------------------------------------------------------------

# test:
# ifndef TARGET
# 	$(error "Usage: make test TARGET=filename (without .c)")
# endif
# 	@mkdir -p $(BUILD_DIR)
# 	@echo ">> Compiling Sandbox: $(SANDBOX_DIR)/$(TARGET).c"
# 	$(CC) $(CFLAGS) -c $(SANDBOX_DIR)/$(TARGET).c -o $(BUILD_DIR)/$(TARGET).o
# 	$(CC) $(BUILD_DIR)/$(TARGET).o -o $(BUILD_DIR)/$(TARGET).elf $(LDFLAGS)
# 	$(OBJCOPY) -O binary $(BUILD_DIR)/$(TARGET).elf $(TARGET).gba
# 	$(GBAFIX) $(TARGET).gba
# 	@echo ">> Build Success: $(TARGET).gba created!"

# # -------------------------------------------------------------------------
# # Cleanup
# # -------------------------------------------------------------------------

# clean:
# 	rm -rf $(BUILD_DIR) *.gba *.elf *.sav

# =========================================================================
# GBA Project Master Makefile
# -------------------------------------------------------------------------
# 기능 설명:
# 1. [자동 이름 감지]: src 폴더 내에 'main()' 함수가 포함된 파일을 찾아,
#    그 파일의 이름으로 .gba 파일을 생성합니다. (예: week2.c -> week2.gba)
# 2. [프로젝트 빌드]: 'make' 입력 시 src 폴더의 모든 코드를 합쳐 빌드합니다.
# 3. [샌드박스 테스트]: 'make test TARGET=xxx' 입력 시 sandbox 폴더의 파일만 단독 빌드합니다.
# 4. [구조적 관리]: 헤더(include), 소스(src), 빌드 부산물(build)을 분리하여 관리합니다.
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
# -mthumb-interwork: ARM 모드와 Thumb 모드 간의 함수 호출을 가능하게 함 (GBA 필수)
# -specs=gba.specs: GBA 전용 링커 스크립트 및 하드웨어 정의 사용
# -O2: 최적화 레벨 2 (속도와 용량의 균형, GBA 개발 표준)
# -Wall: 모든 경고 출력 (버그 방지)
# -Iinclude: 헤더 파일(.h)을 'include' 폴더에서 찾도록 설정
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

# -------------------------------------------------------------------------
# [핵심 로직] 메인 소스 파일 자동 감지 (Auto-Detect Main)
# -------------------------------------------------------------------------
# 1. Make의 파싱 에러 방지를 위해 괄호를 변수로 정의합니다.
LPAREN := (

# 2. grep: src 폴더의 모든 .c 파일 중 "main"과 "(" 가 있는 파일을 찾음
#    $(LPAREN)을 사용하여 Make가 괄호 개수를 잘못 세지 않도록 함
DETECTED_MAIN := $(shell grep -l "main\s*$(LPAREN)" $(SRC_DIR)/*.c 2>/dev/null | head -n 1)

ifneq ($(DETECTED_MAIN),)
    # main 함수가 있는 파일을 찾았다면, 그 파일 이름을 결과물 이름으로 사용
    OUTPUT := $(notdir $(basename $(DETECTED_MAIN)))
else
    # 못 찾았다면 기본값 'main' 사용
    OUTPUT := main
endif

# -------------------------------------------------------------------------
# Mode 1: Main Project Build (Default)
# -------------------------------------------------------------------------

SOURCES    := $(wildcard $(SRC_DIR)/*.c)
OBJECTS    := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

.PHONY: all clean test info

# 기본 타겟
all: info $(BUILD_DIR) $(OUTPUT).gba

# 정보 출력
info:
	@echo "========================================"
	@echo ">> Main Source Found: $(DETECTED_MAIN)"
	@echo ">> Target Output:     $(OUTPUT).gba"
	@echo "========================================"

# [수정됨] 1단계: .elf 파일 생성 (Linking) -> build 폴더에 생성하도록 경로 추가
$(BUILD_DIR)/$(OUTPUT).elf: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# [수정됨] 2단계: .gba 파일 추출 (Object Copy) -> build 폴더에 있는 .elf를 참조
$(OUTPUT).gba: $(BUILD_DIR)/$(OUTPUT).elf
	$(OBJCOPY) -O binary $< $@
	$(GBAFIX) $@

# 3단계: .c -> .o 컴파일 (그대로 유지)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 빌드 디렉토리 생성 (그대로 유지)
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
	@echo ">> [Sandbox Mode] Compiling: $(SANDBOX_DIR)/$(TARGET).c"
	# 샌드박스 파일 하나만 따로 컴파일
	$(CC) $(CFLAGS) -c $(SANDBOX_DIR)/$(TARGET).c -o $(BUILD_DIR)/$(TARGET).o
	# 링킹
	$(CC) $(BUILD_DIR)/$(TARGET).o -o $(BUILD_DIR)/$(TARGET).elf $(LDFLAGS)
	# GBA 변환 및 헤더 수정
	$(OBJCOPY) -O binary $(BUILD_DIR)/$(TARGET).elf $(TARGET).gba
	$(GBAFIX) $(TARGET).gba
	@echo ">> Build Success: $(TARGET).gba created!"

# -------------------------------------------------------------------------
# Cleanup (청소)
# -------------------------------------------------------------------------
clean:
	@echo ">> Cleaning up build artifacts..."
	rm -rf $(BUILD_DIR) *.gba *.elf *.sav