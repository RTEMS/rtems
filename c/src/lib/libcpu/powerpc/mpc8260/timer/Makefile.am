##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/timer.rel

## C sources
C_FILES = timer.c

timer_rel_OBJECTS = $(C_FILES:%.c=${ARCH}/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(timer_rel_OBJECTS)
	$(make-rel)

all-local: ${ARCH} $(PGM)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../../../automake/local.am
