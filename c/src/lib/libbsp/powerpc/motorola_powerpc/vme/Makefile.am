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

all-local: $(PREINSTALL_FILES) $(OBJS)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/bsp
	@: > $(PROJECT_INCLUDE)/bsp/$(dirstamp)

$(PROJECT_INCLUDE)/bsp/VMEConfig.h: ../../shared/vme/VMEConfig.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/VMEConfig.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VMEConfig.h

$(PROJECT_INCLUDE)/bsp/VME.h: ../../shared/vme/VME.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/VME.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VME.h

$(PROJECT_INCLUDE)/bsp/vmeUniverse.h: ../../../shared/vmeUniverse/vmeUniverse.h $(PROJECT_INCLUDE)/bsp/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bsp/vmeUniverse.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vmeUniverse.h

include $(top_srcdir)/../../../../automake/local.am
