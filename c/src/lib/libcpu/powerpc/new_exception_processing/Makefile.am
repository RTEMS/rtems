##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

C_FILES = cpu.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

include_rtems_scoredir = $(includedir)/rtems/score

include_rtems_score_HEADERS = cpu.h c_isr.inl

S_FILES = cpu_asm.S
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

PREINSTALL_FILES = $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%=$(PROJECT_INCLUDE)/rtems/score/%)

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%: %
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/%: %
	$(INSTALL_DATA) $< $@

REL = $(ARCH)/rtems-cpu.rel

$(REL): $(C_O_FILES) $(S_O_FILES)
	test -d ../$(ARCH) || mkdir ../$(ARCH)
	$(make-rel)

all-local: $(ARCH) $(PREINSTALL_FILES) $(REL)
## all-local: $(ARCH)/rtems.o
all-local: $(TMPINSTALL_FILES)

CLEANDIRS = ../o-optimize ../o-debug ../o-profile

EXTRA_DIST = $(C_FILES) $(S_FILES)

include $(top_srcdir)/../../../../../automake/local.am
