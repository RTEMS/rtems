include $(top_srcdir)/automake/compile.am

if LIBNETWORKING
include_mghttpddir = $(includedir)/mghttpd

project_lib_LIBRARIES = libmghttpd.a
libmghttpd_a_CPPFLAGS = $(AM_CPPFLAGS)
# libmghttpd_a_CPPFLAGS += -DHAVE_MD5
libmghttpd_a_CPPFLAGS += -DNO_SSL -DNO_POPEN -DNO_CGI

libmghttpd_a_SOURCES = mongoose.c mongoose.h
include_mghttpd_HEADERS = mongoose.h
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
