##
## $Id$
##

PGM = $(ARCH)/network.rel

C_FILES = network.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

noinst_HEADERS = netexterns.h uti596.h

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

if HAS_NETWORKING
all-local: $(OBJS) $(PGM)
endif

.PRECIOUS: $(PGM)

EXTRA_DIST = netexterns.h network.c uti596.h

include $(top_srcdir)/../../../../automake/local.am
