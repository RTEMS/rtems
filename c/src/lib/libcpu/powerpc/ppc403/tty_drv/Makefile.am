##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/tty_drv.rel

include_HEADERS = tty_drv.h

C_FILES = tty_drv.c

tty_drv_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(tty_drv_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(tty_drv_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = tty_drv.c tty_drv.h

include $(top_srcdir)/../../../../../automake/local.am
