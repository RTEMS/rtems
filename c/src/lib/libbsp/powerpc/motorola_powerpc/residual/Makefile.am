##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@:@srcdir@/../../shared/residual

C_FILES = residual.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

H_FILES = ../../shared/residual/pnp.h ../../shared/residual/residual.h

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PROJECT_INCLUDE)/bsp:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp/pnp.h: ../../shared/residual/pnp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/residual.h: ../../shared/residual/residual.h
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile
PREINSTALL_FILES = $(PROJECT_INCLUDE)/bsp $(PROJECT_INCLUDE)/bsp/pnp.h \
    $(PROJECT_INCLUDE)/bsp/residual.h

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS)

include $(top_srcdir)/../../../../../../automake/local.am
