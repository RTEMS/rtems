##
## $Id$
##

PGM = $(ARCH)/syscall.rel

## Assembly sources
S_FILES = syscall.S

noinst_HEADERS = syscall.h

syscall_rel_OBJECTS = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(syscall_rel_OBJECTS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by
#    libbsp/sparc/BSP/wrapup/Makefile

all-local: $(ARCH) $(syscall_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = syscall.S syscall.h

include $(top_srcdir)/../../../automake/local.am
