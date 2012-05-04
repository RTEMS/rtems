include $(top_srcdir)/automake/compile.am

CLEANFILES =
DISTCLEANFILES =

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = rtems/asm.h

include_rtems_scoredir = $(includedir)/rtems/score

include_rtems_score_HEADERS =
include_rtems_score_HEADERS += rtems/score/cpu.h
include_rtems_score_HEADERS += rtems/score/nios2.h
include_rtems_score_HEADERS += rtems/score/nios2-utility.h
include_rtems_score_HEADERS += rtems/score/cpu_asm.h
include_rtems_score_HEADERS += rtems/score/types.h

noinst_LIBRARIES = libscorecpu.a

libscorecpu_a_SOURCES =
libscorecpu_a_SOURCES += irq.c
libscorecpu_a_SOURCES += nios2-context-initialize.c
libscorecpu_a_SOURCES += nios2-context-switch.S
libscorecpu_a_SOURCES += nios2-eic-il-low-level.S
libscorecpu_a_SOURCES += nios2-eic-rsie-low-level.S
libscorecpu_a_SOURCES += nios2-fatal-halt.c
libscorecpu_a_SOURCES += nios2-iic-low-level.S
libscorecpu_a_SOURCES += nios2-initialize.c
libscorecpu_a_SOURCES += nios2-initialize-vectors.c
libscorecpu_a_SOURCES += nios2-isr-get-level.c
libscorecpu_a_SOURCES += nios2-isr-install-raw-handler.c
libscorecpu_a_SOURCES += nios2-isr-install-vector.c
libscorecpu_a_SOURCES += nios2-isr-is-in-progress.c
libscorecpu_a_SOURCES += nios2-isr-set-level.c
libscorecpu_a_SOURCES += nios2-mpu-add-region.c
libscorecpu_a_SOURCES += nios2-mpu-configuration.c
libscorecpu_a_SOURCES += nios2-mpu-descriptor.c
libscorecpu_a_SOURCES += nios2-mpu-disable-protected.c
libscorecpu_a_SOURCES += nios2-mpu-reset.c
libscorecpu_a_SOURCES += nios2-thread-dispatch-disabled.c

libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)

all-local: $(PREINSTALL_FILES)

include $(srcdir)/preinstall.am

include $(top_srcdir)/automake/local.am
