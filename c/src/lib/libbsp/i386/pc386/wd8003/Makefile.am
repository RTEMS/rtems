##
## $Id$
##


PGM = $(ARCH)/wd8003.rel

C_FILES = wd8003.c
wd8003_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__

$(PGM): $(wd8003_rel_OBJECTS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

if HAS_NETWORKING
all-local: $(ARCH) $(wd8003_rel_OBJECTS) $(PGM)
endif

.PRECIOUS: $(PGM)

EXTRA_DIST = wd8003.c

include $(top_srcdir)/../../../../../../automake/local.am
