##
## $Id$
##

PGM = $(ARCH)/timer.rel

C_FILES = timer.c

timer_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(timer_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(timer_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = timer.c

include $(top_srcdir)/../../../automake/local.am
