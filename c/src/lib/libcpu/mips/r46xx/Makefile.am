##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

EXTRA_PROGRAMS =
CLEANFILES =
noinst_DATA =

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

if r46xx
EXTRA_PROGRAMS += vectorisrs.rel
CLEANFILES += vectorisrs.rel
vectorisrs_rel_SOURCES = vectorisrs/maxvectors.c vectorisrs/vectorisrs.c
vectorisrs_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
vectorisrs_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += vectorisrs_g.rel
CLEANFILES += vectorisrs_g.rel
vectorisrs_g_rel_SOURCES = $(vectorisrs_rel_SOURCES)
vectorisrs_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
vectorisrs_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += vectorisrs$(LIB_VARIANT).rel
endif

include $(top_srcdir)/../../../automake/local.am
