##
## $Id$
##

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

include_bspdir = $(includedir)/bsp
nodist_include_bsp_HEADERS = $(top_srcdir)/../../powerpc/shared/pci/pci.h

power_shared_pci_C_FILES = $(top_srcdir)/../../powerpc/shared/pci/pci.c \
    $(top_srcdir)/../../powerpc/shared/pci/detect_raven_bridge.c \
    $(top_srcdir)/../../powerpc/shared/pci/pcifinddevice.c
OBJS = \
    $(power_shared_pci_C_FILES:$(top_srcdir)/../../powerpc/shared/pci/%.c=$(ARCH)/%.$(OBJEXT))

$(ARCH)/%.$(OBJEXT): $(top_srcdir)/../../powerpc/shared/pci/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<

noinst_DATA = $(OBJS)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/pci.h: $(top_srcdir)/../../powerpc/shared/pci/pci.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/pci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pci.h

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
