##
## $Id$
##

LIBNAME = libtelnetd-tmp
LIB = $(ARCH)/$(LIBNAME).a

C_FILES = pty.c telnetd.c icmds.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

if HAS_NETWORKING
include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = pty.h telnetd.h

$(LIB): $(OBJS)
	$(make-library)

all-local: $(PREINSTALL_FILES) $(OBJS) $(LIB)
endif

.PRECIOUS: $(LIB)

EXTRA_DIST = README pty.c telnetd.c pty.h icmds.c telnetd.h

PREINSTALL_FILES =

if HAS_NETWORKING
$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/pty.h: pty.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/pty.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/pty.h

$(PROJECT_INCLUDE)/rtems/telnetd.h: telnetd.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/telnetd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/telnetd.h
endif

include $(top_srcdir)/automake/local.am
