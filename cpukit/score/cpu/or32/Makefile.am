##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

include_HEADERS= asm.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/types.h \
    rtems/score/or32.h rtems/score/cpu_asm.h

C_FILES = cpu.c cpu_asm.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

LIB = $(ARCH)/libscorecpu.a

$(LIB): $(OBJS)
	$(make-library)

all-local: $(ARCH) $(PREINSTALL_FILES) $(LIB) \
    $(TMPINSTALL_FILES)

EXTRA_DIST = cpu.c cpu_asm.c

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

$(PROJECT_INCLUDE)/rtems/score/types.h: rtems/score/types.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/types.h

$(PROJECT_INCLUDE)/rtems/score/or32.h: rtems/score/or32.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/or32.h

$(PROJECT_INCLUDE)/rtems/score/cpu_asm.h: rtems/score/cpu_asm.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpu_asm.h

include $(top_srcdir)/../../../automake/local.am
