##
## $Id$
##


include_rtemsdir = $(includedir)/rtems

LIBNAME = libshell
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = cmds.c shell.c 
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

if LIBSHELL
include_rtems_HEADERS = shell.h
endif

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

AM_CPPFLAGS += -I$(top_builddir)

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

if LIBSHELL
all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB)
endif

.PRECIOUS: $(LIB)

EXTRA_DIST = README shell.c cmds.c shell.h

include $(top_srcdir)/automake/local.am
