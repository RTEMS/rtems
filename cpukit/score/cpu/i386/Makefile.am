##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

include_HEADERS= asm.h
PREINSTALL_FILES = $(PROJECT_INCLUDE) $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = \
    rtems/score/cpu.h \
    rtems/score/i386.h \
    rtems/score/types.h \
    rtems/score/interrupts.h \
    rtems/score/registers.h \
    rtems/score/idtr.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%.h=$(PROJECT_INCLUDE)/%.h)

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

include $(top_srcdir)/../../../automake/local.am
