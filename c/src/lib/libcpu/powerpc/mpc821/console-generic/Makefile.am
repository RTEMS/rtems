##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/console-generic.rel

C_FILES = console-generic.c

console_generic_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

AM_CFLAGS = $(CFLAGS_OS_V)

$(PGM): $(console_generic_rel_OBJECTS)
	$(make-rel)

all: $(ARCH) $(console_generic_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = console-generic.c

include $(top_srcdir)/../../../../../automake/local.am
