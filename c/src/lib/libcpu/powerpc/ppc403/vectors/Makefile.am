##
## $Id$
##

PGM = ${ARCH}/vectors.rel

## Assembly sources
S_FILES = vectors.S align_h.S

vectors_rel_OBJECTS = $(S_FILES:%.S=${ARCH}/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(vectors_rel_OBJECTS)
	$(make-rel)

all-local: $(PGM)

EXTRA_DIST = $(S_FILES) README

include $(top_srcdir)/../../../automake/local.am
