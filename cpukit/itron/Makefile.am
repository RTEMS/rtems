##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

EXTRA_DIST =

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/itronsys:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/itron:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@
$(PROJECT_INCLUDE)/%.inl: $(INLINEdir)/%.inl
	$(INSTALL_DATA) $< $@

if HAS_ITRON
## include

noinst_HEADERS =
noinst_HEADERS += src/config.h

include_HEADERS = include/itron.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)
PREINSTALL_FILES += $(include_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

## itronsys

include_itronsysdir = $(includedir)/itronsys

include_itronsys_HEADERS = include/itronsys/eventflags.h include/itronsys/fmempool.h \
    include/itronsys/intr.h include/itronsys/mbox.h include/itronsys/msgbuffer.h include/itronsys/network.h \
    include/itronsys/port.h include/itronsys/semaphore.h include/itronsys/status.h include/itronsys/sysmgmt.h \
    include/itronsys/task.h include/itronsys/time.h include/itronsys/types.h include/itronsys/vmempool.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/itronsys \
    $(include_itronsys_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

EXTRA_DIST += include/itronsys/README

## rtems/itron
include_rtems_itrondir = $(includedir)/rtems/itron

include_rtems_itron_HEADERS = include/rtems/itron/config.h include/rtems/itron/eventflags.h \
    include/rtems/itron/fmempool.h include/rtems/itron/intr.h include/rtems/itron/itronapi.h \
    include/rtems/itron/mbox.h include/rtems/itron/msgbuffer.h include/rtems/itron/network.h \
    include/rtems/itron/object.h include/rtems/itron/port.h include/rtems/itron/semaphore.h \
    include/rtems/itron/sysmgmt.h include/rtems/itron/task.h include/rtems/itron/time.h \
    include/rtems/itron/vmempool.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/itron \
    $(include_rtems_itron_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

EXTRA_DIST += include/rtems/itron/README

## Inline

inline_H_FILES = inline/rtems/itron/eventflags.inl \
    inline/rtems/itron/fmempool.inl inline/rtems/itron/intr.inl inline/rtems/itron/mbox.inl \
    inline/rtems/itron/msgbuffer.inl inline/rtems/itron/network.inl inline/rtems/itron/port.inl \
    inline/rtems/itron/semaphore.inl inline/rtems/itron/sysmgmt.inl inline/rtems/itron/task.inl \
    inline/rtems/itron/time.inl inline/rtems/itron/vmempool.inl
noinst_HEADERS += $(inline_H_FILES)

if INLINE
PREINSTALL_FILES += $(inline_H_FILES:inline/%=$(PROJECT_INCLUDE)/%)
include_rtems_itron_HEADERS += $(inline_H_FILES)
endif

## Macros

macros_H_FILES = macros/rtems/itron/eventflags.inl \
    macros/rtems/itron/fmempool.inl macros/rtems/itron/intr.inl macros/rtems/itron/mbox.inl \
    macros/rtems/itron/msgbuffer.inl macros/rtems/itron/network.inl macros/rtems/itron/port.inl \
    macros/rtems/itron/semaphore.inl macros/rtems/itron/sysmgmt.inl macros/rtems/itron/task.inl \
    macros/rtems/itron/time.inl macros/rtems/itron/vmempool.inl
noinst_HEADERS += $(macros_H_FILES)

if MACROS
PREINSTALL_FILES += $(macros_H_FILES:macros/%=$(PROJECT_INCLUDE)/%)
include_rtems_itron_HEADERS += $(macros_H_FILES)
endif

## Sources

LIB=$(ARCH)/libitron.a

TASK_C_FILES = src/task.c src/cre_tsk.c src/del_tsk.c src/sta_tsk.c src/ext_tsk.c src/exd_tsk.c \
    src/ter_tsk.c src/dis_dsp.c src/ena_dsp.c src/chg_pri.c src/rot_rdq.c src/rel_wai.c src/get_tid.c \
    src/ref_tsk.c

TASKSYNC_C_FILES = src/sus_tsk.c src/rsm_tsk.c src/frsm_tsk.c src/slp_tsk.c src/tslp_tsk.c \
    src/wup_tsk.c src/can_wup.c

SEMAPHORE_C_FILES = src/itronsem.c src/cre_sem.c src/del_sem.c src/preq_sem.c src/ref_sem.c \
    src/sig_sem.c src/twai_sem.c src/wai_sem.c

EVENTFLAGS_C_FILES = src/eventflags.c

MAILBOX_C_FILES = src/mbox.c

MSGBUFFER_C_FILES = src/msgbuffer.c src/msgbuffertranslatereturncode.c src/cre_mbf.c \
    src/del_mbf.c src/prcv_mbf.c src/psnd_mbf.c src/rcv_mbf.c src/ref_mbf.c src/snd_mbf.c src/trcv_mbf.c \
    src/tsnd_mbf.c

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
OBJS = $(C_FILES:src/%.c=${ARCH}/%.$(OBJEXT))

AM_CPPFLAGS += -I$(srcdir)/src  -D__RTEMS_INSIDE__

all-local: $(PREINSTALL_FILES) ${ARCH} ${LIB}

$(LIB): ${OBJS}
	$(make-library)
endif

${ARCH}/%.$(OBJEXT): src/%.c
	${COMPILE} -o $@ -c $<

UNUSED_C_FILES = src/cre_mbx.c src/del_mbx.c src/mboxtranslatereturncode.c src/network.c \
    src/prcv_mbx.c src/rcv_mbx.c src/ref_mbx.c src/snd_mbx.c src/sysmgmt.c src/trcv_mbx.c

EXTRA_DIST += $(C_FILES) $(UNUSED_C_FILES) src/TODO

include $(top_srcdir)/automake/local.am
