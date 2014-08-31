include $(top_srcdir)/automake/compile.am

CLEANFILES =
DISTCLEANFILES =

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = rtems/asm.h

include_rtems_scoredir = $(includedir)/rtems/score

include_rtems_score_HEADERS =
include_rtems_score_HEADERS += rtems/score/cpu.h
include_rtems_score_HEADERS += rtems/score/cpu_asm.h
include_rtems_score_HEADERS += rtems/score/types.h
include_rtems_score_HEADERS += rtems/score/or1k.h
include_rtems_score_HEADERS += rtems/score/or1k-utility.h



noinst_LIBRARIES = libscorecpu.a

libscorecpu_a_SOURCES =
libscorecpu_a_SOURCES += cpu.c
libscorecpu_a_SOURCES += or1k-context-switch.S
libscorecpu_a_SOURCES += or1k-context-initialize.c
libscorecpu_a_SOURCES += or1k-context-validate.S
libscorecpu_a_SOURCES += or1k-context-volatile-clobber.S
libscorecpu_a_SOURCES += or1k-exception-default.c
libscorecpu_a_SOURCES += or1k-exception-frame-print.c
libscorecpu_a_SOURCES += or1k-exception-handler-low.S

libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

all-local: $(PREINSTALL_FILES)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
