##
## $Id$
##

include $(top_srcdir)/automake/compile.am

AM_CPPFLAGS += -DWEBS -DUEMF

if LIBNETWORKING
## Always install this header file.  Both webservers use it.
include_HEADERS = rtems_webserver.h

if HAS_PTHREADS
include_goaheaddir = $(includedir)/goahead

include_goahead_HEADERS = ej.h ejIntrn.h emfdb.h md5.h uemf.h um.h webs.h \
    wsIntrn.h

project_lib_LIBRARIES = libhttpd.a
libhttpd_a_SOURCES = asp.c balloc.c default.c ejlex.c ejparse.c emfdb.c \
    form.c h.c handler.c md5c.c mime.c misc.c webpage.c ringq.c rom.c \
    security.c sock.c socket.c sym.c uemf.c um.c url.c value.c wbase64.c \
    webrom.c webs.c websuemf.c webmain.c
libhttpd_a_CPPFLAGS = $(AM_CPPFLAGS)
endif
endif

EXTRA_DIST = webcomp.c webpage.c

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
