##
## $Id$
##

include_HEADERS = bspopts.h

include_HEADERS += ../../shared/include/nvram.h ../../shared/include/bsp.h \
    ../../../shared/include/coverhd.h

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = ../../shared/console/consoleIo.h ../../shared/console/uart.h \
    ../../shared/irq/irq.h ../../shared/motorola/motorola.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/nvram.h: ../../shared/include/nvram.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp.h: ../../shared/include/bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: ../../../shared/include/coverhd.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/consoleIo.h: ../../shared/console/consoleIo.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/uart.h: ../../shared/console/uart.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/irq.h: ../../shared/irq/irq.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/motorola.h: ../../shared/motorola/motorola.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_INCLUDE)
TMPINSTALL_FILES += $(PROJECT_INCLUDE)/bspopts.h
TMPINSTALL_FILES += $(PROJECT_INCLUDE)/bsp.h
TMPINSTALL_FILES += $(PROJECT_INCLUDE)/nvram.h
TMPINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

TMPINSTALL_FILES += $(PROJECT_INCLUDE)/bsp
TMPINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/motorola.h
TMPINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/consoleIo.h
TMPINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/uart.h 
TMPINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/force-preinstall.am
include $(top_srcdir)/../../../../../../automake/local.am
