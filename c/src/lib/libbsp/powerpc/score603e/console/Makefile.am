##
## $Id$
##


PGM = $(ARCH)/console.rel

C_FILES = 85c30.c console.c consolereserveresources.c tbl85c30.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = 85c30.c 85c30.h console.c consolebsp.h \
    consolereserveresources.c tbl85c30.c

include $(top_srcdir)/../../../../automake/local.am
