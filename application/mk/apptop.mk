
MKDIR:=$(PROJ_ROOT)/application/mk
include $(MKDIR)/cross-tools.mk

OBJDIR=$(PROJ_ROOT)/application/obj
LIBDIR=$(PROJ_ROOT)/application/lib

#Git version number generation
GIT_SERIAL:=$(shell git rev-list HEAD | wc -l)
GIT_HASH:=$(shell git rev-list --abbrev-commit HEAD | head -1)
GIT_DIRTY:=$(shell if git diff-files --quiet --ignore-submodules --;then echo "clean";else echo "dirty";fi)
ifeq (dirty, $(GIT_DIRTY))
GIT_HASH:=+$(GIT_HASH)
endif
CHANGESET:=$(GIT_SERIAL)_$(GIT_HASH)
BUILDDATE:=$(shell date +%y%m%d)
BUILDTIME:=$(shell date +%H%M%S)
# Get the branch name from the environment or command line, else compute it from local repo
#  Hudson/Jenkins must set this, due to absence of local repo
ifeq ($(origin BUILDBRANCH), undefined)
BUILDBRANCH:=$(shell git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\1/')
endif
export BUILDBRANCH
