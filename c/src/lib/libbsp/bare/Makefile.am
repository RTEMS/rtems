##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

nodist_include_HEADERS += ../shared/include/coverhd.h

EXTRA_DIST = build-tools do-bare-bsp-build

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/local.am
