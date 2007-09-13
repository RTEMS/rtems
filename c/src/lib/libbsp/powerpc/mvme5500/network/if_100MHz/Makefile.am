##
## $Id$
##

VPATH = @srcdir@:

INCLUDES = -I @srcdir@/../GT64260

C_FILES = GT64260eth.c
include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = GT64260eth.h GT64260ethreg.h

H_FILES = GT64260eth.h GT64260ethreg.h

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

EXTRA_DIST = GT64260eth.c

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

CFLAGS   += -msoft-float

#
# (OPTIONAL) Add local stuff here using +=
#

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $<

$(PROJECT_INCLUDE)/bsp/GT64260eth.h: GT64260eth.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/GT64260ethreg.h: GT64260ethreg.h
	$(INSTALL_DATA) $< $@


# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile
PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp $(PROJECT_INCLUDE)/bsp/GT64260eth.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/GT64260ethreg.h

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

include $(top_srcdir)/../../../../../../automake/local.am
