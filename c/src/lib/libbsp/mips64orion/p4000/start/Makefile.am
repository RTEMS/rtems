##
## $Id$
##

S_FILES = start.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(S_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am
include $(top_srcdir)/../../bspstart.am

#
# (OPTIONAL) Add local stuff here using +=
#

install-data-local: ${PROJECT_RELEASE}/lib/start$(LIB_VARIANT).$(OBJEXT)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = start.S

include $(top_srcdir)/../../../../automake/local.am
