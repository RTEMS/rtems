##
## $Id$
##


include_rtemsdir = $(includedir)/rtems

LIBNAME = libshell-tmp
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = cmds.c shell.c 
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

if RTEMS_LIBSHELL
include_rtems_HEADERS = shell.h
endif

include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

$(LIB): $(OBJS)
	$(make-library)

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

if RTEMS_LIBSHELL
all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB)
endif

.PRECIOUS: $(LIB)

EXTRA_DIST = README shell.c cmds.c shell.h

include $(top_srcdir)/../automake/local.am
