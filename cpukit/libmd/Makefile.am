include $(top_srcdir)/automake/compile.am

if NEWLIB
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

include $(top_srcdir)/automake/local.am
