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

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(PGMNAME)$(LIB_VARIANT).rel

all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = README dummy.c

include $(top_srcdir)/../automake/local.am
