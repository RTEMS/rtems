include $(top_srcdir)/automake/compile.am

if LIBNETWORKING
if LIBSHELL
project_lib_LIBRARIES = libtelnetd.a

$(PROJECT_LIB)/libtelnetd.a: libtelnetd.a
	$(INSTALL_DATA) $< $(PROJECT_LIB)/libtelnetd.a
TMPINSTALL_FILES = $(PROJECT_LIB)/libtelnetd.a

libtelnetd_a_SOURCES = check_passwd.c des.c pty.c telnetd.c
libtelnetd_a_CPPFLAGS = $(AM_CPPFLAGS)
endif
endif

EXTRA_DIST = README

include $(top_srcdir)/automake/local.am
