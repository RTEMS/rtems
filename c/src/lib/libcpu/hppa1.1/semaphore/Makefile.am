##
## $Id$
##


PGM = $(ARCH)/semaphore.rel

C_FILES = semaphore.c

semaphore_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(semaphore_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(semaphore_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = semaphore.c

include $(top_srcdir)/../../../automake/local.am
