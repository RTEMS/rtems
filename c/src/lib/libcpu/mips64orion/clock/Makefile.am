## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/clock.rel

## C source names
C_FILES = ckinit.c

## Assembly source names
S_FILES = clock.S

H_FILES = clock.h
noinst_HEADERS = $(H_FILES)

clock_rel_OBJECTS = $(C_FILES:%.c=${ARCH}/%.o) $(S_FILES:%.S=${ARCH}/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(clock_rel_OBJECTS)
	$(make-rel)

all-local: ${ARCH} $(PGM)

EXTRA_DIST = $(S_FILES) $(C_FILES)

include $(top_srcdir)/../../../../../automake/local.am
