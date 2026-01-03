# -------------------------------------------------------------------------
#  Universal GBA Makefile (Final Fix)
#  설명: .SECONDARY 뒤를 비워서 모든 중간 파일 삭제를 강제로 막음
# -------------------------------------------------------------------------

# [0. Make Config]
MAKEFLAGS += --no-builtin-rules
.SUFFIXES:
# 중요: 뒤에 아무것도 안 적으면 "모든 타겟을 보존하라"는 뜻이 됩니다.
.SECONDARY:

# [1. Path Settings]
ifeq ($(OS),Windows_NT)
    DEVKITPRO ?= C:/devkitPro
else
    DEVKITPRO ?= /opt/devkitpro
endif

DEVKITARM ?= $(DEVKITPRO)/devkitARM
BIN_PATH = $(DEVKITARM)/bin
PREFIX   = arm-none-eabi-

# [2. Tool Definitions]
CC      = $(BIN_PATH)/$(PREFIX)gcc
OBJCOPY = $(BIN_PATH)/$(PREFIX)objcopy
FIX     = $(DEVKITPRO)/tools/bin/gbafix

# [3. Compilation Flags]
ARCH    = -mthumb -mthumb-interwork
SPECS   = -specs=$(DEVKITARM)/arm-none-eabi/lib/gba.specs
CFLAGS  = $(ARCH) $(SPECS) -O2 -Wall

# [4. Build Rules]

# 기본 타겟
all: main.gba

# (1) 바로가기 규칙
%: %.gba
	@echo "Build Complete: $@"

# (2) .elf -> .gba
%.gba: %.elf
	@echo "  [OBJCOPY] $@"
	$(OBJCOPY) -v -O binary $< $@
	@echo "  [GBAFIX]  $@"
	$(FIX) $@ -p -t$(basename $@)

# (3) .c -> .elf
%.elf: %.c
	@echo "  [COMPILE] $<"
	$(CC) $(CFLAGS) -o $@ $<

# [5. Clean Up]
clean:
	@echo "Cleaning up..."
	rm -f *.elf *.gba *.o *.sav