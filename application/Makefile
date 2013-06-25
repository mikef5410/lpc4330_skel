
PROJ_ROOT=..

CALC_OBJS = $(addsuffix .o, $(basename $(notdir $(1))))

include mk/cross-tools.mk

LDSCRIPT := standalone.ld



## LPC4330 Specific Definitions
ARCH = armv7e-m
CPU = cortex-m4
MX = M4
CHIP ?= CHIP_LPC43XX
FLASHK ?= 512
RAMK ?= 192

## LPCOPEN Definifions
LPCOPEN := $(PROJ_ROOT)/external/lpcopen/lpcopen
LPCCHIP := $(LPCOPEN)/software/lpc_core/lpc_chip/chip_18xx_43xx
LPCCHIP_COMMON := $(LPCOPEN)/software/lpc_core/lpc_chip/chip_common
LPCIP := $(LPCOPEN)/software/lpc_core/lpc_ip
CMSIS_INC := $(LPCOPEN)/software/CMSIS/CMSIS/Include
CORE_CHIP_SRCS = adc_18xx_43xx.c dac_18xx_43xx.c gpdma_18xx_43xx.c otp_18xx_43xx.c sct_18xx_43xx.c \
                 timer_18xx_43xx.c aes_18xx_43xx.c eeprom_18xx_43xx.c gpio_18xx_43xx.c pmc_18xx_43xx.c \
                 scu_18xx_43xx.c uart_18xx_43xx.c atimer_18xx_43xx.c emc_18xx_43xx.c i2c_18xx_43xx.c \
                 rgu_18xx_43xx.c sdif_18xx_43xx.c wwdt_18xx_43xx.c ccan_18xx_43xx.c enet_18xx_43xx.c \
                 i2s_18xx_43xx.c ritimer_18xx_43xx.c sdmmc_18xx_43xx.c clock_18xx_43xx.c evrt_18xx_43xx.c \
                 lcd_18xx_43xx.c rtc_18xx_43xx.c ssp_18xx_43xx.c 
CORE_IP_SRCS = adc_001.c atimer_001.c ccan_001.c dac_001.c emc_001.c enet_001.c gpdma_001.c \
               gpiogrpint_001.c gpiopinint_001.c i2c_001.c i2s_001.c lcd_001.c ritimer_001.c \
               rtc_001.c sct_001.c sdmmc_001.c ssp_001.c timer_001.c usart_001.c wwdt_001.c \
               eeprom_002.c
LPCOPEN_OBJS := $(call CALC_OBJS,$(CORE_IP_SRCS) $(CORE_CHIP_SRCS))
LPC_INC= -I $(LPCCHIP) -I $(LPCIP) -I $(LPCCHIP_COMMON) -I $(CMSIS_INC)

## FreeRTOS Definitions
RTOS_SOURCE_DIR := $(PROJ_ROOT)/FreeRTOS/Source
FREERTOS_PORT := GCC/ARM_CM4F
FREERTOS_INC := -I $(RTOS_SOURCE_DIR)/include -I $(RTOS_SOURCE_DIR)/portable/MemMang -I $(RTOS_SOURCE_DIR)/portable/$(FREERTOS_PORT)
FREERTOS_SRCS := list.c queue.c tasks.c timers.c croutine.c port.c heap_2.c
FREERTOS_OBJS := $(call CALC_OBJS,$(FREERTOS_SRCS))

# Switch FPU on/off based on which FreeRTOS port we use
#
ifeq ($(FREERTOS_PORT),GCC/ARM_CM4F)
	USE_FPU = yes
else
	USE_FPU = no
endif

ifeq ($(USE_FPU),yes)
#  -mthumb -mcpu=cortex-m4 -mfloat-abi=hard 
#  USE_OPT += -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -fsingle-precision-constant
  CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fsingle-precision-constant
  CPPFLAGS += -DCORTEX_USE_FPU=TRUE -D__TARGET_FPU_VFP=TRUE
else
  CPPFLAGS += -DCORTEX_USE_FPU=FALSE
endif

PROG = RTOSDemo

# should use --gc-sections but the debugger does not seem to be able to cope with the option.
LINKER_FLAGS=-nostartfiles -Xlinker -o$(PROG).elf -Xlinker -M -Xlinker -Map=$(PROG).map -Xlinker --no-gc-sections

# --eh-frame-hdr

DEBUG=-g
OPTIM=-O0
INCLUDE=-I . $(FREERTOS_INC) $(LPC_INC) -I board/board_common -I board

CFLAGS += $(DEBUG) $(INCLUDE) \
	 -D CORE_M4 -mcpu=$(CPU) -mthumb -march=$(ARCH) \
	 $(OPTIM) -T$(LDSCRIPT) \

APP_SRCS =startup.c main.c sys_init.c tek_board_init.c
APP_OBJS = $(call CALC_OBJS,$(APP_SRCS))

LIBS= libFreeRTOS.a liblpcopen.a

VPATH=.:$(RTOS_SOURCE_DIR):$(RTOS_SOURCE_DIR)/portable/MemMang:$(RTOS_SOURCE_DIR)/portable/$(FREERTOS_PORT):$(LPCCHIP_COMMON):$(LPCCHIP):$(LPCIP):board:board/board_common
all: $(PROG).elf

libFreeRTOS.a: libFreeRTOS.a($(FREERTOS_OBJS))
	$(RANLIB) $@


liblpcopen.a: liblpcopen.a($(LPCOPEN_OBJS))
	$(RANLIB) $@

$(PROG).elf : libFreeRTOS.a($(FREERTOS_OBJS)) liblpcopen.a($(LPCOPEN_OBJS)) $(APP_OBJS) Makefile
	$(CC) $(CFLAGS) $(APP_OBJS) $(LIBS) $(LINKER_FLAGS)
	$(SIZE) $(PROG).elf



download.cmd: Makefile
	/bin/echo -e "FIXME!!!! init\nhalt\nsleep 200\nwait_halt\nflash write_image erase $(AXF)\nsleep 200\nreset run\nshutdown\n" > download.cmd

download: $(PROG).elf download.cmd
	$(OPENOCD)  -f board/ek-lm3s3748.cfg -f download.cmd

clean:
	$(RM) -f *.o *.a

############
# Source indexing with cscope, idutils and etags 
############
cscope id etags tags: FILETYPES:=-iname "*.cpp" -o -iname "*.hpp" -o -name "*.S" -o -iname "*.c" -o -iname "*.h" -o -name "*.s" -o -name "*.cc"
cscope id etags tags: INDEX_FILES:=$(shell (find  . $(RTOS_SOURCE_DIR) $(LPCCHIP) $(LPCIP) $(FILETYPES)))

cscope:
	@echo cscope ...
	@cscope -f cscope.out -q -b -k $(INDEX_FILES)

id:
	@echo idutils ...
	@rm -f ID
	@mkid -s --output=ID $(INDEX_FILES)
	export IDPATH=`pwd`/ID;

etags:
	@echo etags ...
	@rm -f TAGS
	@echo $(INDEX_FILES) | xargs etags --output=TAGS --append

tags: cscope id etags

cleantags:
	rm -f cscope.out* ID TAGS


clean : cleantags
	touch Makefile
	rm $(OBJS) $(PROG).bin $(PROG).axf $(PROG).map


