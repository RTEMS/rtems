##
## $Id$
##

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

AM_CPPFLAGS += -DRTEMS_BSP=\"@RTEMS_BSP@\"

EXTRA_LIBRARIES = libsupport.a
CLEANFILES = libsupport.a
libsupport_a_SOURCES = version.c
libsupport_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libsupport_g.a
CLEANFILES += libsupport_g.a
libsupport_g_a_SOURCES = $(libsupport_a_SOURCES)
libsupport_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

noinst_DATA = libsupport$(LIB_VARIANT).a

include $(top_srcdir)/automake/local.am
