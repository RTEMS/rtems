##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am

CLEANFILES =
noinst_PROGRAMS =

if sh7750
# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts
include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score

include_sh_HEADERS = include/sh/sh4uart.h
include_rtems_score_HEADERS = include/rtems/score/sh7750_regs.h \
    include/rtems/score/ipl.h include/rtems/score/iosh7750.h \
    include/rtems/score/ispsh7750.h include/rtems/score/sh4_regs.h

## clock
noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/ckinit.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## sci
noinst_PROGRAMS += sci.rel
sci_rel_SOURCES = sci/sh4uart.c
sci_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
sci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## score
noinst_PROGRAMS += score.rel
score_rel_SOURCES = score/cpu_asm.c score/ispsh7750.c
score_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
score_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## timer
noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

## --

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if sh7750
$(PROJECT_INCLUDE)/sh/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/sh
	@: > $(PROJECT_INCLUDE)/sh/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/sh/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/sh/sh4uart.h: include/sh/sh4uart.h $(PROJECT_INCLUDE)/sh/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sh/sh4uart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sh/sh4uart.h

$(PROJECT_INCLUDE)/rtems/score/sh7750_regs.h: include/rtems/score/sh7750_regs.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/sh7750_regs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/sh7750_regs.h

$(PROJECT_INCLUDE)/rtems/score/ipl.h: include/rtems/score/ipl.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/ipl.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/ipl.h

$(PROJECT_INCLUDE)/rtems/score/iosh7750.h: include/rtems/score/iosh7750.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/iosh7750.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/iosh7750.h

$(PROJECT_INCLUDE)/rtems/score/ispsh7750.h: include/rtems/score/ispsh7750.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/ispsh7750.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/ispsh7750.h

$(PROJECT_INCLUDE)/rtems/score/sh4_regs.h: include/rtems/score/sh4_regs.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/sh4_regs.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/sh4_regs.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
