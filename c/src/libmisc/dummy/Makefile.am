##
## $Id$
##


LIBNAME = libdummy-tmp
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = dummy.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/$(PGMNAME)$(LIB_VARIANT).rel

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = README dummy.c

include $(top_srcdir)/../../../automake/local.am
