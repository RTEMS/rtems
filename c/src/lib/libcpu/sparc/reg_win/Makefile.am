##
## $Id$
##


PGM = $(ARCH)/reg_win.rel

S_FILES = window.S
reg_win_rel_OBJECTS = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(reg_win_rel_OBJECTS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by
#    libbsp/sparc/BSP/wrapup/Makefile

all-local: $(ARCH) $(reg_win_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = window.S

include $(top_srcdir)/../../../../../automake/local.am
