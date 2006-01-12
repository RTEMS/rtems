##
## $Id$
##

include $(top_srcdir)/automake/compile.am

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/asm.h

include_rtems_tic4xdir = $(includedir)/rtems/tic4x
include_rtems_tic4x_HEADERS = rtems/tic4x/c4xio.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/c4x.h \
    rtems/score/types.h rtems/score/cpu_asm.h

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c irq.c cpu_asm.S
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
