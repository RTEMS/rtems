include $(top_srcdir)/automake/compile.am

project_lib_LIBRARIES = libftpd.a

$(PROJECT_LIB)/libftpd.a: libftpd.a
	$(INSTALL_DATA) $< $(PROJECT_LIB)/libftpd.a
TMPINSTALL_FILES = $(PROJECT_LIB)/libftpd.a

libftpd_a_SOURCES = ftpd.c ftpd-init.c

include $(top_srcdir)/automake/local.am
