##
## $Id$
##

C_FILES = i8259.c irq.c irq_init.c

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = irq.h

S_FILES = irq_asm.S

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = i8259.c irq.c irq_asm.S irq_init.c

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/irq.h: irq.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

include $(top_srcdir)/../../../automake/local.am
