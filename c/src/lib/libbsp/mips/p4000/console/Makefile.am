##
## $Id$
##


PGM = $(ARCH)/console.rel

C_FILES = console.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

S_FILES = led.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(ARCH) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = console.c led.S

include $(top_srcdir)/../../../../../../automake/local.am
