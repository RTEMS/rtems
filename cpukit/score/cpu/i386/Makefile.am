##
## $Id$
##

include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS= asm.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/i386.h \
    rtems/score/types.h rtems/score/interrupts.h rtems/score/registers.h \
    rtems/score/idtr.h

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c cpu_asm.S
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

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

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/cpu.h: rtems/score/cpu.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpu.h

$(PROJECT_INCLUDE)/rtems/score/i386.h: rtems/score/i386.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/i386.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/i386.h

$(PROJECT_INCLUDE)/rtems/score/types.h: rtems/score/types.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/types.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/types.h

$(PROJECT_INCLUDE)/rtems/score/interrupts.h: rtems/score/interrupts.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/interrupts.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/interrupts.h

$(PROJECT_INCLUDE)/rtems/score/registers.h: rtems/score/registers.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/registers.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/registers.h

$(PROJECT_INCLUDE)/rtems/score/idtr.h: rtems/score/idtr.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/idtr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/idtr.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/automake/local.am
