##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4




LIBNAME = lib.a
LIB = $(ARCH)/$(LIBNAME)

C_FILES = ftpd.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

#
# Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

all-local: $(PREINSTALL_FILES) $(ARCH) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = ftpd.c ftpd.h

include_HEADERS = ftpd.h

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

include $(top_srcdir)/../../../automake/local.am
