##
## $Id$
##


PGM = $(ARCH)/clock.rel

C_FILES = ckinit.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

noinst_HEADERS = clock.h

S_FILES = clock.S
OBJS += $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

all-local: $(ARCH) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = Makefile ckinit.c clock.S clock.h

include $(top_srcdir)/../../../../../automake/local.am
