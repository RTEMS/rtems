##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts
include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score

include_sh_HEADERS = include/sh/sh4uart.h
include_rtems_score_HEADERS = include/rtems/score/sh7750_regs.h \
    include/rtems/score/ipl.h include/rtems/score/iosh7750.h \
    include/rtems/score/ispsh7750.h include/rtems/score/sh4_regs.h

all-local: $(TMPINSTALL_FILES)

## clock

PGMS = $(ARCH)/clock.rel

clock_C_FILES = clock/ckinit.c
clock_rel_OBJECTS = $(clock_C_FILES:clock/%.c=$(ARCH)/%.$(OBJEXT))
EXTRA_DIST = $(clock_C_FILES)

${ARCH}/%.$(OBJEXT): clock/%.c
	${COMPILE} -o $@ -c $<
${ARCH}/clock.rel: $(clock_rel_OBJECTS)
	${make-rel}

## sci

PGMS += $(ARCH)/sci.rel

sci_C_FILES = sci/sh4uart.c
sci_rel_OBJECTS = $(sci_C_FILES:sci/%.c=$(ARCH)/%.$(OBJEXT))
EXTRA_DIST += $(sci_C_FILES)

${ARCH}/%.$(OBJEXT): sci/%.c
	${COMPILE} -o $@ -c $<
${ARCH}/sci.rel: $(sci_rel_OBJECTS)
	${make-rel}

## score

PGMS += $(ARCH)/score.rel

score_C_FILES = score/cpu_asm.c score/ispsh7750.c
score_rel_OBJECTS = $(score_C_FILES:score/%.c=$(ARCH)/%.$(OBJEXT))
EXTRA_DIST += $(score_C_FILES)

${ARCH}/%.$(OBJEXT): score/%.c
	${COMPILE} -o $@ -c $<
${ARCH}/score.rel: $(score_rel_OBJECTS)
	${make-rel}

## timer

PGMS += $(ARCH)/timer.rel

timer_C_FILES = timer/timer.c
timer_rel_OBJECTS = $(timer_C_FILES:timer/%.c=$(ARCH)/%.$(OBJEXT))
EXTRA_DIST += $(timer_C_FILES)

${ARCH}/%.$(OBJEXT): timer/%.c
	${COMPILE} -o $@ -c $<
${ARCH}/timer.rel: $(timer_rel_OBJECTS)
	${make-rel}

# --

all-local: $(PGMS) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGMS)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/sh/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/sh
	@: > $(PROJECT_INCLUDE)/sh/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sh/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkinstalldirs) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

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

include $(top_srcdir)/../../../../automake/local.am
