##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

# C source names
C_FILES = cpu.c ppccache.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

ROOT_H_FILES =
RTEMS_SCORE_H_FILES = cpu.h c_isr.inl
noinst_HEADERS = $(ROOT_H_FILES) $(RTEMS_SCORE_H_FILES)

# Assembly source names
S_FILES = cpu_asm.S
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/rtems/score \
    $(ROOT_H_FILES:%=$(PROJECT_INCLUDE)/%) \
    $(RTEMS_SCORE_H_FILES:%=$(PROJECT_INCLUDE)/rtems/score/%)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/rtems$(LIB_VARIANT).o

$(PROJECT_RELEASE)/lib/rtems$(LIB_VARIANT).o: $(ARCH)/rtems.o
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%: %
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/%: %
	$(INSTALL_DATA) $< $@

REL = ../$(ARCH)/rtems-cpu.rel

$(REL): $(C_O_FILES) $(S_O_FILES)
	test -d ../$(ARCH) || mkdir ../$(ARCH)
	$(make-rel)

all-local: $(ARCH) $(PREINSTALL_FILES) $(REL) $(ARCH)/rtems.o $(TMPINSTALL_FILES)

clean-local:
	$(RM) -r ../o-optimize ../o-debug ../o-profile

UNUSED_FILES = irq_stub.S

EXTRA_DIST = TODO rtems.S $(C_FILES) $(S_FILES) $(UNUSED_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
