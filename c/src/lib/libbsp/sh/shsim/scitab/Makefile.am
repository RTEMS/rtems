##
## $Id$
##


PGM = $(ARCH)/scitab.rel

C_FILES = scitab.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

SHGEN = $(PROJECT_TOPdir)/tools/cpu/sh/shgen

#
# (OPTIONAL) Add local stuff here using +=
#

CLEANFILES = scitab.c

$(PGM): $(OBJS)
	$(make-rel)

scitab.c: $(SHGEN)
	$(SHGEN) -H @CPU_CLOCK_RATE_HZ@ sci > $@

all-local: $(ARCH) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

include $(top_srcdir)/../../../../../../automake/local.am
