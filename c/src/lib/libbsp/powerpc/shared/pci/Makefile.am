##
## $Id$
##

C_FILES = pci.c detect_raven_bridge.c pcifinddevice.c

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = pci.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = pci.c detect_raven_bridge.c pcifinddevice.c

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/pci.h: pci.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/pci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pci.h

include $(top_srcdir)/../../../automake/local.am
