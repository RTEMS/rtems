##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../shared/io

PGM = $(ARCH)/console.rel

## sources
C_FILES = console.c

console_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(console_rel_OBJECTS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by
# 	../wrapup/Makefile

all-local: $(ARCH) $(console_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = console.c

include $(top_srcdir)/../../../../automake/local.am
