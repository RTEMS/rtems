##
## $Id$
##

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

CLEANFILES =
EXTRA_LIBRARIES =

if HAS_NETWORKING
include_HEADERS = ftpd.h

EXTRA_LIBRARIES += libftpd.a
CLEANFILES += libftpd.a
libftpd_a_SOURCES = ftpd.c ftpd.h
libftpd_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libftpd_g.a
CLEANFILES += libftpd_g.a
libftpd_g_a_SOURCES = $(libftpd_a_SOURCES)
libftpd_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

noinst_DATA = libftpd$(LIB_VARIANT).a
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

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
