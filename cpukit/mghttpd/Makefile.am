include $(top_srcdir)/automake/compile.am

if LIBNETWORKING
if HAS_PTHREADS
project_lib_LIBRARIES = libmghttpd.a

$(PROJECT_LIB)/libmghttpd.a: libmghttpd.a
	$(INSTALL_DATA) $< $(PROJECT_LIB)/libmghttpd.a
TMPINSTALL_FILES = $(PROJECT_LIB)/libmghttpd.a

libmghttpd_a_CPPFLAGS = $(AM_CPPFLAGS)
# libmghttpd_a_CPPFLAGS += -DHAVE_MD5
libmghttpd_a_CPPFLAGS += -DNO_SSL -DNO_POPEN -DNO_CGI -DUSE_WEBSOCKET

libmghttpd_a_SOURCES = mongoose.c mongoose.h
endif
endif

include $(top_srcdir)/automake/local.am
