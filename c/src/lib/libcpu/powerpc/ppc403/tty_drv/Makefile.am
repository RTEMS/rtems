##
## $Id$
##


PGM = $(ARCH)/tty_drv.rel

include_HEADERS = tty_drv.h

C_FILES = tty_drv.c

tty_drv_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(tty_drv_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(tty_drv_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = tty_drv.c tty_drv.h

include $(top_srcdir)/../../../automake/local.am
