## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/clock.rel

## C source names
C_FILES = ckinit.c
clock_rel_OBJECTS = $(C_FILES:%.c=${ARCH}/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(clock_rel_OBJECTS)
	$(make-rel)

all-local: ${ARCH} $(PGM)

# the .rel file built here will be put into libbsp.a by 
# 	../wrapup/Makefile

EXTRA_DIST = ckinit.c

include $(top_srcdir)/../../../../../../automake/local.am
