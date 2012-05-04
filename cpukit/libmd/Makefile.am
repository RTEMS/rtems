include $(top_srcdir)/automake/compile.am

if NEWLIB
noinst_LIBRARIES = libmd.a
libmd_a_SOURCES = md5.h md5.c
libmd_a_SOURCES += md4.h md4.c
libmd_a_CPPFLAGS = $(AM_CPPFLAGS)
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
