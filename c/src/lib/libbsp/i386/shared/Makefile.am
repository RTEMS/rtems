##
## $Id$
##

include_HEADERS =
EXTRA_DIST =

# comm
include_HEADERS += comm/uart.h comm/tty_drv.h comm/i386_io.h

EXTRA_DIST += comm/i386-stub-glue.c comm/i386-stub.c comm/uart.c \
    comm/tty_drv.c comm/gdb_glue.c

EXTRA_DIST += comm/GDB.HOWTO

# irq
include_HEADERS += irq/irq.h irq/irq_asm.h

EXTRA_DIST += irq/idt.c irq/irq.c irq/irq_init.c
EXTRA_DIST += irq/irq_asm.S

# pci
include_HEADERS += pci/pcibios.h

EXTRA_DIST += pci/pcibios.c

# ---
all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/uart.h: comm/uart.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/uart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/uart.h

$(PROJECT_INCLUDE)/tty_drv.h: comm/tty_drv.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tty_drv.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tty_drv.h

$(PROJECT_INCLUDE)/i386_io.h: comm/i386_io.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/i386_io.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/i386_io.h

$(PROJECT_INCLUDE)/irq.h: irq/irq.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/irq.h

$(PROJECT_INCLUDE)/irq_asm.h: irq/irq_asm.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/irq_asm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/irq_asm.h

$(PROJECT_INCLUDE)/pcibios.h: pci/pcibios.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/pcibios.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/pcibios.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
