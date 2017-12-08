ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

noinst_PROGRAMS =

noinst_PROGRAMS += cache.rel
cache_rel_SOURCES = ../shared/src/cache_manager.c
cache_rel_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/cache
cache_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if sh7045
# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts
include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score

include_sh_HEADERS = sh7045/include/io_types.h
include_sh_HEADERS += sh7045/include/sci.h
include_sh_HEADERS += sh7045/include/sh7_pfc.h
include_sh_HEADERS += sh7045/include/sh7_sci.h
include_sh_HEADERS += sh7045/include/sci_termios.h
include_rtems_score_HEADERS = sh7045/include/ispsh7045.h
include_rtems_score_HEADERS += sh7045/include/iosh7045.h

## sh7045/clock
noinst_PROGRAMS += sh7045/clock.rel
sh7045_clock_rel_SOURCES = sh7045/clock/ckinit.c
sh7045_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
sh7045_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## sh7045/sci
noinst_PROGRAMS += sh7045/sci.rel
sh7045_sci_rel_SOURCES = sh7045/sci/sci.c sh7045/sci/sci_termios.c
sh7045_sci_rel_CPPFLAGS = $(AM_CPPFLAGS)
sh7045_sci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## sh7045/score
noinst_PROGRAMS += sh7045/score.rel
sh7045_score_rel_SOURCES = sh7045/score/cpu_asm.c sh7045/score/ispsh7045.c
sh7045_score_rel_CPPFLAGS = $(AM_CPPFLAGS)
sh7045_score_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## sh7045/timer
noinst_PROGRAMS += sh7045/timer.rel
sh7045_timer_rel_SOURCES = sh7045/timer/timer.c
sh7045_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
sh7045_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if sh7750
# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts
include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score

include_sh_HEADERS = sh7750/include/sh/sh4uart.h
include_rtems_score_HEADERS = sh7750/include/rtems/score/sh7750_regs.h
include_rtems_score_HEADERS += sh7750/include/rtems/score/ipl.h
include_rtems_score_HEADERS += sh7750/include/rtems/score/iosh7750.h
include_rtems_score_HEADERS += sh7750/include/rtems/score/ispsh7750.h
include_rtems_score_HEADERS += sh7750/include/rtems/score/sh4_regs.h

## sh7750/clock
noinst_PROGRAMS += sh7750/clock.rel
sh7750_clock_rel_SOURCES = sh7750/clock/ckinit.c
sh7750_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
sh7750_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## sh7750/sci
noinst_PROGRAMS += sh7750/sci.rel
sh7750_sci_rel_SOURCES = sh7750/sci/sh4uart.c
sh7750_sci_rel_CPPFLAGS = $(AM_CPPFLAGS)
sh7750_sci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## sh7750/score
noinst_PROGRAMS += sh7750/score.rel
sh7750_score_rel_SOURCES = sh7750/score/cpu_asm.c sh7750/score/ispsh7750.c
sh7750_score_rel_CPPFLAGS = $(AM_CPPFLAGS)
sh7750_score_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## sh7750/timer
noinst_PROGRAMS += sh7750/timer.rel
sh7750_timer_rel_SOURCES = sh7750/timer/timer.c
sh7750_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
sh7750_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/local.am
