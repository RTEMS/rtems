## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/null.rel

## C source names
C_FILES = open.c close.c read.c write.c init.c cntrl.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

H_FILES =

## Assembly source names
S_FILES =
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/leaf.cfg

#
# (OPTIONAL) Add local stuff here using +=
#

${PGM}: ${OBJS}
	$(make-rel)

all: ${ARCH} $(PGM)

EXTRA_DIST = $(C_FILES) $(S_FILES)

include $(top_srcdir)/../../../../../automake/local.am
