##
## $Id$
##


include_bspdir = $(includedir)/bsp

C_FILES = irq.c irq_init.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_bsp_HEADERS = irq.h

S_FILES = irq_asm.S
OBJS += $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp \
    $(include_bsp_HEADERS:%=$(PROJECT_INCLUDE)/bsp/%)

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(PREINSTALL_FILES) $(ARCH) $(OBJS) $(PGM)

EXTRA_DIST = irq.c irq_asm.S irq_init.c

include $(top_srcdir)/../../../../../../automake/local.am
