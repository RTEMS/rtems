##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/syscall.rel

## Assembly sources
S_FILES = syscall.S

H_FILES = syscall.h

syscall_rel_OBJECTS = $(S_FILES:%.S=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS = $(CFLAGS_OS_V)

$(PGM): $(syscall_rel_OBJECTS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by
#    libbsp/sparc/BSP/wrapup/Makefile

all: $(ARCH) $(syscall_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = syscall.S syscall.h

include $(top_srcdir)/../../../../../automake/local.am
