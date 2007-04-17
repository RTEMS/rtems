##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

EXTRA_DIST =

if LIBITRON
## include

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = include/itron.h

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

include_rtems_itron_HEADERS += inline/rtems/itron/eventflags.inl \
    inline/rtems/itron/fmempool.inl inline/rtems/itron/intr.inl \
    inline/rtems/itron/mbox.inl inline/rtems/itron/msgbuffer.inl \
    inline/rtems/itron/network.inl inline/rtems/itron/port.inl \
    inline/rtems/itron/semaphore.inl inline/rtems/itron/sysmgmt.inl \
    inline/rtems/itron/task.inl inline/rtems/itron/time.inl \
    inline/rtems/itron/vmempool.inl

## Sources

TASK_C_FILES = src/task.c src/cre_tsk.c src/del_tsk.c src/sta_tsk.c \
    src/ext_tsk.c src/exd_tsk.c src/ter_tsk.c src/dis_dsp.c src/ena_dsp.c \
    src/chg_pri.c src/rot_rdq.c src/rel_wai.c src/get_tid.c src/ref_tsk.c \
    src/itroninittasks.c

TASKSYNC_C_FILES = src/sus_tsk.c src/rsm_tsk.c src/frsm_tsk.c src/slp_tsk.c \
    src/tslp_tsk.c src/wup_tsk.c src/can_wup.c

SEMAPHORE_C_FILES = src/itronsem.c src/cre_sem.c src/del_sem.c \
    src/preq_sem.c src/ref_sem.c src/sig_sem.c src/twai_sem.c src/wai_sem.c

EVENTFLAGS_C_FILES = src/eventflags.c src/cre_flg.c src/del_flg.c \
    src/set_flg.c src/clr_flg.c src/wai_flg.c src/pol_flg.c \
    src/twai_flg.c src/ref_flg.c

MAILBOX_C_FILES = src/mbox.c

MSGBUFFER_C_FILES = src/msgbuffer.c src/msgbuffertranslatereturncode.c \
    src/cre_mbf.c src/del_mbf.c src/prcv_mbf.c src/psnd_mbf.c src/rcv_mbf.c \
    src/ref_mbf.c src/snd_mbf.c src/trcv_mbf.c src/tsnd_mbf.c

RENDEZVOUS_C_FILES = src/port.c src/cre_por.c src/del_por.c src/cal_por.c \
    src/pcal_por.c src/tcal_por.c src/acp_por.c src/pacp_por.c \
    src/tacp_por.c src/fwd_por.c src/rpl_rdv.c src/ref_por.c

INTERRUPT_C_FILES = src/itronintr.c src/def_int.c src/loc_cpu.c \
    src/unl_cpu.c src/dis_int.c src/ena_int.c src/chg_iXX.c src/ref_iXX.c \
    src/ret_int.c src/ret_wup.c

VARIABLE_MEMORY_POOL_C_FILES = src/vmempool.c src/cre_mpl.c src/del_mpl.c \
    src/get_blk.c src/pget_blk.c src/tget_blk.c src/rel_blk.c src/ref_mpl.c

FIXED_MEMORY_POOL_C_FILES = src/fmempool.c src/cre_mpf.c src/del_mpf.c \
    src/get_blf.c src/pget_blf.c src/tget_blf.c src/rel_blf.c src/ref_mpf.c

TIME_C_FILES = src/itrontime.c src/set_tim.c src/get_tim.c src/dly_tsk.c \
    src/def_cyc.c src/act_cyc.c src/ref_cyc.c src/def_alm.c src/ref_alm.c \
    src/ret_tmr.c 

CONFIGURATION_C_FILES = src/sysmgmt.c src/get_ver.c src/ref_sys.c \
    src/ref_cfg.c src/def_svc.c src/def_exc.c

NETWORK_C_FILES = src/network.c src/nrea_dat.c src/nwri_dat.c \
    src/nget_nod.c src/nget_ver.c

C_FILES = $(TASK_C_FILES) $(TASKSYNC_C_FILES) $(SEMAPHORE_C_FILES) \
    $(EVENTFLAGS_C_FILES) $(MAILBOX_C_FILES) $(MSGBUFFER_C_FILES) \
    $(RENDEZVOUS_C_FILES) $(INTERRUPT_C_FILES) \
    $(VARIABLE_MEMORY_POOL_C_FILES) $(FIXED_MEMORY_POOL_C_FILES) \
    $(TIME_C_FILES)

AM_CPPFLAGS += -D__RTEMS_INSIDE__

noinst_LIBRARIES = libitron.a
libitron_a_SOURCES = $(C_FILES)
libitron_a_CPPFLAGS = $(AM_CPPFLAGS)
endif

UNUSED_C_FILES = src/cre_mbx.c src/del_mbx.c src/mboxtranslatereturncode.c \
    $(NETWORK_C_FILES) src/prcv_mbx.c src/rcv_mbx.c src/ref_mbx.c \
    src/snd_mbx.c $(CONFIGURATION_C_FILES) src/trcv_mbx.c

EXTRA_DIST += $(UNUSED_C_FILES) src/TODO

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
