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

$(PROJECT_INCLUDE)/bsp:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/bsp
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp

$(PROJECT_INCLUDE)/bsp/consoleIo.h: ../shared/console/consoleIo.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/consoleIo.h

$(PROJECT_INCLUDE)/bsp/uart.h: ../shared/console/uart.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/uart.h

$(PROJECT_INCLUDE)/bsp/irq.h: ../shared/irq/irq.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/irq.h

$(PROJECT_INCLUDE)/bsp/motorola.h: ../shared/motorola/motorola.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/motorola.h

$(PROJECT_INCLUDE)/nvram.h: ../shared/include/nvram.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/nvram.h

$(PROJECT_INCLUDE)/coverhd.h: ../../shared/include/coverhd.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
