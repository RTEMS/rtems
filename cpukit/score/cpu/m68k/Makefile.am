##
## $Id$
##

include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = asm.h

include_rtems_m68kdir = $(includedir)/rtems/m68k
include_rtems_m68k_HEADERS = rtems/m68k/m68302.h rtems/m68k/m68360.h \
    rtems/m68k/qsm.h rtems/m68k/sim.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/m68k.h \
    rtems/score/types.h

libscorecpu_a_SOURCES = cpu.c cpu_asm.S

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

$(PROJECT_INCLUDE)/rtems/m68k/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/m68k
	@: > $(PROJECT_INCLUDE)/rtems/m68k/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/m68k/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/m68k/m68302.h: rtems/m68k/m68302.h $(PROJECT_INCLUDE)/rtems/m68k/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/m68k/m68302.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/m68k/m68302.h

$(PROJECT_INCLUDE)/rtems/m68k/m68360.h: rtems/m68k/m68360.h $(PROJECT_INCLUDE)/rtems/m68k/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/m68k/m68360.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/m68k/m68360.h

$(PROJECT_INCLUDE)/rtems/m68k/qsm.h: rtems/m68k/qsm.h $(PROJECT_INCLUDE)/rtems/m68k/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/m68k/qsm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/m68k/qsm.h

$(PROJECT_INCLUDE)/rtems/m68k/sim.h: rtems/m68k/sim.h $(PROJECT_INCLUDE)/rtems/m68k/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/m68k/sim.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/m68k/sim.h

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/cpu.h: rtems/score/cpu.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpu.h

$(PROJECT_INCLUDE)/rtems/score/m68k.h: rtems/score/m68k.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/m68k.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/m68k.h

$(PROJECT_INCLUDE)/rtems/score/types.h: rtems/score/types.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/types.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/types.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
