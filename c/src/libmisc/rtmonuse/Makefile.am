##
## $Id$
##


include_rtemsdir = $(includedir)/rtems

LIBNAME = librtmonuse-tmp
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = rtmonuse.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include_rtems_HEADERS = rtmonuse.h

OBJS = $(C_O_FILES)

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

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS) $(LIB)

.PRECIOUS: $(LIB)

EXTRA_DIST = rtmonuse.c

include $(top_srcdir)/../../../automake/local.am
