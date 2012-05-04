include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/asm.h

include_rtems_bfindir = $(includedir)/rtems/bfin
include_rtems_bfin_HEADERS = rtems/bfin/bfin.h
include_rtems_bfin_HEADERS += rtems/bfin/bf533.h
include_rtems_bfin_HEADERS += rtems/bfin/bf52x.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h
include_rtems_score_HEADERS += rtems/score/bfin.h
include_rtems_score_HEADERS += rtems/score/cpu_asm.h
include_rtems_score_HEADERS += rtems/score/types.h

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c cpu_asm.S
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
