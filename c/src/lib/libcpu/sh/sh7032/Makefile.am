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

include_sh_HEADERS = include/sci.h include/sh7_pfc.h include/sh7_sci.h
include_rtems_score_HEADERS = include/ispsh7032.h include/iosh7032.h

# clock

PGMS = $(ARCH)/clock.rel

clock_C_FILES = clock/ckinit.c
clock_rel_OBJECTS = $(clock_C_FILES:clock/%.c=$(ARCH)/%.$(OBJEXT))
EXTRA_DIST = $(clock_C_FILES)

${ARCH}/%.$(OBJEXT): clock/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
${ARCH}/clock.rel: $(clock_rel_OBJECTS)
	${make-rel}

# delay

PGMS += $(ARCH)/delay.rel

delay_C_FILES = delay/delay.c
delay_rel_OBJECTS = $(delay_C_FILES:delay/%.c=$(ARCH)/%.$(OBJEXT))
EXTRA_DIST += $(delay_C_FILES)

${ARCH}/%.$(OBJEXT): delay/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
${ARCH}/delay.rel: $(delay_rel_OBJECTS)
	${make-rel}

# sci

PGMS += $(ARCH)/sci.rel

sci_C_FILES = sci/sci.c
sci_rel_OBJECTS = $(sci_C_FILES:sci/%.c=$(ARCH)/%.$(OBJEXT))
EXTRA_DIST += $(sci_C_FILES)

${ARCH}/%.$(OBJEXT): sci/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
${ARCH}/sci.rel: $(sci_rel_OBJECTS)
	${make-rel}

# score

PGMS += $(ARCH)/score.rel

score_C_FILES = score/cpu_asm.c score/ispsh7032.c
score_rel_OBJECTS = $(score_C_FILES:score/%.c=$(ARCH)/%.$(OBJEXT))
EXTRA_DIST += $(score_C_FILES)

${ARCH}/%.$(OBJEXT): score/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
${ARCH}/score.rel: $(score_rel_OBJECTS)
	${make-rel}

## timer

PGMS += $(ARCH)/timer.rel

timer_C_FILES = timer/timer.c
timer_rel_OBJECTS = $(timer_C_FILES:timer/%.c=$(ARCH)/%.$(OBJEXT))
EXTRA_DIST += $(timer_C_FILES)

${ARCH}/%.$(OBJEXT): timer/%.c $(ARCH)/$(dirstamp)
	${COMPILE} -o $@ -c $<
${ARCH}/timer.rel: $(timer_rel_OBJECTS)
	${make-rel}

all-local: $(PGMS) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGMS)

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/sh/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/sh
	@: > $(PROJECT_INCLUDE)/sh/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sh/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/sh/sci.h: include/sci.h $(PROJECT_INCLUDE)/sh/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sh/sci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sh/sci.h

$(PROJECT_INCLUDE)/sh/sh7_pfc.h: include/sh7_pfc.h $(PROJECT_INCLUDE)/sh/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sh/sh7_pfc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sh/sh7_pfc.h

$(PROJECT_INCLUDE)/sh/sh7_sci.h: include/sh7_sci.h $(PROJECT_INCLUDE)/sh/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sh/sh7_sci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sh/sh7_sci.h

$(PROJECT_INCLUDE)/rtems/score/ispsh7032.h: include/ispsh7032.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/ispsh7032.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/ispsh7032.h

$(PROJECT_INCLUDE)/rtems/score/iosh7032.h: include/iosh7032.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/iosh7032.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/iosh7032.h

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/local.am
