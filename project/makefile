# # =========================================================================
# # GBA Project Master Makefile
# # -------------------------------------------------------------------------
# # 기능 설명:
# # 1. [자동 이름 감지]: src 폴더 내에 'main()' 함수가 포함된 파일을 찾아,
# #    그 파일의 이름으로 .gba 파일을 생성합니다. (예: week2.c -> week2.gba)
# # 2. [프로젝트 빌드]: 'make' 입력 시 src 폴더의 모든 코드를 합쳐 빌드합니다.
# # 3. [샌드박스 테스트]: 'make test TARGET=xxx' 입력 시 sandbox 폴더의 파일만 단독 빌드합니다.
# # 4. [구조적 관리]: 헤더(include), 소스(src), 빌드 부산물(build)을 분리하여 관리합니다.
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
# # -mthumb-interwork: ARM 모드와 Thumb 모드 간의 함수 호출을 가능하게 함 (GBA 필수)
# # -specs=gba.specs: GBA 전용 링커 스크립트 및 하드웨어 정의 사용
# # -O2: 최적화 레벨 2 (속도와 용량의 균형, GBA 개발 표준)
# # -Wall: 모든 경고 출력 (버그 방지)
# # -Iinclude: 헤더 파일(.h)을 'include' 폴더에서 찾도록 설정
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

# # -------------------------------------------------------------------------
# # [핵심 로직] 메인 소스 파일 자동 감지 (Auto-Detect Main)
# # -------------------------------------------------------------------------
# # 1. Make의 파싱 에러 방지를 위해 괄호를 변수로 정의합니다.
# LPAREN := (

# # 2. grep: src 폴더의 모든 .c 파일 중 "main"과 "(" 가 있는 파일을 찾음
# #    $(LPAREN)을 사용하여 Make가 괄호 개수를 잘못 세지 않도록 함
# DETECTED_MAIN := $(shell grep -l "main\s*$(LPAREN)" $(SRC_DIR)/*.c 2>/dev/null | head -n 1)

# ifneq ($(DETECTED_MAIN),)
#     # main 함수가 있는 파일을 찾았다면, 그 파일 이름을 결과물 이름으로 사용
#     OUTPUT := $(notdir $(basename $(DETECTED_MAIN)))
# else
#     # 못 찾았다면 기본값 'main' 사용
#     OUTPUT := main
# endif

# # -------------------------------------------------------------------------
# # Mode 1: Main Project Build (Default)
# # -------------------------------------------------------------------------

# SOURCES    := $(wildcard $(SRC_DIR)/*.c)
# OBJECTS    := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

# .PHONY: all clean test info

# # 기본 타겟
# all: info $(BUILD_DIR) $(OUTPUT).gba

# # 정보 출력
# info:
# 	@echo "========================================"
# 	@echo ">> Main Source Found: $(DETECTED_MAIN)"
# 	@echo ">> Target Output:     $(OUTPUT).gba"
# 	@echo "========================================"

# # [수정됨] 1단계: .elf 파일 생성 (Linking) -> build 폴더에 생성하도록 경로 추가
# $(BUILD_DIR)/$(OUTPUT).elf: $(OBJECTS)
# 	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# # [수정됨] 2단계: .gba 파일 추출 (Object Copy) -> build 폴더에 있는 .elf를 참조
# $(OUTPUT).gba: $(BUILD_DIR)/$(OUTPUT).elf
# 	$(OBJCOPY) -O binary $< $@
# 	$(GBAFIX) $@

# # 3단계: .c -> .o 컴파일 (그대로 유지)
# $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# # 빌드 디렉토리 생성 (그대로 유지)
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
# 	@echo ">> [Sandbox Mode] Compiling: $(SANDBOX_DIR)/$(TARGET).c"
# 	# 샌드박스 파일 하나만 따로 컴파일
# 	$(CC) $(CFLAGS) -c $(SANDBOX_DIR)/$(TARGET).c -o $(BUILD_DIR)/$(TARGET).o
# 	# 링킹
# 	$(CC) $(BUILD_DIR)/$(TARGET).o -o $(BUILD_DIR)/$(TARGET).elf $(LDFLAGS)
# 	# GBA 변환 및 헤더 수정
# 	$(OBJCOPY) -O binary $(BUILD_DIR)/$(TARGET).elf $(TARGET).gba
# 	$(GBAFIX) $(TARGET).gba
# 	@echo ">> Build Success: $(TARGET).gba created!"

# # -------------------------------------------------------------------------
# # Cleanup (청소)
# # -------------------------------------------------------------------------
# clean:
# 	@echo ">> Cleaning up build artifacts..."
# 	rm -rf $(BUILD_DIR) *.gba *.elf *.sav

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
LDFLAGS    := $(ARCH) -specs=gba.specs

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