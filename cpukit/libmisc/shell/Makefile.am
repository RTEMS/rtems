##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtemsdir = $(includedir)/rtems

LIBNAME = libshell-tmp
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = cmds.c shell.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

include_rtems_HEADERS = shell.h

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = README shell.c cmds.c shell.h

include $(top_srcdir)/../../../automake/local.am
