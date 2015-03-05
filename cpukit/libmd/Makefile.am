include $(top_srcdir)/automake/compile.am

if NEWLIB
include_HEADERS =
include_HEADERS += md4.h
include_HEADERS += md5.h
include_HEADERS += sha256.h
include_HEADERS += sha512.h

noinst_LIBRARIES = libmd.a
libmd_a_SOURCES =
libmd_a_SOURCES += md4.c
libmd_a_SOURCES += md5.c
if SHA
libmd_a_SOURCES += sha256c.c
libmd_a_SOURCES += sha512c.c
endif
libmd_a_CPPFLAGS = $(AM_CPPFLAGS)
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
