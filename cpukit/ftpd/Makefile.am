##
## $Id$
##

LIBNAME = lib.a
LIB = $(ARCH)/$(LIBNAME)

C_FILES = ftpd.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../automake/compile.am
include $(top_srcdir)/../../automake/lib.am

#
# Add local stuff here using +=
#

EXTRA_DIST = ftpd.c ftpd.h

if HAS_NETWORKING
include_rtemsdir = $(includedir)/rtems

include_HEADERS = ftpd.h

include_rtems_HEADERS =

$(LIB): $(OBJS)
	$(make-library)

all-local: $(PREINSTALL_FILES) $(ARCH) $(OBJS) $(LIB)
endif

.PRECIOUS: $(LIB)

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
   $(include_HEADERS:%=$(PROJECT_INCLUDE)/%) \
   $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@
$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

include $(top_srcdir)/../../automake/local.am
