##
## $Id$
##


include_rtemsdir = $(includedir)/rtems

LIBNAME = libdevnull
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = devnull.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_rtems_HEADERS = devnull.h

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

all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = devnull.c

include $(top_srcdir)/automake/local.am
