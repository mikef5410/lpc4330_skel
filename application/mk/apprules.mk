#
#
#

CALC_OBJS = $(addsuffix .o, $(basename $(notdir $(1))))
CALC_DEPS = $(addsuffix .d, $(basename $(notdir $(1))))

APP_OBJS := $(addprefix $(OBJDIR), $(call CALC_OBJS,$(APP_SRCS)))
APP_DEPS := $(addprefix $(OBJDIR), $(call CALC_DEPS,$(APP_SRCS)))

ifdef MAKE_LISTING
CFLAGS += -Wa,-ahlms=$@.lst --save-temps
endif



# control code
APP_DEFINES += $(APP_EXTRA_DEFINES)
APP_INCLUDES += -I. $(APP_EXTRA_INCLUDES)
APP_CFLAGS +=  $(APP_OTHER_CC_FLAGS)

APP_LDFLAGS += -Wl,-Map,app.map $(APP_OTHER_CC_FLAGS)
APP_LIBPATH += $(APP_LIBPATH)
APP_LIBS += $(APP_LFLAGS) 

ifneq ($(APP_EXTRA_DEFINES),)
$(warning WARNING: APP_EXTRA_DEFINES is $(APP_EXTRA_DEFINES))
endif

define APP_COMPILE
#
#---------------------------------------------------------------
# User space compile of $@
#
-$(APP_CC) -MM -MG $(DEFINES) $(APP_DEFINES) $(APP_INCLUDES) $(INCLUDES) $(APP_CFLAGS)  $< -o $(@:%.o=%.X)
sed -e 's,\($(basename $(<F))\)\.o[ :]*,$(OBJDIR)\1.o $(OBJDIR)\1.d: $(MAKEFILE_DEP) ,g' < $(@:%.o=%.X) > $(@:%.o=%.d);$(RM) -f $(@:%.o=%.X);
#
$(APP_CC) $(CFLAGS) $(DEFINES) $(APP_DEFINES) $(APP_INCLUDES) $(INCLUDES) $(APP_CFLAGS) -c  $< -o $@ $(LOG)
endef

define APP-COMPILE-NODEPS
$(APP_CC) $(CFLAGS) $(DEFINES) $(APP_DEFINES) $(APP_INCLUDES) $(INCLUDES) $(APP_CFLAGS) -c  $< -o $@ $(LOG)
endef


define APP-PP
#
#---------------------------------------------------------------
# User space preprocess of $@
#
$(APP_CC) -E $(CFLAGS) -Wa,-ahls=$@.lst $(DEFINES) $(APP_DEFINES) $(APP_INCLUDES) $(INCLUDES) $(APP_CFLAGS) -c -MD $< -o $@ $(LOG)
endef

define APP-LINK
#
#---------------------------------------------------------------
# User space link of $@
#
$(APP_CCLINK) $(APP_LDFLAGS) -o  $@ $(APP_EARLY_LIBS) $^ $(APP_LIBPATH) $(APP_EXTRA_LIBPATH) $(APP_LIBS) $(APP_EXTRA_LIBS)  $(LOG)
endef


define LIB-LINK
#
#---------------------------------------------------------------
# Library link link of $@
#
$(AR) crsv $@ $^
endef

define LIB-ADD
$(AR) rsv $@ $^
endef

define OBJDUMP-MOD-LST
#--------------
# Generated objdump module listing file
$(OBJDUMP) -S -D -d $^ > $@
endef

#
#----------------------------------------------------------------
#Pattern Rules
#
%.tab.c: %.y
        $(YACC) $(YFLAGS) -b$(basename $<) $<

%.lex.c: %.l
        $(LEX) $(LFLAGS) -o$@ $<


$(OBJDIR)%.tab.o: %.tab.c
        $(APP_COMPILE)

$(OBJDIR)%.lex.o: %.lex.c
        $(APP_COMPILE)

$(OBJDIR)%.o: %.c
        $(APP_COMPILE)

$(OBJDIR)%.o: %.cpp
        $(APP_COMPILE)

$(OBJDIR)%.o: %.cc
        $(APP_COMPILE)

$(OBJDIR)%.o: %.s
        $(APP_COMPILE)

$(OBJDIR)%.o: %.S
        $(APP_COMPILE)

%.i: %.c
        $(APP-PP)

%.i: %.cpp
        $(APP-PP)

%.i: %.cc
        $(APP-PP)



ifdef LIB_TARGET
BUILD_TARGS+= $(LIB_TARGET)
$(LIB_TARGET) : $(LIB_OBJS) $(LIB_EXTRA_OBJS)
        mkdir -p $(@D)
        $(LIB-LINK)
endif

ifdef APP_TARGET
BUILD_TARGS+= $(APP_TARGET)
$(APP_TARGET) :  $(APP_OBJS) $(APP_EXTRA_OBJS) | recurse
        mkdir -p $(@D)
        $(APP-LINK)
        $(SIZE) $(APP_TARGET)
ifdef MAKE_LISTING
BUILD_TARGS+= $(APP_TARGET).lst
$(APP_TARGET).lst : $(APP_TARGET)
        $(OBJDUMP-MOD-LST)
endif
endif

#HACK ... for lib-only subdir builds, DONT try to build the main app targets
ifndef LIB_ONLY
all_targs: $(OBJDIR) $(LIBDIR) $(BUILD_TARGS) $(EXTRA_TARGETS)
else
all_targs: $(OBJDIR) $(LIBDIR)
endif

$(OBJDIR):
        mkdir -p $(OBJDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

-include $(DEPS)

.PHONY : clean distclean timestamp download recurse release

#Update our local copy of globals.h with current changeset and date
timestamp: GLOBALS:=../../../include/globals.h
timestamp: TFILE:=globals.h
timestamp:
        @echo stamp globals.h ...
        @head --lines=-1 $(GLOBALS) | grep -v FW_BUILD >$(TFILE)
        @echo "#define FW_BUILD_DATE \"$(BUILDDATE)\"" >>$(TFILE)
        @echo "#define FW_BUILD_CLNUM \"$(CHANGESET)\"" >>$(TFILE)
        @echo "#endif /* GLOBALS_H */" >>$(TFILE)
        @mv globals.h $(GLOBALS)

download: $(PROM)promPlus.mcs
        @time echo -e "setMode -bscan\nsetCable -p auto\nidentify\nattachflash -position 1 -spi \"M25P64\"\nassignfiletoattachedflash -position 1 -file \"prom/promPlus.mcs\"\nprogram -p 1 -spionly -e -loadfpga\nquit" | $(IMPACT)


VFILE:=version.h
$(VFILE): USER:=$(shell id -u -n)
$(VFILE): $(APP_SRCS) $(LIB_SRCS)
        @echo make version.h
        @rm -f version.h
        @echo "#ifndef _VERSION_INCLUDED" >> version.h
        @echo "#define _VERSION_INCLUDED" >> version.h
        @echo "#ifdef GLOBAL_VERSION" >> version.h
        @echo "#define VERSIONGLOBAL" >> version.h
        @echo "#define VERSIONPRESET(A) = (A)" >> version.h
        @echo "#else" >> version.h
        @echo "#define VERSIONPRESET(A)" >> version.h
        @echo "#ifdef __cplusplus" >> version.h
        @echo "#define VERSIONGLOBAL extern \"C\"" >> version.h
        @echo "#else" >> version.h
        @echo "#define VERSIONGLOBAL extern" >> version.h
        @echo "#endif  /*__cplusplus*/" >> version.h
        @echo "#endif                          /*GLOBAL_VERSION */" >> version.h
        @echo "#ifdef RELEASE" >> version.h
        @echo "#define BUILDINFO \"built at $(BUILDDATE) $(BUILDTIME) changeset:$(CHANGESET) ($(BUILDBRANCH)) by $(USER) on $(HOSTNAME)\"" >> version.h
        @echo "#define RELEASE_STATE \"RELEASE\"" >> version.h
        @echo "#else" >> version.h
        @echo "#define BUILDINFO \"DEBUG, built at $(BUILDDATE) $(BUILDTIME) changeset:$(CHANGESET) ($(BUILDBRANCH)) by $(USER) on $(HOSTNAME)\"" >> version.h
        @echo "#define RELEASE_STATE \"DEBUG\"" >> version.h
        @echo "#endif                          /*RELEASE_STATE */" >> version.h
        @echo "#define BUILD_SHA1 \"$(CHANGESET)\"" >> version.h
        @echo "#define BUILDBRANCH \"$(BUILDBRANCH)\"" >> version.h
        @echo "VERSIONGLOBAL char buildInfo[] VERSIONPRESET(BUILDINFO);" >> version.h
        @echo "VERSIONGLOBAL char build_sha1[] VERSIONPRESET(BUILD_SHA1);" >> version.h
        @echo "VERSIONGLOBAL char build_branch[] VERSIONPRESET(BUILDBRANCH);" >> version.h
        @echo "VERSIONGLOBAL char build_rls_state [] VERSIONPRESET(RELEASE_STATE);" >> version.h
        @echo "#endif                          //_VERSION_INCLUDED" >> version.h


version: $(VFILE)


recurse:
        @list='$(SUBDIRS)'; for subdir in $$list; do \
                echo Making in $$subdir; \
                if test "$$subdir" = "."; then \
                        prevdir="."; \
                else \
                        prevdir=".."; \
                fi; \
                (cd $$subdir; $(MAKE) all); \
                if [ $$? -ne 0 ] ; then \
                        exit 1; \
                fi; \
        done;


appclean:
        $(RM) -f $(APP_OBJS) $(APP_DEPS) $(VFILE)
        $(RM) -f $(APP_TARGET)

libclean:
        $(RM) -f $(LIB_OBJS) $(LIB_DEPS)
        $(RM) -f $(LIB_TARGET)
        @list='$(SUBDIRS)'; for subdir in $$list; do \
                echo Making in $$subdir; \
                if test "$$subdir" = "."; then \
                        prevdir="."; \
                else \
                        prevdir=".."; \
                fi; \
                (cd $$subdir; $(MAKE) libclean); \
                if [ $$? -ne 0 ] ; then \
                        exit 1; \
                fi; \
        done;
        #$(RM) -rf $(LIBLOC)/*

clean: bootclean appclean libclean
        @list='$(SUBDIRS)'; for subdir in $$list; do \
                if test "$$subdir" != "." ; then \
                        echo ---------------------; \
                        echo Making $@ in $$subdir; \
                        $(MAKE) -C $$subdir $@; \
                fi; \
        done;
        $(RM) -f $(OBJS) $(EXTRA_TARGETS) *.o *~ \#*
        $(RM) -f *.core *.exe core *.a $(FAKETARGET) *.lst $(DEPS)
        $(RM) -f *.bb *.bbg *.da
        $(RM) -f *.i *.s
        $(RM) -rf static $(VFILE)
        $(RM) -f $(PROM)/*
ifdef SRCS
        $(RM) -f $(SRCS:.c=.P)
endif
ifdef EXTRA_CLEAN_TARGETS
        $(RM) -f $(EXTRA_CLEAN_TARGETS)
endif
        $(RM) -f *~ \#*
ifdef LIB_TARGET
        $(RM) -f $(LIB_TARGET)
endif
ifdef OBJDIR
        -$(RM) -rf $(OBJDIR)*
        -$(RM) -f $(shell basename $(OBJDIR))
endif
ifdef BOBJDIR
        -$(RM) -rf $(BOBJDIR)*
        -$(RM) -f $(shell basename $(BOBJDIR))
endif

subdirs: $(MAKEFILE_DEP)
        @echo $(SUBDIRS) > subdirs

help:
        cat build/makehelp


ifndef LIB_ONLY

include $(MKDIR)/tags.mk
