##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

EXTRA_DIST =

if HAS_ITRON
## include

include_HEADERS = include/itron.h

## itronsys

include_itronsysdir = $(includedir)/itronsys

include_itronsys_HEADERS = include/itronsys/eventflags.h \
    include/itronsys/fmempool.h include/itronsys/intr.h \
    include/itronsys/mbox.h include/itronsys/msgbuffer.h \
    include/itronsys/network.h include/itronsys/port.h \
    include/itronsys/semaphore.h include/itronsys/status.h \
    include/itronsys/sysmgmt.h include/itronsys/task.h \
    include/itronsys/time.h include/itronsys/types.h \
    include/itronsys/vmempool.h

EXTRA_DIST += include/itronsys/README

## rtems/itron
include_rtems_itrondir = $(includedir)/rtems/itron

include_rtems_itron_HEADERS = include/rtems/itron/config.h \
    include/rtems/itron/eventflags.h include/rtems/itron/fmempool.h \
    include/rtems/itron/intr.h include/rtems/itron/itronapi.h \
    include/rtems/itron/mbox.h include/rtems/itron/msgbuffer.h \
    include/rtems/itron/network.h include/rtems/itron/object.h \
    include/rtems/itron/port.h include/rtems/itron/semaphore.h \
    include/rtems/itron/sysmgmt.h include/rtems/itron/task.h \
    include/rtems/itron/time.h include/rtems/itron/vmempool.h

EXTRA_DIST += include/rtems/itron/README

## Inline

if INLINE
include_rtems_itron_HEADERS += inline/rtems/itron/eventflags.inl \
    inline/rtems/itron/fmempool.inl inline/rtems/itron/intr.inl \
    inline/rtems/itron/mbox.inl inline/rtems/itron/msgbuffer.inl \
    inline/rtems/itron/network.inl inline/rtems/itron/port.inl \
    inline/rtems/itron/semaphore.inl inline/rtems/itron/sysmgmt.inl \
    inline/rtems/itron/task.inl inline/rtems/itron/time.inl \
    inline/rtems/itron/vmempool.inl
else
include_rtems_itron_HEADERS += macros/rtems/itron/eventflags.inl \
    macros/rtems/itron/fmempool.inl macros/rtems/itron/intr.inl \
    macros/rtems/itron/mbox.inl macros/rtems/itron/msgbuffer.inl \
    macros/rtems/itron/network.inl macros/rtems/itron/port.inl \
    macros/rtems/itron/semaphore.inl macros/rtems/itron/sysmgmt.inl \
    macros/rtems/itron/task.inl macros/rtems/itron/time.inl \
    macros/rtems/itron/vmempool.inl
endif

## Sources

TASK_C_FILES = src/task.c src/cre_tsk.c src/del_tsk.c src/sta_tsk.c \
    src/ext_tsk.c src/exd_tsk.c src/ter_tsk.c src/dis_dsp.c src/ena_dsp.c \
    src/chg_pri.c src/rot_rdq.c src/rel_wai.c src/get_tid.c src/ref_tsk.c

TASKSYNC_C_FILES = src/sus_tsk.c src/rsm_tsk.c src/frsm_tsk.c src/slp_tsk.c \
    src/tslp_tsk.c src/wup_tsk.c src/can_wup.c

SEMAPHORE_C_FILES = src/itronsem.c src/cre_sem.c src/del_sem.c \
    src/preq_sem.c src/ref_sem.c src/sig_sem.c src/twai_sem.c src/wai_sem.c

EVENTFLAGS_C_FILES = src/eventflags.c

MAILBOX_C_FILES = src/mbox.c

MSGBUFFER_C_FILES = src/msgbuffer.c src/msgbuffertranslatereturncode.c \
    src/cre_mbf.c src/del_mbf.c src/prcv_mbf.c src/psnd_mbf.c src/rcv_mbf.c \
    src/ref_mbf.c src/snd_mbf.c src/trcv_mbf.c src/tsnd_mbf.c

RENDEZVOUS_C_FILES = src/port.c

INTERRUPT_C_FILES = src/itronintr.c

VARIABLE_MEMORY_POOL_C_FILES = src/vmempool.c

FIXED_MEMORY_POOL_C_FILES = src/fmempool.c

TIME_C_FILES = src/itrontime.c

CONFIGURATION_C_FILES = src/sysmgmt.c

NETWORK_C_FILES = src/network.c

C_FILES = $(TASK_C_FILES) $(TASKSYNC_C_FILES) $(SEMAPHORE_C_FILES) \
    $(EVENTFLAGS_C_FILES) $(MAILBOX_C_FILES) $(MSGBUFFER_C_FILES) \
    $(RENDEZVOUS_C_FILES) $(INTERRUPT_C_FILES) \
    $(VARIABLE_MEMORY_POOL_C_FILES) $(FIXED_MEMORY_POOL_C_FILES) \
    $(TIME_C_FILES)

AM_CPPFLAGS += -I$(top_builddir)
AM_CPPFLAGS += -D__RTEMS_INSIDE__

EXTRA_LIBRARIES = libitron.a
CLEANFILES = libitron.a
libitron_a_SOURCES = $(C_FILES)
libitron_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libitron_g.a
CLEANFILES += libitron_g.a
libitron_g_a_SOURCES = $(libitron_a_SOURCES)
libitron_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

all-local: $(PREINSTALL_FILES) libitron$(LIB_VARIANT).a
endif

UNUSED_C_FILES = src/cre_mbx.c src/del_mbx.c src/mboxtranslatereturncode.c \
    $(NETWORK_C_FILES) src/prcv_mbx.c src/rcv_mbx.c src/ref_mbx.c \
    src/snd_mbx.c $(CONFIGURATION_C_FILES) src/trcv_mbx.c

EXTRA_DIST += $(UNUSED_C_FILES) src/TODO

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)
	@: > $(PROJECT_INCLUDE)/$(dirstamp)

if HAS_ITRON
$(PROJECT_INCLUDE)/itron.h: include/itron.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itron.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itron.h

$(PROJECT_INCLUDE)/itronsys/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/itronsys
	@: > $(PROJECT_INCLUDE)/itronsys/$(dirstamp)

$(PROJECT_INCLUDE)/itronsys/eventflags.h: include/itronsys/eventflags.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/eventflags.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/eventflags.h

$(PROJECT_INCLUDE)/itronsys/fmempool.h: include/itronsys/fmempool.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/fmempool.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/fmempool.h

$(PROJECT_INCLUDE)/itronsys/intr.h: include/itronsys/intr.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/intr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/intr.h

$(PROJECT_INCLUDE)/itronsys/mbox.h: include/itronsys/mbox.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/mbox.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/mbox.h

$(PROJECT_INCLUDE)/itronsys/msgbuffer.h: include/itronsys/msgbuffer.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/msgbuffer.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/msgbuffer.h

$(PROJECT_INCLUDE)/itronsys/network.h: include/itronsys/network.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/network.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/network.h

$(PROJECT_INCLUDE)/itronsys/port.h: include/itronsys/port.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/port.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/port.h

$(PROJECT_INCLUDE)/itronsys/semaphore.h: include/itronsys/semaphore.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/semaphore.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/semaphore.h

$(PROJECT_INCLUDE)/itronsys/status.h: include/itronsys/status.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/status.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/status.h

$(PROJECT_INCLUDE)/itronsys/sysmgmt.h: include/itronsys/sysmgmt.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/sysmgmt.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/sysmgmt.h

$(PROJECT_INCLUDE)/itronsys/task.h: include/itronsys/task.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/task.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/task.h

$(PROJECT_INCLUDE)/itronsys/time.h: include/itronsys/time.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/time.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/time.h

$(PROJECT_INCLUDE)/itronsys/types.h: include/itronsys/types.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/types.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/types.h

$(PROJECT_INCLUDE)/itronsys/vmempool.h: include/itronsys/vmempool.h $(PROJECT_INCLUDE)/itronsys/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/itronsys/vmempool.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys/vmempool.h

$(PROJECT_INCLUDE)/rtems/itron/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/itron
	@: > $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/itron/config.h: include/rtems/itron/config.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/config.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/config.h

$(PROJECT_INCLUDE)/rtems/itron/eventflags.h: include/rtems/itron/eventflags.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/eventflags.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/eventflags.h

$(PROJECT_INCLUDE)/rtems/itron/fmempool.h: include/rtems/itron/fmempool.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/fmempool.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/fmempool.h

$(PROJECT_INCLUDE)/rtems/itron/intr.h: include/rtems/itron/intr.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/intr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/intr.h

$(PROJECT_INCLUDE)/rtems/itron/itronapi.h: include/rtems/itron/itronapi.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/itronapi.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/itronapi.h

$(PROJECT_INCLUDE)/rtems/itron/mbox.h: include/rtems/itron/mbox.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/mbox.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/mbox.h

$(PROJECT_INCLUDE)/rtems/itron/msgbuffer.h: include/rtems/itron/msgbuffer.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/msgbuffer.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/msgbuffer.h

$(PROJECT_INCLUDE)/rtems/itron/network.h: include/rtems/itron/network.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/network.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/network.h

$(PROJECT_INCLUDE)/rtems/itron/object.h: include/rtems/itron/object.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/object.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/object.h

$(PROJECT_INCLUDE)/rtems/itron/port.h: include/rtems/itron/port.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/port.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/port.h

$(PROJECT_INCLUDE)/rtems/itron/semaphore.h: include/rtems/itron/semaphore.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/semaphore.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/semaphore.h

$(PROJECT_INCLUDE)/rtems/itron/sysmgmt.h: include/rtems/itron/sysmgmt.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/sysmgmt.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/sysmgmt.h

$(PROJECT_INCLUDE)/rtems/itron/task.h: include/rtems/itron/task.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/task.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/task.h

$(PROJECT_INCLUDE)/rtems/itron/time.h: include/rtems/itron/time.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/time.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/time.h

$(PROJECT_INCLUDE)/rtems/itron/vmempool.h: include/rtems/itron/vmempool.h $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/vmempool.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/vmempool.h

if INLINE
$(PROJECT_INCLUDE)/rtems/itron/eventflags.inl: inline/rtems/itron/eventflags.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/eventflags.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/eventflags.inl

$(PROJECT_INCLUDE)/rtems/itron/fmempool.inl: inline/rtems/itron/fmempool.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/fmempool.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/fmempool.inl

$(PROJECT_INCLUDE)/rtems/itron/intr.inl: inline/rtems/itron/intr.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/intr.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/intr.inl

$(PROJECT_INCLUDE)/rtems/itron/mbox.inl: inline/rtems/itron/mbox.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/mbox.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/mbox.inl

$(PROJECT_INCLUDE)/rtems/itron/msgbuffer.inl: inline/rtems/itron/msgbuffer.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/msgbuffer.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/msgbuffer.inl

$(PROJECT_INCLUDE)/rtems/itron/network.inl: inline/rtems/itron/network.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/network.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/network.inl

$(PROJECT_INCLUDE)/rtems/itron/port.inl: inline/rtems/itron/port.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/port.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/port.inl

$(PROJECT_INCLUDE)/rtems/itron/semaphore.inl: inline/rtems/itron/semaphore.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/semaphore.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/semaphore.inl

$(PROJECT_INCLUDE)/rtems/itron/sysmgmt.inl: inline/rtems/itron/sysmgmt.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/sysmgmt.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/sysmgmt.inl

$(PROJECT_INCLUDE)/rtems/itron/task.inl: inline/rtems/itron/task.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/task.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/task.inl

$(PROJECT_INCLUDE)/rtems/itron/time.inl: inline/rtems/itron/time.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/time.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/time.inl

$(PROJECT_INCLUDE)/rtems/itron/vmempool.inl: inline/rtems/itron/vmempool.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/vmempool.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/vmempool.inl

else
$(PROJECT_INCLUDE)/rtems/itron/eventflags.inl: macros/rtems/itron/eventflags.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/eventflags.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/eventflags.inl

$(PROJECT_INCLUDE)/rtems/itron/fmempool.inl: macros/rtems/itron/fmempool.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/fmempool.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/fmempool.inl

$(PROJECT_INCLUDE)/rtems/itron/intr.inl: macros/rtems/itron/intr.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/intr.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/intr.inl

$(PROJECT_INCLUDE)/rtems/itron/mbox.inl: macros/rtems/itron/mbox.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/mbox.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/mbox.inl

$(PROJECT_INCLUDE)/rtems/itron/msgbuffer.inl: macros/rtems/itron/msgbuffer.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/msgbuffer.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/msgbuffer.inl

$(PROJECT_INCLUDE)/rtems/itron/network.inl: macros/rtems/itron/network.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/network.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/network.inl

$(PROJECT_INCLUDE)/rtems/itron/port.inl: macros/rtems/itron/port.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/port.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/port.inl

$(PROJECT_INCLUDE)/rtems/itron/semaphore.inl: macros/rtems/itron/semaphore.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/semaphore.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/semaphore.inl

$(PROJECT_INCLUDE)/rtems/itron/sysmgmt.inl: macros/rtems/itron/sysmgmt.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/sysmgmt.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/sysmgmt.inl

$(PROJECT_INCLUDE)/rtems/itron/task.inl: macros/rtems/itron/task.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/task.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/task.inl

$(PROJECT_INCLUDE)/rtems/itron/time.inl: macros/rtems/itron/time.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/time.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/time.inl

$(PROJECT_INCLUDE)/rtems/itron/vmempool.inl: macros/rtems/itron/vmempool.inl $(PROJECT_INCLUDE)/rtems/itron/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/itron/vmempool.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron/vmempool.inl
endif
endif

include $(top_srcdir)/automake/local.am
