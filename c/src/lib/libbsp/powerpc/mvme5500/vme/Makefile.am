##
## $Id$
## Kate Feng modified it for the MVME5500 board
##

VPATH = @srcdir@:@srcdir@/../../../shared/vmeUniverse:

INCLUDES = -I @srcdir@/../vme

H_FILES = VME.h VMEConfig.h

C_FILES = vmeUniverse.c vmeconfig.c


C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = VMEConfig.h
include_bsp_HEADERS += VME.h
include_bsp_HEADERS += ../../../shared/vmeUniverse/vmeUniverse.h

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/vmeUniverse.h: vmeUniverse.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/VME.h: VME.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/VMEConfig.h: VMEConfig.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VME.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VMEConfig.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/vmeUniverse.h

all-local: $(PREINSTALL_FILES) $(ARCH) $(OBJS)

#
# (OPTIONAL) Add local stuff here using +=
#
EXTRA_DIST = vmeconfig.c

include $(top_srcdir)/../../../../../../automake/local.am
