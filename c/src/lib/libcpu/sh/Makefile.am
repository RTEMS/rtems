ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

noinst_PROGRAMS =

if sh7032
# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts
include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score

include_sh_HEADERS = sh7032/include/sci.h sh7032/include/sh7_pfc.h sh7032/include/sh7_sci.h
include_rtems_score_HEADERS = sh7032/include/ispsh7032.h sh7032/include/iosh7032.h

## sh7032/clock
noinst_PROGRAMS += sh7032/clock.rel
sh7032_clock_rel_SOURCES = sh7032/clock/ckinit.c
sh7032_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
sh7032_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## sh7032/delay
noinst_PROGRAMS += sh7032/delay.rel
sh7032_delay_rel_SOURCES = sh7032/delay/delay.c
sh7032_delay_rel_CPPFLAGS = $(AM_CPPFLAGS)
sh7032_delay_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## sh7032/sci
noinst_PROGRAMS += sh7032/sci.rel
sh7032_sci_rel_SOURCES = sh7032/sci/sci.c
sh7032_sci_rel_CPPFLAGS = $(AM_CPPFLAGS)
sh7032_sci_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

## sh7032/score
noinst_PROGRAMS += sh7032/score.rel
sh7032_score_rel_SOURCES = sh7032/score/cpu_asm.c sh7032/score/ispsh7032.c
sh7032_score_rel_CPPFLAGS = $(AM_CPPFLAGS)
sh7032_score_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

### sh7032/timer
noinst_PROGRAMS += sh7032/timer.rel
sh7032_timer_rel_SOURCES = sh7032/timer/timer.c
sh7032_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
sh7032_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

if sh7045
# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts
include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score

include_sh_HEADERS = sh7045/include/io_types.h sh7045/include/sci.h sh7045/include/sh7_pfc.h \
    sh7045/include/sh7_sci.h sh7045/include/sci_termios.h
include_rtems_score_HEADERS = sh7045/include/ispsh7045.h sh7045/include/iosh7045.h

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
include_rtems_score_HEADERS = sh7750/include/rtems/score/sh7750_regs.h \
    sh7750/include/rtems/score/ipl.h sh7750/include/rtems/score/iosh7750.h \
    sh7750/include/rtems/score/ispsh7750.h sh7750/include/rtems/score/sh4_regs.h

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

if shgdb
## sh7045/score
noinst_PROGRAMS += shgdb/score.rel
shgdb_score_rel_SOURCES = shgdb/score/cpu_asm.c shgdb/score/ispshgdb.c
shgdb_score_rel_CPPFLAGS = $(AM_CPPFLAGS)
shgdb_score_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../automake/local.am
