include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/asm.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h
include_rtems_score_HEADERS += rtems/score/types.h
include_rtems_score_HEADERS += rtems/score/sh.h
include_rtems_score_HEADERS += rtems/score/sh_io.h

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c context.c
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
