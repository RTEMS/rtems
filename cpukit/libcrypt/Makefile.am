include $(top_srcdir)/automake/compile.am

include_HEADERS =

noinst_LIBRARIES = libcrypt.a

libcrypt_a_CPPFLAGS = $(AM_CPPFLAGS)

libcrypt_a_SOURCES =
libcrypt_a_SOURCES += crypt.c
libcrypt_a_SOURCES += crypt-md5.c
libcrypt_a_SOURCES += crypt-sha256.c
libcrypt_a_SOURCES += crypt-sha512.c
libcrypt_a_SOURCES += misc.c

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
