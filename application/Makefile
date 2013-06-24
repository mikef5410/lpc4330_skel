
PROJ_ROOT=..
RTOS_SOURCE_DIR=$(PROJ_ROOT)/FreeRTOS/Source

OPENOCDDIR=/opt/oocd
TOOLDIR=/opt/cross-arm-linaro
TOOLPREFIX=arm-none-eabi

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
INCLUDE=-I . $(FREERTOS_INC) -I $(LUMINARY_DIR) 

CFLAGS = $(DEBUG) $(INCLUDE) \
	-D GCC_ARMCM3_LM3S102 -D inline=-mthumb -mcpu=cortex-m3 \
	 $(OPTIM) -T$(LDSCRIPT) \
	-D PACK_STRUCT_END=__attribute\(\(packed\)\) \
	-D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\) \
	-D sprintf=usprintf -D snprintf=usnprintf -D printf=uipprintf \
	-ffunction-sections -fdata-sections

SOURCE=	main.c \
	formike128x128x16.c \
	timertest.c \
	$(LUMINARY_DIR)/utils/ustdlib.c \
	$(LUMINARY_DIR)/utils/uartstdio.c \
	$(LUMINARY_DIR)/utils/cmdline.c \
	$(RTOS_SOURCE_DIR)/list.c \
	$(RTOS_SOURCE_DIR)/queue.c \
	$(RTOS_SOURCE_DIR)/tasks.c \
	$(RTOS_SOURCE_DIR)/portable/GCC/ARM_CM3/port.c \
	$(RTOS_SOURCE_DIR)/portable/MemMang/heap_2.c

LIBS= -L $(LUMINARY_DRIVER_DIR)/gcc -ldriver -L $(LUMINARY_GR_DIR)/gcc -lgr

OBJS = $(SOURCE:.c=.o)


all: $(PROG).bin
	arm-bare_newlib_cortex_m3_nommu-eabi-size $(PROG).axf

$(PROG).bin : $(PROG).axf
	$(OBJCOPY) $(PROG).axf -O binary $(PROG).bin

$(PROG).axf : $(OBJS) startup.o Makefile
	$(CC) $(CFLAGS) $(OBJS) startup.o $(LIBS) $(LINKER_FLAGS)

$(OBJS) : %.o : %.c Makefile FreeRTOSConfig.h
	$(CC) -c $(CFLAGS) $< -o $@

startup.o : startup.c Makefile
	$(CC) -c $(CFLAGS) -O1 startup.c -o startup.o



download.cmd: Makefile
	/bin/echo -e "init\nhalt\nsleep 200\nwait_halt\nflash write_image erase $(AXF)\nsleep 200\nreset run\nshutdown\n" > download.cmd

download: $(PROG).axf download.cmd
	$(OPENOCD)  -f board/ek-lm3s3748.cfg -f download.cmd


############
# Source indexing with cscope, idutils and etags 
############
cscope id etags tags: FILETYPES:=-iname "*.cpp" -o -iname "*.hpp" -o -name "*.S" -o -iname "*.c" -o -iname "*.h" -o -name "*.s" -o -name "*.cc"
cscope id etags tags: INDEX_FILES:=$(shell (find  . $(RTOS_SOURCE_DIR) $(LUMINARY_DIR) $(LUMINARY_DRIVER_DIR) $(LUMINARY_GR_DIR) $(FILETYPES)))

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


