include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/asm.h

#include_rtems_sparc64dir = $(includedir)/rtems/sparc64
#include_rtems_sparc64_HEADERS = 

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/sparc64.h
include_rtems_score_HEADERS += rtems/score/cpu.h
include_rtems_score_HEADERS += rtems/score/types.h

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = context.S cpu.c
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
