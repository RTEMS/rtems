##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../shared/pci

C_FILES = pci.c detect_raven_bridge.c pcifinddevice.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

H_FILES = ../../shared/pci/pci.h

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS += -O2

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $<

$(PROJECT_INCLUDE)/bsp/pci.h: ../../shared/pci/pci.h
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile
PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp $(PROJECT_INCLUDE)/bsp/pci.h

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

include $(top_srcdir)/../../../../../../automake/local.am
