##
## $Id$
##


PGM = ${ARCH}/console-generic.rel

## C sources
C_FILES = console-generic.c

console_generic_rel_OBJECTS = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(console_generic_rel_OBJECTS)
	$(make-rel)

all-local: ${ARCH} $(PGM)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../../../automake/local.am
