##
## $Id$
##

include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

EXTRA_PROGRAMS =
CLEANFILES =
project_lib_DATA =

EXTRA_PROGRAMS += no-ext.rel
CLEANFILES += no-ext.rel
no_ext_rel_SOURCES = sapi/no-ext.c
no_ext_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_ext_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += no-ext_g.rel
CLEANFILES += no-ext_g.rel
no_ext_g_rel_SOURCES = $(no_ext_rel_SOURCES)
no_ext_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
no_ext_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += no-ext$(LIB_VARIANT).rel

EXTRA_PROGRAMS += no-io.rel
CLEANFILES += no-io.rel
no_io_rel_SOURCES = sapi/no-io.c
no_io_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_io_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += no-io_g.rel
CLEANFILES += no-io_g.rel
no_io_g_rel_SOURCES = $(no_io_rel_SOURCES)
no_io_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
no_io_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += no-io$(LIB_VARIANT).rel

EXTRA_PROGRAMS += no-dpmem.rel
CLEANFILES += no-dpmem.rel
no_dpmem_rel_SOURCES = rtems/no-dpmem.c
no_dpmem_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_dpmem_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += no-dpmem_g.rel
CLEANFILES += no-dpmem_g.rel
no_dpmem_g_rel_SOURCES = $(no_dpmem_rel_SOURCES)
no_dpmem_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
no_dpmem_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += no-dpmem$(LIB_VARIANT).rel

EXTRA_PROGRAMS += no-event.rel
CLEANFILES += no-event.rel
no_event_rel_SOURCES = rtems/no-event.c
no_event_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_event_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += no-event_g.rel
CLEANFILES += no-event_g.rel
no_event_g_rel_SOURCES = $(no_event_rel_SOURCES)
no_event_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
no_event_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += no-event$(LIB_VARIANT).rel

EXTRA_PROGRAMS += no-msg.rel
CLEANFILES += no-msg.rel
no_msg_rel_SOURCES = rtems/no-msg.c
no_msg_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_msg_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += no-msg_g.rel
CLEANFILES += no-msg_g.rel
no_msg_g_rel_SOURCES = $(no_msg_rel_SOURCES)
no_msg_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
no_msg_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += no-msg$(LIB_VARIANT).rel

EXTRA_PROGRAMS += no-mp.rel
CLEANFILES += no-mp.rel
no_mp_rel_SOURCES = rtems/no-mp.c
no_mp_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_mp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += no-mp_g.rel
CLEANFILES += no-mp_g.rel
no_mp_g_rel_SOURCES = $(no_mp_rel_SOURCES)
no_mp_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
no_mp_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += no-mp$(LIB_VARIANT).rel

EXTRA_PROGRAMS += no-part.rel
CLEANFILES += no-part.rel
no_part_rel_SOURCES = rtems/no-part.c
no_part_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_part_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += no-part_g.rel
CLEANFILES += no-part_g.rel
no_part_g_rel_SOURCES = $(no_part_rel_SOURCES)
no_part_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
no_part_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += no-part$(LIB_VARIANT).rel

EXTRA_PROGRAMS += no-region.rel
CLEANFILES += no-region.rel
no_region_rel_SOURCES = rtems/no-region.c
no_region_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_region_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += no-region_g.rel
CLEANFILES += no-region_g.rel
no_region_g_rel_SOURCES = $(no_region_rel_SOURCES)
no_region_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
no_region_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += no-region$(LIB_VARIANT).rel

EXTRA_PROGRAMS += no-rtmon.rel
CLEANFILES += no-rtmon.rel
no_rtmon_rel_SOURCES = rtems/no-rtmon.c
no_rtmon_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_rtmon_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += no-rtmon_g.rel
CLEANFILES += no-rtmon_g.rel
no_rtmon_g_rel_SOURCES = $(no_rtmon_rel_SOURCES)
no_rtmon_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
no_rtmon_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += no-rtmon$(LIB_VARIANT).rel

EXTRA_PROGRAMS += no-sem.rel
CLEANFILES += no-sem.rel
no_sem_rel_SOURCES = rtems/no-sem.c
no_sem_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_sem_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += no-sem_g.rel
CLEANFILES += no-sem_g.rel
no_sem_g_rel_SOURCES = $(no_sem_rel_SOURCES)
no_sem_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
no_sem_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += no-sem$(LIB_VARIANT).rel

EXTRA_PROGRAMS += no-signal.rel
CLEANFILES += no-signal.rel
no_signal_rel_SOURCES = rtems/no-signal.c
no_signal_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_signal_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += no-signal_g.rel
CLEANFILES += no-signal_g.rel
no_signal_g_rel_SOURCES = $(no_signal_rel_SOURCES)
no_signal_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
no_signal_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += no-signal$(LIB_VARIANT).rel

EXTRA_PROGRAMS += no-timer.rel
CLEANFILES += no-timer.rel
no_timer_rel_SOURCES = rtems/no-timer.c
no_timer_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)
no_timer_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

EXTRA_PROGRAMS += no-timer_g.rel
CLEANFILES += no-timer_g.rel
no_timer_g_rel_SOURCES = $(no_timer_rel_SOURCES)
no_timer_g_rel_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)
no_timer_g_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

project_lib_DATA += no-timer$(LIB_VARIANT).rel

all-local: $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

$(PROJECT_LIB)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_LIB)
	@: > $(PROJECT_LIB)/$(dirstamp)
TMPINSTALL_FILES += $(PROJECT_LIB)/$(dirstamp)

$(PROJECT_LIB)/no-ext$(LIB_VARIANT).rel: no-ext$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-ext$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-ext$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-io$(LIB_VARIANT).rel: no-io$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-io$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-io$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-dpmem$(LIB_VARIANT).rel: no-dpmem$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-dpmem$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-dpmem$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-event$(LIB_VARIANT).rel: no-event$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-event$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-event$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-msg$(LIB_VARIANT).rel: no-msg$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-msg$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-msg$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-mp$(LIB_VARIANT).rel: no-mp$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-mp$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-mp$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-part$(LIB_VARIANT).rel: no-part$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-part$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-part$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-region$(LIB_VARIANT).rel: no-region$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-region$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-region$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-rtmon$(LIB_VARIANT).rel: no-rtmon$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-rtmon$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-rtmon$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-sem$(LIB_VARIANT).rel: no-sem$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-sem$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-sem$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-signal$(LIB_VARIANT).rel: no-signal$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-signal$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-signal$(LIB_VARIANT).rel

$(PROJECT_LIB)/no-timer$(LIB_VARIANT).rel: no-timer$(LIB_VARIANT).rel $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/no-timer$(LIB_VARIANT).rel
TMPINSTALL_FILES += $(PROJECT_LIB)/no-timer$(LIB_VARIANT).rel

CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/automake/local.am
