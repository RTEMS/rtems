##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include_HEADERS= asm.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/i386.h \
    rtems/score/types.h rtems/score/interrupts.h rtems/score/registers.h \
    rtems/score/idtr.h

LIB = $(ARCH)/libscorecpu.a

C_FILES = cpu.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = cpu_asm.S
OBJS += $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB) \
    $(TMPINSTALL_FILES)

EXTRA_DIST = cpu.c cpu_asm.S

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/asm.h: asm.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/asm.h

$(PROJECT_INCLUDE)/rtems/score:
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/rtems/score
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score

$(PROJECT_INCLUDE)/rtems/score/cpu.h: rtems/score/cpu.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpu.h

$(PROJECT_INCLUDE)/rtems/score/i386.h: rtems/score/i386.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/i386.h

$(PROJECT_INCLUDE)/rtems/score/types.h: rtems/score/types.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/types.h

$(PROJECT_INCLUDE)/rtems/score/interrupts.h: rtems/score/interrupts.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/interrupts.h

$(PROJECT_INCLUDE)/rtems/score/registers.h: rtems/score/registers.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/registers.h

$(PROJECT_INCLUDE)/rtems/score/idtr.h: rtems/score/idtr.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/idtr.h

include $(top_srcdir)/../../../automake/local.am
