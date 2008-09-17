##
## $Id$
##

include $(top_srcdir)/automake/compile.am

EXEEXT =

project_lib_PROGRAMS =

project_lib_PROGRAMS += no-ext.rel
no_ext_rel_SOURCES = sapi/no-ext.c
no_ext_rel_CPPFLAGS = $(AM_CPPFLAGS)
no_ext_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-io.rel
no_io_rel_SOURCES = sapi/no-io.c
no_io_rel_CPPFLAGS = $(AM_CPPFLAGS)
no_io_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-barrier.rel
no_barrier_rel_SOURCES = rtems/no-barrier.c
no_barrier_rel_CPPFLAGS = $(AM_CPPFLAGS)
no_barrier_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-dpmem.rel
no_dpmem_rel_SOURCES = rtems/no-dpmem.c
no_dpmem_rel_CPPFLAGS = $(AM_CPPFLAGS)
no_dpmem_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-event.rel
no_event_rel_SOURCES = rtems/no-event.c
no_event_rel_CPPFLAGS = $(AM_CPPFLAGS)
no_event_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-msg.rel
no_msg_rel_SOURCES = rtems/no-msg.c
no_msg_rel_CPPFLAGS = $(AM_CPPFLAGS)
no_msg_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-mp.rel
no_mp_rel_SOURCES = rtems/no-mp.c
no_mp_rel_CPPFLAGS = $(AM_CPPFLAGS)
no_mp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-part.rel
no_part_rel_SOURCES = rtems/no-part.c
no_part_rel_CPPFLAGS = $(AM_CPPFLAGS)
no_part_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-region.rel
no_region_rel_SOURCES = rtems/no-region.c
no_region_rel_CPPFLAGS = $(AM_CPPFLAGS)
no_region_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-rtmon.rel
no_rtmon_rel_SOURCES = rtems/no-rtmon.c
no_rtmon_rel_CPPFLAGS = $(AM_CPPFLAGS)
no_rtmon_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-sem.rel
no_sem_rel_SOURCES = rtems/no-sem.c
no_sem_rel_CPPFLAGS = $(AM_CPPFLAGS)
no_sem_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-signal.rel
no_signal_rel_SOURCES = rtems/no-signal.c
no_signal_rel_CPPFLAGS = $(AM_CPPFLAGS)
no_signal_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-timer.rel
no_timer_rel_SOURCES = rtems/no-timer.c
no_timer_rel_CPPFLAGS = $(AM_CPPFLAGS)
no_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
