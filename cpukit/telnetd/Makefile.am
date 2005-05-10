##
## $Id$
##

include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES =
CLEANFILES =
DISTCLEANFILES =

if HAS_NETWORKING
include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = pty.h telnetd.h

noinst_LIBRARIES += libtelnetd.a
libtelnetd_a_SOURCES = pty.c pty.h telnetd.c telnetd.h icmds.c
libtelnetd_a_CPPFLAGS = $(AM_CPPFLAGS)
endif

EXTRA_DIST = README

all-local: $(PREINSTALL_FILES)

include $(srcdir)/preinstall.am

include $(top_srcdir)/automake/local.am
