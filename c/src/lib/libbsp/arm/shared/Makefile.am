##
## $Id$
##

# comm
include_HEADERS = comm/uart.h
EXTRA_DIST = comm/console.c comm/uart.c

# irq
EXTRA_DIST += irq/irq_asm.S irq/irq_init.c

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/uart.h: comm/uart.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/uart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/uart.h

include $(top_srcdir)/../../../automake/local.am
