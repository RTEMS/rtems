##
## $Id$
##

PGM = $(ARCH)/sci.rel

C_FILES = gdbsci.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

all-local: $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = sci.c

include $(top_srcdir)/../../../../automake/local.am
