##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/vme:@srcdir@/../../../shared/vmeUniverse:

H_FILES = ../pci/pci.h ../irq/irq.h ../vme/VME.h ../vme/VMEConfig.h

C_FILES = vmeUniverse.c vmeconfig.c


C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = ../../shared/vme/VMEConfig.h
include_bsp_HEADERS += ../../shared/vme/VME.h
include_bsp_HEADERS += ../../../shared/vmeUniverse/vmeUniverse.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/vmeUniverse.h: vmeUniverse.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/VME.h: VME.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/VMEConfig.h: VMEConfig.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp \
    $(addprefix $(PROJECT_INCLUDE)/bsp/,$(notdir $(include_bsp_HEADERS)))

all-local: $(PREINSTALL_FILES) $(ARCH) $(OBJS)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

include $(top_srcdir)/../../../../../../automake/local.am
