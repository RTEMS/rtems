include $(top_srcdir)/automake/compile.am

project_lib_LIBRARIES = libz.a

$(PROJECT_LIB)/libz.a: libz.a
	$(INSTALL_DATA) $< $(PROJECT_LIB)/libz.a
TMPINSTALL_FILES = $(PROJECT_LIB)/libz.a


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

include $(top_srcdir)/automake/local.am
