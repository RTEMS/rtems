##
## $Id$
##


PGM = $(ARCH)/sci.rel

C_FILES = sh4uart.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

all-local: $(ARCH) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

## FIXME: Moved to libbsp/sh/<BSP>/console/
## Needs to be reworked to be usable here.
EXTRA_DIST = console.c

include $(top_srcdir)/../../../../../../automake/local.am
