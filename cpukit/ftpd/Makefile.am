##
## $Id$
##

include $(top_srcdir)/automake/compile.am

if HAS_NETWORKING
project_lib_LIBRARIES = libftpd.a
libftpd_a_SOURCES = ftpd.c ftpd.h
endif

all-local: $(PREINSTALL_FILES)

TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

if HAS_NETWORKING
$(PROJECT_LIB)/libftpd.a: libftpd.a $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/libftpd.a
TMPINSTALL_FILES += $(PROJECT_LIB)/libftpd.a
endif

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
