##
## $Id$
##


C_FILES = vectors_init.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = vectors.h

S_FILES = vectors.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES) $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/bsp/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp \
    $(include_bsp_HEADERS:%=$(PROJECT_INCLUDE)/bsp/%)

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

include $(top_srcdir)/../../../../../../automake/local.am
