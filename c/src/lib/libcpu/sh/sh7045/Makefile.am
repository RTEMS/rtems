##
## $Id$
##

include $(top_srcdir)/../../../automake/compile.am

CLEANFILES =
noinst_PROGRAMS =

if sh7045
# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts
include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score

include_sh_HEADERS = include/io_types.h include/sci.h include/sh7_pfc.h \
    include/sh7_sci.h include/sci_termios.h
include_rtems_score_HEADERS = include/ispsh7045.h include/iosh7045.h

# clock
noinst_PROGRAMS += clock.rel
clock_rel_SOURCES = clock/ckinit.c
clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# sci
noinst_PROGRAMS += sci.rel
sci_rel_SOURCES = sci/sci.c sci/sci_termios.c
sci_rel_CPPFLAGS = $(AM_CPPFLAGS)
sci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# score
noinst_PROGRAMS += score.rel
score_rel_SOURCES = score/cpu_asm.c score/ispsh7045.c
score_rel_CPPFLAGS = $(AM_CPPFLAGS)
score_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## timer
noinst_PROGRAMS += timer.rel
timer_rel_SOURCES = timer/timer.c
timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

all-local: $(PREINSTALL_FILES)

PREINSTALL_DIRS =
PREINSTALL_FILES =

if sh7045
$(PROJECT_INCLUDE)/sh/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/sh
	@: > $(PROJECT_INCLUDE)/sh/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/sh/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
PREINSTALL_DIRS += $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/sh/io_types.h: include/io_types.h $(PROJECT_INCLUDE)/sh/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sh/io_types.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sh/io_types.h

$(PROJECT_INCLUDE)/sh/sci.h: include/sci.h $(PROJECT_INCLUDE)/sh/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sh/sci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sh/sci.h

$(PROJECT_INCLUDE)/sh/sh7_pfc.h: include/sh7_pfc.h $(PROJECT_INCLUDE)/sh/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sh/sh7_pfc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sh/sh7_pfc.h

$(PROJECT_INCLUDE)/sh/sh7_sci.h: include/sh7_sci.h $(PROJECT_INCLUDE)/sh/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sh/sh7_sci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sh/sh7_sci.h

$(PROJECT_INCLUDE)/sh/sci_termios.h: include/sci_termios.h $(PROJECT_INCLUDE)/sh/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/sh/sci_termios.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/sh/sci_termios.h

$(PROJECT_INCLUDE)/rtems/score/ispsh7045.h: include/ispsh7045.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/ispsh7045.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/ispsh7045.h

$(PROJECT_INCLUDE)/rtems/score/iosh7045.h: include/iosh7045.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/iosh7045.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/iosh7045.h
endif

CLEANFILES += $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
