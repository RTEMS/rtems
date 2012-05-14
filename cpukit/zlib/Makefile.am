include $(top_srcdir)/automake/compile.am

project_lib_LIBRARIES = libz.a

libz_a_SOURCES = adler32.c
libz_a_SOURCES += compress.c
libz_a_SOURCES += crc32.c crc32.h
libz_a_SOURCES += deflate.c deflate.h
libz_a_SOURCES += gzclose.c gzguts.h gzlib.c gzread.c gzwrite.c
libz_a_SOURCES += infback.c
libz_a_SOURCES += inffast.c inffast.h
libz_a_SOURCES += inffixed.h
libz_a_SOURCES += inflate.c inflate.h
libz_a_SOURCES += inftrees.c inftrees.h
libz_a_SOURCES += trees.c trees.h
libz_a_SOURCES += uncompr.c
libz_a_SOURCES += zconf.h
libz_a_SOURCES += zlib.h
libz_a_SOURCES += zutil.c zutil.h

include_HEADERS = zlib.h zconf.h

zconf.h: $(srcdir)/zconf.h.in
	sed \
	  -e 's,#ifdef Z_PREFIX.* may be ,#if 1 /* was ,' \
	  -e 's,#ifdef HAVE_UNISTD_H.* may be ,#if 1 /* was ,' \
	< $(srcdir)/zconf.h.in > zconf.h
CLEANFILES = zconf.h
EXTRA_DIST = zconf.h.in

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
