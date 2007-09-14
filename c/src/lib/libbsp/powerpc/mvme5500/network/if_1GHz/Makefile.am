##
## $Id$
##

VPATH = @srcdir@:

INCLUDES = -I @srcdir@/../GT64260

C_FILES = if_wm.c pci_map.c
include_bspdir = $(includedir)/bsp
include_bsp_HEADERS =  if_wmreg.h pcireg.h

H_FILES =  if_wmreg.h pcireg.h

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

EXTRA_DIST =  if_wm.c pci_map.c

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

CFLAGS   += -msoft-float

#
# (OPTIONAL) Add local stuff here using +=
#

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $<

$(PROJECT_INCLUDE)/bsp/if_wmreg.h: if_wmreg.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/pcireg.h: pcireg.h 
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/if_wmreg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/pcireg.h

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

include $(top_srcdir)/../../../../../../automake/local.am
