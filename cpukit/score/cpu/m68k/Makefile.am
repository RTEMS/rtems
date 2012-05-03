include $(top_srcdir)/automake/compile.am

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/asm.h

include_rtems_m68kdir = $(includedir)/rtems/m68k
include_rtems_m68k_HEADERS = rtems/m68k/m68302.h
include_rtems_m68k_HEADERS += rtems/m68k/m68360.h
include_rtems_m68k_HEADERS += rtems/m68k/qsm.h
include_rtems_m68k_HEADERS += rtems/m68k/sim.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h
include_rtems_score_HEADERS += rtems/score/m68k.h
include_rtems_score_HEADERS += rtems/score/types.h

libscorecpu_a_SOURCES = cpu.c cpu_asm.S

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
