##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/amcompile.am

include_HEADERS = asm.h idtcpu.h iregdef.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/mips.h \
    rtems/score/types.h

EXTRA_LIBRARIES = libscorecpu.a
CLEANFILES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c cpu_asm.S
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libscorecpu_g.a
CLEANFILES += libscorecpu_g.a
libscorecpu_g_a_SOURCES = $(libscorecpu_a_SOURCES)
libscorecpu_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

all-local: $(PREINSTALL_FILES) libscorecpu$(LIB_VARIANT).a

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/asm.h: asm.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/asm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/asm.h

$(PROJECT_INCLUDE)/idtcpu.h: idtcpu.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/idtcpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/idtcpu.h

$(PROJECT_INCLUDE)/iregdef.h: iregdef.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/iregdef.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/iregdef.h

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/cpu.h: rtems/score/cpu.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpu.h

$(PROJECT_INCLUDE)/rtems/score/mips.h: rtems/score/mips.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/mips.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/mips.h

$(PROJECT_INCLUDE)/rtems/score/types.h: rtems/score/types.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/types.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/types.h

include $(top_srcdir)/../../../automake/local.am
