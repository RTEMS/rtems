##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . clock console pci residual openpic irq vectors start startup \
    bootloader motorola @exceptions@ vme wrapup

include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

nodist_include_HEADERS += ../shared/include/nvram.h
nodist_include_HEADERS += ../../shared/include/coverhd.h
nodist_include_bsp_HEADERS = ../shared/console/consoleIo.h
nodist_include_bsp_HEADERS += ../shared/console/uart.h
nodist_include_bsp_HEADERS += ../shared/irq/irq.h
nodist_include_bsp_HEADERS += ../shared/motorola/motorola.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = BOOTING README.MVME2300 README.OTHERBOARDS README.dec21140
EXTRA_DIST += times.mcp750 times.mvme2307

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/nvram.h: ../shared/include/nvram.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/nvram.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nvram.h

$(PROJECT_INCLUDE)/coverhd.h: ../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_INCLUDE)/bsp/consoleIo.h: ../shared/console/consoleIo.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/consoleIo.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/consoleIo.h

$(PROJECT_INCLUDE)/bsp/uart.h: ../shared/console/uart.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/uart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/uart.h

$(PROJECT_INCLUDE)/bsp/irq.h: ../shared/irq/irq.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/irq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

$(PROJECT_INCLUDE)/bsp/motorola.h: ../shared/motorola/motorola.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/motorola.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/motorola.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
