##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

CLEANFILES =
EXTRA_PROGRAMS =
noinst_DATA =

if sh7032
# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts
include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score

include_sh_HEADERS = include/sci.h include/sh7_pfc.h include/sh7_sci.h
include_rtems_score_HEADERS = include/ispsh7032.h include/iosh7032.h

# clock
EXTRA_PROGRAMS += clock.rel
CLEANFILES += clock.rel
clock_rel_SOURCES = clock/ckinit.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += clock_g.rel
CLEANFILES += clock_g.rel
clock_g_rel_SOURCES = $(clock_rel_SOURCES)
clock_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
clock_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += clock$(LIB_VARIANT).rel

# delay
EXTRA_PROGRAMS += delay.rel
CLEANFILES += delay.rel
delay_rel_SOURCES = delay/delay.c
delay_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
delay_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += delay_g.rel
CLEANFILES += delay_g.rel
delay_g_rel_SOURCES = $(delay_rel_SOURCES)
delay_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
delay_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += delay$(LIB_VARIANT).rel

# sci
EXTRA_PROGRAMS += sci.rel
CLEANFILES += sci.rel
sci_rel_SOURCES = sci/sci.c
sci_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
sci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += sci_g.rel
CLEANFILES += sci_g.rel
sci_g_rel_SOURCES = $(sci_rel_SOURCES)
sci_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
sci_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += sci$(LIB_VARIANT).rel

# score
EXTRA_PROGRAMS += score.rel
CLEANFILES += score.rel
score_rel_SOURCES =score/cpu_asm.c score/ispsh7032.c
score_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
score_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += score_g.rel
CLEANFILES += score_g.rel
score_g_rel_SOURCES = $(score_rel_SOURCES)
score_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
score_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += score$(LIB_VARIANT).rel

## timer
EXTRA_PROGRAMS += timer.rel
CLEANFILES += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += timer_g.rel
CLEANFILES += timer_g.rel
timer_g_rel_SOURCES = $(timer_rel_SOURCES)
timer_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
timer_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_DATA += timer$(LIB_VARIANT).rel
endif

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if sh7032
$(PROJECT_INCLUDE)/sh/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/sh
	@: > $(PROJECT_INCLUDE)/sh/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/sh/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

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
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
