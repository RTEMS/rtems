##
##  $Id$
##

LIB=$(ARCH)/libitron.a

INCLUDES = -I.

TASK_C_FILES = task.c cre_tsk.c del_tsk.c sta_tsk.c ext_tsk.c exd_tsk.c \
    ter_tsk.c dis_dsp.c ena_dsp.c chg_pri.c rot_rdq.c rel_wai.c get_tid.c \
    ref_tsk.c

TASKSYNC_C_FILES = sus_tsk.c rsm_tsk.c frsm_tsk.c slp_tsk.c tslp_tsk.c \
    wup_tsk.c can_wup.c

SEMAPHORE_C_FILES = itronsem.c cre_sem.c del_sem.c preq_sem.c ref_sem.c \
    sig_sem.c twai_sem.c wai_sem.c

EVENTFLAGS_C_FILES = eventflags.c

MAILBOX_C_FILES = mbox.c

MSGBUFFER_C_FILES = msgbuffer.c msgbuffertranslatereturncode.c cre_mbf.c \
    del_mbf.c prcv_mbf.c psnd_mbf.c rcv_mbf.c ref_mbf.c snd_mbf.c trcv_mbf.c \
    tsnd_mbf.c

RENDEZVOUS_C_FILES = port.c

INTERRUPT_C_FILES = itronintr.c

VARIABLE_MEMORY_POOL_C_FILES = vmempool.c

FIXED_MEMORY_POOL_C_FILES = fmempool.c

TIME_C_FILES = itrontime.c

CONFIGURATION_C_FILES = sysmgmt.c

NETWORK_C_FILES = network.c

C_FILES = $(TASK_C_FILES) $(TASKSYNC_C_FILES) $(SEMAPHORE_C_FILES) \
    $(EVENTFLAGS_C_FILES) $(MAILBOX_C_FILES) $(MSGBUFFER_C_FILES) \
    $(RENDEZVOUS_C_FILES) $(INTERRUPT_C_FILES) \
    $(VARIABLE_MEMORY_POOL_C_FILES) $(FIXED_MEMORY_POOL_C_FILES) \
    $(TIME_C_FILES)
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

OBJS = $(C_O_FILES)

include $(top_srcdir)/../automake/multilib.am
include $(top_srcdir)/../automake/compile.am
include $(top_srcdir)/../automake/lib.am

AM_CPPFLAGS += -D__RTEMS_INSIDE__

all-local: ${ARCH} ${LIB}

$(LIB): ${OBJS}
	$(make-library)

UNUSED_C_FILES = cre_mbx.c del_mbx.c mboxtranslatereturncode.c network.c \
    prcv_mbx.c rcv_mbx.c ref_mbx.c snd_mbx.c sysmgmt.c trcv_mbx.c

EXTRA_DIST = $(C_FILES) $(UNUSED_C_FILES)

include $(top_srcdir)/../automake/local.am
