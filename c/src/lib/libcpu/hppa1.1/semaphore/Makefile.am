## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/semaphore.rel

## C source names
C_FILES = semaphore.c

semaphore_rel_OBJECTS = $(C_FILES:%.c=${ARCH}/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(semaphore_rel_OBJECTS)
	$(make-rel)

all-local: ${ARCH} $(PGM)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../../../automake/local.am
