##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am

AM_CPPFLAGS += -DCPU_SYNC_IO $(LIBC_DEFINES)

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/types.h rtems/score/cpu.h \
    rtems/score/unix.h rtems/score/unixsize.h

EXTRA_LIBRARIES = libscorecpu.a
CLEANFILES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libscorecpu_g.a
CLEANFILES += libscorecpu_g.a
libscorecpu_g_a_SOURCES = $(libscorecpu_a_SOURCES)
libscorecpu_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

all-local: $(PREINSTALL_FILES) libscorecpu$(LIB_VARIANT).a

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/types.h: rtems/score/types.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/types.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/types.h

$(PROJECT_INCLUDE)/rtems/score/cpu.h: rtems/score/cpu.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpu.h

$(PROJECT_INCLUDE)/rtems/score/unix.h: rtems/score/unix.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/unix.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/unix.h

$(PROJECT_INCLUDE)/rtems/score/unixsize.h: rtems/score/unixsize.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/unixsize.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/unixsize.h

CLEANFILES += $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
