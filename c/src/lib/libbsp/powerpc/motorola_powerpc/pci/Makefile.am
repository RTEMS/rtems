##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/pci

C_FILES = pci.c detect_raven_bridge.c pcifinddevice.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_bspdir = $(includedir)/bsp
nodist_include_bsp_HEADERS = $(top_srcdir)/../shared/pci/pci.h

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

AM_CFLAGS += -O2

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(PREINSTALL_FILES) $(OBJS)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/pci.h: $(top_srcdir)/../shared/pci/pci.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/pci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pci.h

include $(top_srcdir)/../../../../automake/local.am
