# =========================================================================
# GBA Project Master Makefile (Dual Mode)
# -------------------------------------------------------------------------
# [사용법]
# 1. Release 빌드 (기본):
#    $ make
#    -> 결과: week2.gba (최적화됨, 빠름)
#
# 2. Debug 빌드:
#    $ make DEBUG=1
#    -> 결과: week2_debug.gba (디버깅 정보 포함, 변수 추적 가능)
#
# 3. Sandbox 테스트:
#    $ make test TARGET=input_test          (Release)
#    $ make test TARGET=input_test DEBUG=1  (Debug)
# =========================================================================

# -------------------------------------------------------------------------
# 1. 툴체인 설정 (Toolchain Setup)
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
# 2. 빌드 모드 설정 (Build Configuration)
# -------------------------------------------------------------------------
ARCH       := -mthumb -mthumb-interwork
COMMON_FLAGS := $(ARCH) -Wall -fno-strict-aliasing -Iinclude
LDFLAGS    := $(ARCH) -specs=gba.specs -lm

# DEBUG 변수 여부에 따라 플래그와 경로를 다르게 설정
ifdef DEBUG
    # [Debug Mode]
    # -Og: 디버깅 경험을 해치지 않는 선에서 최적화 (변수가 사라지지 않음)
    # -g3: 최대한 상세한 디버깅 정보 포함 (매크로 정보 등)
    BUILD_TYPE := debug
    CFLAGS     := $(COMMON_FLAGS) -Og -g3 -DDEBUG_MODE
    SUFFIX     := _debug
else
    # [Release Mode]
    # -O2: 일반적인 배포용 최적화 (속도와 크기 균형)
    BUILD_TYPE := release
    CFLAGS     := $(COMMON_FLAGS) -O2
    SUFFIX     :=
endif

# -------------------------------------------------------------------------
# 3. 디렉토리 설정 (Directories)
# -------------------------------------------------------------------------
# 빌드 부산물(.o, .elf)은 모드별로 다른 폴더에 저장 (build/release 또는 build/debug)
BASE_BUILD_DIR := build
BUILD_DIR      := $(BASE_BUILD_DIR)/$(BUILD_TYPE)
SRC_DIR        := src
SANDBOX_DIR    := sandbox

# -------------------------------------------------------------------------
# [핵심 로직] 메인 소스 파일 자동 감지
# -------------------------------------------------------------------------
LPAREN := (
DETECTED_MAIN := $(shell grep -l "main\s*$(LPAREN)" $(SRC_DIR)/*.c 2>/dev/null | head -n 1)

ifneq ($(DETECTED_MAIN),)
    # 파일명 추출 (예: week2)
    BASE_NAME := $(notdir $(basename $(DETECTED_MAIN)))
else
    BASE_NAME := main
endif

# 최종 결과물 이름 (예: week2.gba 또는 week2_debug.gba)
OUTPUT_NAME := $(BASE_NAME)$(SUFFIX)

# -------------------------------------------------------------------------
# Mode 1: Main Project Build
# -------------------------------------------------------------------------

SOURCES    := $(wildcard $(SRC_DIR)/*.c)
OBJECTS    := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

.PHONY: all clean test info

all: info $(OUTPUT_NAME).gba

info:
	@echo "========================================"
	@echo ">> Build Mode:    $(BUILD_TYPE)"
	@echo ">> Main Source:   $(DETECTED_MAIN)"
	@echo ">> Output Target: $(OUTPUT_NAME).gba"
	@echo ">> Object Dir:    $(BUILD_DIR)"
	@echo "========================================"

# 1. ELF 생성 (build/mode 폴더에)
$(BUILD_DIR)/$(OUTPUT_NAME).elf: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# 2. GBA 생성 (루트 폴더에, 이름에 접미사 포함)
$(OUTPUT_NAME).gba: $(BUILD_DIR)/$(OUTPUT_NAME).elf
	$(OBJCOPY) -O binary $< $@
	$(GBAFIX) $@

# 3. 컴파일
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	@mkdir -p $@

# -------------------------------------------------------------------------
# Mode 2: Sandbox Test Build
# -------------------------------------------------------------------------
test:
ifndef TARGET
	$(error "Usage: make test TARGET=filename (without .c)")
endif
	@mkdir -p $(BUILD_DIR)
	@echo ">> [Sandbox $(BUILD_TYPE)] Compiling: $(SANDBOX_DIR)/$(TARGET).c"
	
	# 컴파일
	$(CC) $(CFLAGS) -c $(SANDBOX_DIR)/$(TARGET).c -o $(BUILD_DIR)/$(TARGET).o
	
	# 링킹 (결과물 이름에도 접미사 붙음)
	$(CC) $(BUILD_DIR)/$(TARGET).o -o $(BUILD_DIR)/$(TARGET)$(SUFFIX).elf $(LDFLAGS)
	
	# GBA 추출
	$(OBJCOPY) -O binary $(BUILD_DIR)/$(TARGET)$(SUFFIX).elf $(TARGET)$(SUFFIX).gba
	$(GBAFIX) $(TARGET)$(SUFFIX).gba
	
	@echo ">> Build Success: $(TARGET)$(SUFFIX).gba created!"

# -------------------------------------------------------------------------
# Clean
# -------------------------------------------------------------------------
clean:
	@echo ">> Cleaning up all builds..."
	rm -rf $(BASE_BUILD_DIR) *.gba *.elf *.sav