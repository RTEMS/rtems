##
## $Id$
##

include $(top_srcdir)/automake/compile.am

EXTRA_LIBRARIES =
CLEANFILES =

if HAS_NETWORKING
include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = pty.h telnetd.h

EXTRA_LIBRARIES += libtelnetd.a
CLEANFILES += libtelnetd.a
libtelnetd_a_SOURCES = pty.c pty.h telnetd.c telnetd.h icmds.c
libtelnetd_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libtelnetd_g.a
CLEANFILES += libtelnetd_g.a
libtelnetd_g_a_SOURCES = $(libtelnetd_a_SOURCES)
libtelnetd_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

noinst_DATA = libtelnetd$(LIB_VARIANT).a
endif

EXTRA_DIST = README

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if HAS_NETWORKING
$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/pty.h: pty.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/pty.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/pty.h

$(PROJECT_INCLUDE)/rtems/telnetd.h: telnetd.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/telnetd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/telnetd.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
