##
## $Id$
##

include $(top_srcdir)/automake/compile.am

CLEANFILES =
DISTCLEANFILES =

if HAS_NETWORKING
project_lib_LIBRARIES = libftpd.a
libftpd_a_SOURCES = ftpd.c ftpd.h
endif

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

include $(srcdir)/preinstall.am

include $(top_srcdir)/automake/local.am
