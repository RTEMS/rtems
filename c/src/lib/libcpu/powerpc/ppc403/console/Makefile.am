##
## $Id$
##


PGM = $(ARCH)/console.rel

if ppc403
C_FILES = console.c
endif
if ppc405
C_FILES = console405.c
endif

console_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(console_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(console_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = console.c console.c.polled console405.c

include $(top_srcdir)/../../../../../automake/local.am
