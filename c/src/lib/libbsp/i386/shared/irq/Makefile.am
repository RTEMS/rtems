##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_HEADERS = irq.h irq_asm.h

C_FILES = idt.c irq.c irq_init.c
S_FILES = irq_asm.S

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = idt.c irq.c irq_asm.S irq_init.c

include $(top_srcdir)/../../../../../automake/local.am
