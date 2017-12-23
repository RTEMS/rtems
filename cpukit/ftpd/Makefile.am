include $(top_srcdir)/automake/compile.am

if LIBNETWORKING
project_lib_LIBRARIES = libftpd.a

$(PROJECT_LIB)/libftpd.a: libftpd.a
	$(INSTALL_DATA) $< $(PROJECT_LIB)/libftpd.a
TMPINSTALL_FILES = $(PROJECT_LIB)/libftpd.a

libftpd_a_SOURCES = ftpd.c ftpd.h
endif

include $(top_srcdir)/automake/local.am
