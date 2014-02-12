##
## $Id: Makefile.am,v 1.35 2006/01/12 09:57:43 ralf Exp $
##

include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/asm.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/moxie.h \
    rtems/score/types.h
include_rtems_score_HEADERS += rtems/score/cpuatomic.h

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c moxie-exception-frame-print.c cpu_asm.S
libscorecpu_a_SOURCES += ../no_cpu/cpucounterread.c
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
