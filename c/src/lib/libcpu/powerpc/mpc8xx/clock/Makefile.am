##
## $Id$
##


PGM = ${ARCH}/clock.rel

## C sources
C_FILES = clock.c

clock_rel_OBJECTS = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(clock_rel_OBJECTS)
	$(make-rel)

all-local: ${ARCH} $(PGM)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../automake/local.am
