##
## $Id$
##


PGM = $(ARCH)/vectorisrs.rel

C_FILES = maxvectors.c vectorisrs.c

vectorisrs_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(vectorisrs_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(vectorisrs_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = vectorisrs.c

include $(top_srcdir)/../../../../../automake/local.am
