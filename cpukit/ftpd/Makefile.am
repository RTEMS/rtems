##
## $Id$
##

LIBNAME = lib.a
LIB = $(ARCH)/$(LIBNAME)

C_FILES = ftpd.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

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

all-local: $(PREINSTALL_FILES) $(OBJS) $(LIB)
endif

.PRECIOUS: $(LIB)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)

if HAS_NETWORKING
$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/ftpd.h: ftpd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/ftpd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/ftpd.h
endif

include $(top_srcdir)/automake/local.am
