##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = include/confdefs.h
include_rtems_HEADERS += include/rtems/config.h include/rtems/extension.h \
    include/rtems/fatal.h include/rtems/init.h include/rtems/io.h \
    include/rtems/mptables.h include/rtems/sptables.h

EXTRA_DIST = include/rtems/README

include_rtems_HEADERS += inline/rtems/extension.inl

## src
AM_CPPFLAGS += -D__RTEMS_INSIDE__

noinst_LIBRARIES = libsapi.a
libsapi_a_SOURCES = src/debug.c src/extension.c src/extensioncreate.c \
    src/extensiondelete.c src/extensionident.c src/fatal.c src/exinit.c \
    src/exshutdown.c src/io.c src/itronapi.c src/posixapi.c src/rtemsapi.c \
    src/extensiondata.c
libsapi_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
