##
## Makefile.am,v 1.6 2002/05/14 17:10:16 joel Exp
##

VPATH = @srcdir@:

INCLUDES = -I @srcdir@/../pci
include_bspdir = $(includedir)/bsp
include_bsp_HEADERS =  gtpcireg.h pci.h
H_FILES = gtpcireg.h pci.h

C_FILES = pci.c pci_interface.c detect_host_bridge.c pcifinddevice.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))
EXTRA_DIST = pci.c pci_interface.c detect_host_bridge.c pcifinddevice.c

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/gtpcireg.h: gtpcireg.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/pci.h: pci.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp $(PROJECT_INCLUDE)/bsp/pci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/gtpcireg.h

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

include $(top_srcdir)/../../../../../../automake/local.am
