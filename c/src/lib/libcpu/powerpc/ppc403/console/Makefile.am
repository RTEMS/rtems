## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/console.rel

C_FILES = console.c

console_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS = $(CFLAGS_OS_V)

$(PGM): $(console_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(console_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = console.c console.c.polled

include $(top_srcdir)/../../../../../automake/local.am
