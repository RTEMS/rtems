## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/timer.rel

C_FILES = timer.c

timer_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS = $(CFLAGS_OS_V)

$(PGM): $(timer_rel_OBJECTS)
	$(make-rel)

all: $(ARCH) $(timer_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = timer.c

include $(top_srcdir)/../../../../../automake/local.am
