##
## $Id$
##

include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = asm.h

include_rtems_mipsdir = $(includedir)/rtems/mips
include_rtems_mips_HEADERS = idtcpu.h iregdef.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/mips.h \
    rtems/score/types.h

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c cpu_asm.S
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/asm.h: asm.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/asm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/asm.h

$(PROJECT_INCLUDE)/rtems/mips/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/mips
	@: > $(PROJECT_INCLUDE)/rtems/mips/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/mips/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/mips/idtcpu.h: idtcpu.h $(PROJECT_INCLUDE)/rtems/mips/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/mips/idtcpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/mips/idtcpu.h

$(PROJECT_INCLUDE)/rtems/mips/iregdef.h: iregdef.h $(PROJECT_INCLUDE)/rtems/mips/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/mips/iregdef.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/mips/iregdef.h

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/cpu.h: rtems/score/cpu.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpu.h

$(PROJECT_INCLUDE)/rtems/score/mips.h: rtems/score/mips.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/mips.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/mips.h

$(PROJECT_INCLUDE)/rtems/score/types.h: rtems/score/types.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/types.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/types.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
