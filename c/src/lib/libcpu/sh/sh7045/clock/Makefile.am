## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/clock.rel

## C source names
C_FILES = ckinit.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES =

## Assembly source names
S_FILES =
S_O_FILES = $(S_FILES:%.s=${ARCH}/%.o)

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/leaf.cfg

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS += $(CFLAGS_OS_V)

${PGM}: ${OBJS}
	${make-rel}

all: ${ARCH} $(PGM)

EXTRA_DIST = $(C_FILES) $(S_FILES)

include $(top_srcdir)/../../../../../automake/local.am
