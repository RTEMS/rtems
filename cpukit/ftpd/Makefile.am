##
## $Id$
##

include $(top_srcdir)/automake/compile.am

if HAS_NETWORKING
include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = ftpd.h

project_lib_LIBRARIES = libftpd.a
libftpd_a_SOURCES = ftpd.c ftpd.h
libftpd_a_CPPFLAGS = $(AM_CPPFLAGS)
endif

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =
TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

if HAS_NETWORKING
$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/ftpd.h: ftpd.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/ftpd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/ftpd.h

$(PROJECT_LIB)/libftpd.a: libftpd.a $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/libftpd.a
TMPINSTALL_FILES += $(PROJECT_LIB)/libftpd.a
endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
