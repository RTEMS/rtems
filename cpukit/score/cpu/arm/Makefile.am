##
## $Id$
##
include $(top_srcdir)/automake/compile.am
include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/asm.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/cpu_asm.h \
    rtems/score/arm.h rtems/score/types.h

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c cpu_asm.S
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)
SUBDIRS = thumb
libscorecpu_a_LIBADD = ./thumb/libthumbisr_a-thumb_isr.o

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
