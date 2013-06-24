
PROJ_ROOT=..
RTOS_SOURCE_DIR=$(PROJ_ROOT)/FreeRTOS/Source

OPENOCDDIR=/opt/oocd
TOOLDIR=/opt/cross-arm-linaro
TOOLPREFIX=arm-none-eabi

LPCOPEN=$(PROJ_ROOT)/external/lpcopen/lpcopen
LPCCHIP=$(LPC_OPEN)/software/lpc_core/lpc_chip/chip_18xx_43xx
LPCIP=$(LPC_OPEN)/software/lpc_core/lpc_ip

PATH:=$(TOOLDIR)/bin:$(OPENOCDDIR)/bin:$(PATH)
OPENOCD=openocd
CC=$(TOOLPREFIX)-gcc
OBJCOPY=$(TOOLPREFIX)-objcopy
LDSCRIPT=standalone.ld

PROG = RTOSDemo

# should use --gc-sections but the debugger does not seem to be able to cope with the option.
LINKER_FLAGS=-nostartfiles -Xlinker -o$(AXF) -Xlinker -M -Xlinker -Map=$(PROG).map -Xlinker --no-gc-sections

# --eh-frame-hdr

DEBUG=-g
OPTIM=-O0
FREERTOS_INC= -I $(RTOS_SOURCE_DIR)/include -I $(RTOS_SOURCE_DIR)/portable/GCC/ARM_CM4F
LPC_INC= -I $(LPCCHIP) -I $(LPCIP)
INCLUDE=-I . $(FREERTOS_INC) $(LPC_INC)

CFLAGS = $(DEBUG) $(INCLUDE) \
	-D GCC_ARMCM3_LM3S102 -D inline=-mthumb -mcpu=cortex-m3 \
	 $(OPTIM) -T$(LDSCRIPT) \

SOURCE=	startup.c main.c \
	$(RTOS_SOURCE_DIR)/list.c \
	$(RTOS_SOURCE_DIR)/queue.c \
	$(RTOS_SOURCE_DIR)/tasks.c \
	$(RTOS_SOURCE_DIR)/portable/GCC/ARM_CM3/port.c \
	$(RTOS_SOURCE_DIR)/portable/MemMang/heap_2.c

LIBS= -L $(LUMINARY_DRIVER_DIR)/gcc -ldriver -L $(LUMINARY_GR_DIR)/gcc -lgr

OBJS = $(SOURCE:.c=.o)


all: $(PROG).elf

$(PROG).elf : $(OBJS) startup.o Makefile
	$(CC) $(CFLAGS) $(OBJS) startup.o $(LIBS) $(LINKER_FLAGS)
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


