#
# Make recipes for various tags programs ...
#

# $Id$
# $Author$

# Collect tags for files NOT built (viz, have no .obj), that are in the scs12500 directory
#  This is useful during development of the UML model-to-c-code interface
cscope id etags: APP:=$(shell (find  $(EDK)../../scs12500_s3a -maxdepth 1 -type f $(FILETYPES))) 

#Make these target specific vars so we don't have to wait 
# for multiple finds when we're not making tags
# remove UI build cruft and editor auto-save and/or temp files
cruft:           CRUFT_UI:=-name "*.*~" -o -name "*.i" -o -name "*.inc" -o -name "*.lst" -o -name "*.map" -o -name "*.obj" -o -name "*.sym"
cruft:           CRUFT_EMACS:=-name "*.*~"
cruft:           REMOVE:=$(shell (find  $(EDK)../../../uiboard/UI_AVR $(CRUFT_UI))) 
cruft:           REMOVE2:=$(shell (find  $(EDK)../../../instrument $(CRUFT_EMACS))) 
cscope id etags cscope2 etags2: FILETYPES:=-iname "*.cpp" -o -iname "*.hpp" -o -name "*.S" -o -iname "*.c" -o -iname "*.h" -o -name "*.s"
cscope id etags cscope2 etags2: UI_TYPES:=-iname "scs*.[ch]" -o -iname "ssb*.[ch]" -o -iname "*_table.h" -o -iname "ui*.[ch]" -o -iname "[fxo]*.[ch]" -o -iname "dis*.[ch]" -o -iname "ques.[ch]"
cscope id etags cscope2 etags2: GENERATED:=$(shell (find $(EDK)microblaze_0/include $(EDK)microblaze_0/libsrc $(FILETYPES))) 
cscope id etags cscope2 etags2: COMMON:=$(shell (find  $(EDK)../../common $(FILETYPES))) 
cscope id etags cscope2 etags2: UML01:=$(shell (find  $(UML_MODELDIR) $(FILETYPES))) 
cscope id etags cscope2 etags2: BSP:=$(shell (find  $(EDK)../repository/Micrium/bsp/uCOS-II_v2_86_a/src/microblaze $(FILETYPES)))
cscope id etags cscope2 etags2: UCOS:=$(shell (find  $(EDK)../../uCOS-II $(EDK)../../uC-FS/Inc $(EDK)../../uC-FS/FS $(FILETYPES))) 
cscope id etags cscope2 etags2: BOOT:=$(shell (find  $(EDK)../drivers/Boot/src $(FILETYPES))) 
cscope id etags cscope2 etags2: UI:=$(shell (find  $(EDK)../../../uiboard/UI_AVR $(UI_TYPES))) 
cscope id etags cscope2 etags2: INDEX_DEPS:=$(sort $(filter %.h,$(strip $(shell cat $(OBJDIR)*.d | sed -e s/://g))))
cscope id etags cscope2 etags2: INDEX_CFILES:=$(sort $(filter %.c,$(strip $(shell cat $(OBJDIR)*.d | sed -e s/://g))))


tags: cscope id etags
 
stags:  cscope2 etags2

cruft:
        @(rm -f $(REMOVE))
        @(rm -f $(REMOVE2))

cscope: cruft
        @echo cscope ...
        @cscope -f cscope.out -q -b -k $(BT_INCLUDES) $(CTRL_INCLUDES) $(INCLUDES) $(INDEX_CFILES) $(GENERATED) $(BSP) $(UCOS) $(UI) $(COMMON) $(UML01) $(SCS) $(BOOT)

cscope2:cruft
        @echo limited-SCS cscope, to file s_cscope ...
        @rm -f s_cscope.out
        @cscope -f s_cscope.out -q -b -k $(UI) $(COMMON) $(UML01) $(SCS)
        @mv -f cscope.out cscope.out.bak
        @cp -f s_cscope.out cscope.out

id:     cruft
        @echo idutils ...
        @rm -f ID
        @mkid -s --output=ID $(INDEX_CFILES) $(GENERATED) $(BSP) $(UCOS) $(INDEX_DEPS)
        export IDPATH=`pwd`/ID;

etags:  cruft
        @echo etags ...
        @rm -f TAGS
        @echo $(INDEX_CFILES) $(GENERATED) $(BSP) $(UCOS) $(INDEX_DEPS) $(UI) $(COMMON) $(UML01) $(SCS) $(BOOT) | xargs etags --output=TAGS --append

etags2: cruft
        @echo limited-SCS etags, to file S_TAGS ...
        @rm -f S_TAGS
        @echo $(UI) $(COMMON) $(UML01) $(SCS) $(BOOT) | xargs etags --output=S_TAGS --append

cleantags:
        rm -f cscope.out* ID TAGS s_cscope.out S_TAGS
