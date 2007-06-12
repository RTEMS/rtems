##
## $Id$
##

include $(top_srcdir)/automake/compile.am

if LIBNETWORKING
include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtemspppd.h rtemsdialer.h

project_lib_LIBRARIES = libpppd.a
libpppd_a_SOURCES = auth.c ccp.c ccp.h chap.c chap.h chap_ms.c chap_ms.h \
    chat.c demand.c fsm.c fsm.h ipcp.c ipcp.h lcp.c lcp.h magic.c magic.h \
    options.c upap.c upap.h utils.c sys-rtems.c \
    rtemsmain.c rtemspppd.c rtemspppd.h pppd.h patchlevel.h pathnames.h \
    rtemsdialer.h
libpppd_a_CPPFLAGS = $(AM_CPPFLAGS) -D__BSD_VISIBLE
endif

if NEWLIB
noinst_LIBRARIES = libmd.a
libmd_a_SOURCES = md5.h md5.c
libmd_a_SOURCES += md4.h md4.c
libmd_a_CPPFLAGS = $(AM_CPPFLAGS)
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
