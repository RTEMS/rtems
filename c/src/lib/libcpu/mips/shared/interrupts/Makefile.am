##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/interrupts.rel

C_FILES = installisrentries.c maxvectors.c

S_FILES = isr_entries.S

interrupts_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o) $(S_FILES:%.S=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(interrupts_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(interrupts_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = maxvectors.c

include $(top_srcdir)/../../../../../automake/local.am
