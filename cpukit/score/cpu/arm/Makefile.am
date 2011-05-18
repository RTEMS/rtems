##
## $Id$
##
include $(top_srcdir)/automake/compile.am
include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/asm.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h
include_rtems_score_HEADERS += rtems/score/cpu_asm.h
include_rtems_score_HEADERS += rtems/score/arm.h
include_rtems_score_HEADERS += rtems/score/types.h

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)
libscorecpu_a_SOURCES = cpu.c \
	cpu_asm.S \
	arm_exc_abort.S \
	arm_exc_interrupt.S \
	arm_exc_handler_low.S \
	arm_exc_handler_high.c

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
