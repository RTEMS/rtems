##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../shared/irq

PGM = $(ARCH)/irq.rel

C_FILES = irq.c irq_init.c bsp_irq_init.c
S_FILES = irq_asm.S bsp_irq_asm.S
include_HEADERS = irq.h

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

irq_rel_OBJECTS = $(C_O_FILES) $(S_O_FILES)

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(irq_rel_OBJECTS)
	$(make-rel)

all: $(PREINSTALL_FILES) $(ARCH) $(irq_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = irq.c bsp_irq_init.c bsp_irq_asm.S

include $(top_srcdir)/../../../../../../automake/local.am
