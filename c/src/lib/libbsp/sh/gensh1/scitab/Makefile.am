## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/scitab.rel

## C source names
C_FILES = scitab.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

SHGEN = $(PROJECT_ROOT)/tools/cpu/sh/shgen

#
# (OPTIONAL) Add local stuff here using +=
#

CLEANFILES = scitab.c

$(PGM): ${OBJS}
	$(make-rel)

all-local: ${ARCH} $(PGM)

scitab.c: $(SHGEN)
	$(SHGEN) -H $(HZ) sci > $@

include $(top_srcdir)/../../../../../../automake/local.am
