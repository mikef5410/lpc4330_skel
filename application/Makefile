
PROJ_ROOT=..

RTOS_SOURCE_DIR=$(PROJ_ROOT)/FreeRTOS/Source
CALC_OBJS = $(addsuffix .o, $(basename $(notdir $(1))))
OPENOCDDIR:=/opt/oocd
TOOLDIR:=/opt/cross-arm-linaro
PATH:=$(TOOLDIR)/bin:$(OPENOCDDIR)/bin:$(PATH)
OPENOCD=openocd
TOOLPREFIX=arm-none-eabi
CC=$(TOOLPREFIX)-gcc
OBJCOPY=$(TOOLPREFIX)-objcopy
LDSCRIPT=standalone.ld

LPCOPEN=$(PROJ_ROOT)/external/lpcopen/lpcopen
LPCCHIP=$(LPC_OPEN)/software/lpc_core/lpc_chip/chip_18xx_43xx
LPCCHIP_COMMON=$(LPC_OPEN)/software/lpc_core/lpc_chip/chip_common
LPCIP=$(LPC_OPEN)/software/lpc_core/lpc_ip

## LPC4330 Specific Definitions
ARCH = armv7e-m
CPU = cortex-m4
MX = M4
CHIP ?= CHIP_LPC43XX
FLASHK ?= 512
RAMK ?= 192


## LPCOPEN Definifions
CORE_CHIP_SRCS = adc_18xx_43xx.c can_18xx_43xx.c clock_18xx_43xx.c dac_18xx_43xx.c enet_18xx_43xx.c gpdma_18xx_43xx.c
CORE_CHIP_SRCS += i2c_18xx_43xx.c i2s_18xx_43xx.c iocon_18xx_43xx.c
CORE_CHIP_SRCS += rtc_18xx_43xx.c ssp_18xx_43xx.c sysctl_18xx_43xx.c timer_18xx_43xx.c uart_18xx_43xx.c wwdt_18xx_43xx.c
CORE_IP_SRCS = adc_001.c atimer_001.c ccan_001.c dac_001.c emc_001.c enet_001.c gima_001.c gpdma_001.c gpiogrpint_001.c gpiopinint_001.c i2c_001.c i2s_001.c lcd_001.c ritimer_001.c rtc_001.c sct_001.c sdmmc_001.c ssp_001.c timer_001.c usart_001.c wwdt_001.c eeprom_002.c
LPCOPEN_OBJS = $(call CALC_OBJS,$(CORE_IP_SRCS) $(CORE_CHIP_SRCS))

## FreeRTOS Definitions
FREERTOS_PORT=GCC/ARM_CM4F
FREERTOS_INC= -I $(RTOS_SOURCE_DIR)/include -I $(RTOS_SOURCE_DIR)/portable/$(FREERTOS_PORT)
FREERTOS_SRCS = list.c queue.c tasks.c timers.c coroutine.c port.c heap_2.c
FREERTOS_OBJS = $(call CALC_OBJS,$(FREERTOS_SRCS))

PROG = RTOSDemo

# should use --gc-sections but the debugger does not seem to be able to cope with the option.
LINKER_FLAGS=-nostartfiles -Xlinker -o$(AXF) -Xlinker -M -Xlinker -Map=$(PROG).map -Xlinker --no-gc-sections

# --eh-frame-hdr

DEBUG=-g
OPTIM=-O0
LPC_INC= -I $(LPCCHIP) -I $(LPCIP)
INCLUDE=-I . $(FREERTOS_INC) $(LPC_INC)

CFLAGS = $(DEBUG) $(INCLUDE) \
	 -mcpu=$(CPU) -march=$(ARCH) -mthumb \
	 $(OPTIM) -T$(LDSCRIPT) \

APP_SRCS =startup.c main.c
APP_OBJS = $(call CALC_OBJS,$(APP_SRCS))

LIBS= libFreeRTOS.a liblpcopen.a

VPATH=.:$(RTOS_SOURCE_DIR):$(LPCCHIP_COMMON):$(LPCCHIP):$(LPCIP)
all: $(PROG).elf

libFreeRTOS.a: $(FREERTOS_OBJS)
	$(AR) cr $@ $?
	$(RANLIB) $@

liblpcopen.a: $(LPCOPEN_OBJS)
	$(AR) cr $@ $?
	$(RANLIB) $@

$(PROG).elf : $(OBJS) startup.o Makefile
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) $(LINKER_FLAGS)
	$(TOOLPREFIX-size) $(PROG).axf

$(OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@




download.cmd: Makefile
	/bin/echo -e "FIXME!!!! init\nhalt\nsleep 200\nwait_halt\nflash write_image erase $(AXF)\nsleep 200\nreset run\nshutdown\n" > download.cmd

download: $(PROG).elf download.cmd
	$(OPENOCD)  -f board/ek-lm3s3748.cfg -f download.cmd


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


