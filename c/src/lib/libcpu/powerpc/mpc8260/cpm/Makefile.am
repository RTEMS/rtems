##
## $Id$
##

PGM = ${ARCH}/cp.rel

## C sources
C_FILES = cp.c dpram.c brg.c

cpm_rel_OBJECTS = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(cpm_rel_OBJECTS)
	$(make-rel)

all-local: ${ARCH} $(PGM)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../automake/local.am
