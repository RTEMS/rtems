## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@:@srcdir@/../../shared

PGM = ${ARCH}/console.rel

## C source names
C_FILES = console.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): ${OBJS}
	$(make-rel)

all-local: ${ARCH} $(PGM)

# the .rel file built here will be put into libbsp.a by 
# 	../wrapup/Makefile

include $(top_srcdir)/../../../../../../automake/local.am
