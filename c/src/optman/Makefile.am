##
## $Id$
##

include $(top_srcdir)/automake/compile.am

EXEEXT =

project_lib_PROGRAMS =

project_lib_PROGRAMS += no-ext.rel
no_ext_rel_SOURCES = sapi/no-ext.c
no_ext_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_ext_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-io.rel
no_io_rel_SOURCES = sapi/no-io.c
no_io_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_io_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-dpmem.rel
no_dpmem_rel_SOURCES = rtems/no-dpmem.c
no_dpmem_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_dpmem_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-event.rel
no_event_rel_SOURCES = rtems/no-event.c
no_event_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_event_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-msg.rel
no_msg_rel_SOURCES = rtems/no-msg.c
no_msg_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_msg_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-mp.rel
no_mp_rel_SOURCES = rtems/no-mp.c
no_mp_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_mp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-part.rel
no_part_rel_SOURCES = rtems/no-part.c
no_part_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_part_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-region.rel
no_region_rel_SOURCES = rtems/no-region.c
no_region_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_region_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-rtmon.rel
no_rtmon_rel_SOURCES = rtems/no-rtmon.c
no_rtmon_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_rtmon_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-sem.rel
no_sem_rel_SOURCES = rtems/no-sem.c
no_sem_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_sem_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-signal.rel
no_signal_rel_SOURCES = rtems/no-signal.c
no_signal_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_signal_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_PROGRAMS += no-timer.rel
no_timer_rel_SOURCES = rtems/no-timer.c
no_timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

all-local: $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

$(PROJECT_LIB)/no-ext.rel: no-ext.rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_LIB)/no-ext.rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-ext.rel

$(PROJECT_LIB)/no-io.rel: no-io.rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_LIB)/no-io.rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-io.rel

$(PROJECT_LIB)/no-dpmem.rel: no-dpmem.rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_LIB)/no-dpmem.rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-dpmem.rel

$(PROJECT_LIB)/no-event.rel: no-event.rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_LIB)/no-event.rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-event.rel

$(PROJECT_LIB)/no-msg.rel: no-msg.rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_LIB)/no-msg.rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-msg.rel

$(PROJECT_LIB)/no-mp.rel: no-mp.rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_LIB)/no-mp.rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-mp.rel

$(PROJECT_LIB)/no-part.rel: no-part.rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_LIB)/no-part.rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-part.rel

$(PROJECT_LIB)/no-region.rel: no-region.rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_LIB)/no-region.rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-region.rel

$(PROJECT_LIB)/no-rtmon.rel: no-rtmon.rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_LIB)/no-rtmon.rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-rtmon.rel

$(PROJECT_LIB)/no-sem.rel: no-sem.rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_LIB)/no-sem.rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-sem.rel

$(PROJECT_LIB)/no-signal.rel: no-signal.rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_LIB)/no-signal.rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-signal.rel

$(PROJECT_LIB)/no-timer.rel: no-timer.rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_PROGRAM) $< $(PROJECT_LIB)/no-timer.rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-timer.rel

CLEANFILES = $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
