##
## $Id$
##

include $(top_srcdir)/automake/compile.am

CLEANFILES =
DISTCLEANFILES =

if HAS_NETWORKING
if LIBSHELL
include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = pty.h telnetd.h

project_lib_LIBRARIES = libtelnetd.a
libtelnetd_a_SOURCES = pty.c pty.h telnetd.c telnetd.h icmds.c
libtelnetd_a_CPPFLAGS = $(AM_CPPFLAGS)
endif
endif

EXTRA_DIST = README

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

include $(srcdir)/preinstall.am

include $(top_srcdir)/automake/local.am
