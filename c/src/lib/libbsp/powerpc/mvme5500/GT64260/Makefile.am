##
## $Id: Makefile.am, S. Kate Feng /12/03
##

VPATH = @srcdir@:

INCLUDES = -I @srcdir@/../GT64260

C_FILES = GT64260TWSI.c MVME5500I2C.c

include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = bspMvme5500.h gtreg.h GT64260TWSI.h VPD.h

H_FILES = bspMvme5500.h gtreg.h GT64260TWSI.h VPD.h

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

EXTRA_DIST = GT64260TWSI.c MVME5500I2C.c

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $<

$(PROJECT_INCLUDE)/bsp/bspMvme5500.h: bspMvme5500.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/gtreg.h: gtreg.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/GT64260TWSI.h: GT64260TWSI.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/VPD.h: VPD.h
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile
PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp $(PROJECT_INCLUDE)/bsp/bspMvme5500.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/gtreg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/GT64260TWSI.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/VPD.h

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

include $(top_srcdir)/../../../../../../automake/local.am
