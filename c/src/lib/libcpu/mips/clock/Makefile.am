##
## $Id$
##


PGM = $(ARCH)/clock.rel

C_FILES = ckinit.c

S_FILES = clock.S

noinst_HEADERS = clock.h

clock_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT)) $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(clock_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(clock_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = ckinit.c clock.S clock.h

include $(top_srcdir)/../../../automake/local.am
