##
## $Id$
##


include_rtemsdir = $(includedir)/rtems

LIBNAME = libuntar-tmp
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = untar.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_rtems_HEADERS = untar.h

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

all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = README untar.c

include $(top_srcdir)/../automake/local.am
