##
## $Id$
##

include $(top_srcdir)/automake/compile.am

CLEANFILES =
DISTCLEANFILES =

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/asm.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h rtems/score/nios2.h \
    rtems/score/cpu_asm.h rtems/score/types.h

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_SOURCES = cpu.c irq.c cpu_asm.S
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

all-local: $(PREINSTALL_FILES)

include $(srcdir)/preinstall.am

include $(top_srcdir)/automake/local.am
