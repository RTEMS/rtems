##
## $Id$
##


LIBNAME = libtelnetd-tmp
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = pty.c telnetd.c icmds.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))


include $(top_srcdir)/../../automake/compile.am
include $(top_srcdir)/../../automake/lib.am

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

#
# (OPTIONAL) Add local stuff here using +=
#

if HAS_NETWORKING
include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = pty.h telnetd.h

$(LIB): $(OBJS)
	$(make-library)

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS) $(LIB)
endif

.PRECIOUS: $(LIB)

EXTRA_DIST = README pty.c telnetd.c pty.h icmds.c telnetd.h

include $(top_srcdir)/../../automake/local.am
