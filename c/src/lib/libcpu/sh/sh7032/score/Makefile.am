##
## $Id$
##


PGM = $(ARCH)/score.rel

C_FILES = cpu_asm.c ispsh7032.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

all-local: $(ARCH) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = ispsh7032.c

include $(top_srcdir)/../../../../../../automake/local.am
