##
## $Id$
##


PGM = $(ARCH)/timer.rel

## sources
C_FILES = timer.c
S_FILES = timerisr.S
timer_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT)) $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(timer_rel_OBJECTS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by
# 	../wrapup/Makefile

all-local: $(timer_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = timer.c timerisr.S

include $(top_srcdir)/../../../../automake/local.am
