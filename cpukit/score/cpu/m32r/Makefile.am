##
## $Id$
##

include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/asm.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/m32r.h \
    rtems/score/cpu_asm.h rtems/score/types.h

noinst_LIBRARIES = libscorecpu.a
# for now just use the stubs
libscorecpu_a_SOURCES = ../no_cpu/cpu.c ../no_cpu/cpu_asm.c
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
