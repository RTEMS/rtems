##
## $Id$
##


LIBNAME = libdummy
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = dummy.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = README dummy.c

include $(top_srcdir)/../automake/local.am
