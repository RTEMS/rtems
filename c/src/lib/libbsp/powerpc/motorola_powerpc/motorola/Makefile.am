##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/motorola

C_FILES = motorola.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

# FIXME: This doesn't give any sense here
include_bspdir = $(includedir)/bsp
nodist_include_bsp_HEADERS = $(top_srcdir)/../shared/motorola/motorola.h

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(ARCH) $(OBJS)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/bsp:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/bsp
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp

$(PROJECT_INCLUDE)/bsp/motorola.h: \
    $(top_srcdir)/../shared/motorola/motorola.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bsp/motorola.h

include $(top_srcdir)/../../../../automake/local.am
