##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

C_FILES = i8259.c irq.c irq_init.c

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = irq.h

S_FILES = irq_asm.S

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp \
    $(include_bsp_HEADERS:%=$(PROJECT_INCLUDE)/bsp/%)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = i8259.c irq.c irq.h irq_asm.S irq_init.c

include $(top_srcdir)/../../../../../automake/local.am
