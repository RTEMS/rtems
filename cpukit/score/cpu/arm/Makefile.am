include $(top_srcdir)/automake/compile.am
include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/asm.h

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/cpu.h
include_rtems_score_HEADERS += rtems/score/cpu_asm.h
include_rtems_score_HEADERS += rtems/score/arm.h
include_rtems_score_HEADERS += rtems/score/armv7m.h
include_rtems_score_HEADERS += rtems/score/types.h

noinst_LIBRARIES = libscorecpu.a
libscorecpu_a_CPPFLAGS = $(AM_CPPFLAGS)
libscorecpu_a_SOURCES =
libscorecpu_a_SOURCES += cpu.c
libscorecpu_a_SOURCES += cpu_asm.S
libscorecpu_a_SOURCES += arm_exc_abort.S
libscorecpu_a_SOURCES += arm_exc_interrupt.S
libscorecpu_a_SOURCES += arm_exc_handler_low.S
libscorecpu_a_SOURCES += arm_exc_handler_high.c
libscorecpu_a_SOURCES += armv7m-context-initialize.c
libscorecpu_a_SOURCES += armv7m-context-restore.c
libscorecpu_a_SOURCES += armv7m-context-switch.c
libscorecpu_a_SOURCES += armv7m-exception-handler-get.c
libscorecpu_a_SOURCES += armv7m-exception-handler-set.c
libscorecpu_a_SOURCES += armv7m-exception-priority-get.c
libscorecpu_a_SOURCES += armv7m-exception-priority-handler.c
libscorecpu_a_SOURCES += armv7m-exception-priority-set.c
libscorecpu_a_SOURCES += armv7m-initialize.c
libscorecpu_a_SOURCES += armv7m-isr-dispatch.c
libscorecpu_a_SOURCES += armv7m-isr-enter-leave.c
libscorecpu_a_SOURCES += armv7m-isr-level-get.c
libscorecpu_a_SOURCES += armv7m-isr-level-set.c
libscorecpu_a_SOURCES += armv7m-isr-vector-install.c
libscorecpu_a_SOURCES += armv7m-multitasking-start-stop.c

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
