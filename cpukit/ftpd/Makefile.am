##
## $Id$
##

include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES =

if HAS_NETWORKING
include_HEADERS = ftpd.h

noinst_LIBRARIES += libftpd.a
libftpd_a_SOURCES = ftpd.c ftpd.h
libftpd_a_CPPFLAGS = $(AM_CPPFLAGS)
endif

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/$(dirstamp)

if HAS_NETWORKING
$(PROJECT_INCLUDE)/ftpd.h: ftpd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/ftpd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/ftpd.h
endif

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
