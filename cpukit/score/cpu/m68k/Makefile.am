##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include_HEADERS = asm.h m68302.h m68360.h qsm.h sim.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/m68k.h \
    rtems/score/types.h

C_FILES = cpu.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = cpu_asm.S
OBJS += $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

LIB = $(ARCH)/libscorecpu.a

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB) \
    $(TMPINSTALL_FILES)

EXTRA_DIST = cpu.c cpu_asm.S

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/$(dirstamp)

$(PROJECT_INCLUDE)/asm.h: asm.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/asm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/asm.h

$(PROJECT_INCLUDE)/m68302.h: m68302.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/m68302.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/m68302.h

$(PROJECT_INCLUDE)/m68360.h: m68360.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/m68360.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/m68360.h

$(PROJECT_INCLUDE)/qsm.h: qsm.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/qsm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/qsm.h

$(PROJECT_INCLUDE)/sim.h: sim.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sim.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sim.h

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/cpu.h: rtems/score/cpu.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/cpu.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpu.h

$(PROJECT_INCLUDE)/rtems/score/m68k.h: rtems/score/m68k.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/m68k.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/m68k.h

$(PROJECT_INCLUDE)/rtems/score/types.h: rtems/score/types.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/types.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/types.h

include $(top_srcdir)/../../../automake/local.am
