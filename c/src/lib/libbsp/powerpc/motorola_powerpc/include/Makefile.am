##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_HEADERS = bspopts.h

H_FILES = ../../shared/include/nvram.h ../../shared/include/bsp.h \
    ../../../shared/include/coverhd.h

BSP_H_FILES = ../../shared/console/consoleIo.h ../../shared/console/uart.h \
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

TMPINSTALL_FILES = $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp \
    $(PROJECT_INCLUDE)/nvram.h $(PROJECT_INCLUDE)/bsp.h \
    $(PROJECT_INCLUDE)/coverhd.h $(PROJECT_INCLUDE)/bsp/consoleIo.h \
    $(PROJECT_INCLUDE)/bsp/uart.h $(PROJECT_INCLUDE)/bsp/irq.h \
    $(PROJECT_INCLUDE)/bsp/motorola.h \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/force-preinstall.am
include $(top_srcdir)/../../../../../../automake/local.am
