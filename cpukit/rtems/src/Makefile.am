##
##  $Id$
##

LIB = $(ARCH)/librtems.a

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

# We only build multiprocessing related files if HAS_MP was defined
MP_C_FILES = eventmp.c mp.c msgmp.c partmp.c regionmp.c semmp.c signalmp.c \
    taskmp.c

TASK_C_FILES = tasks.c taskcreate.c taskdelete.c taskgetnote.c taskident.c \
    taskinitusers.c taskissuspended.c taskmode.c taskrestart.c taskresume.c \
    tasksetnote.c tasksetpriority.c taskstart.c tasksuspend.c \
    taskwakeafter.c taskwakewhen.c taskvariableadd.c taskvariabledelete.c \
    taskvariableget.c

RATEMON_C_FILES = ratemon.c ratemoncancel.c ratemoncreate.c ratemondelete.c \
    ratemongetstatus.c ratemonident.c ratemonperiod.c ratemontimeout.c

INTR_C_FILES = intr.c intrbody.c intrcatch.c

CLOCK_C_FILES = rtclock.c clockget.c clockset.c clocktick.c

TIMER_C_FILES = rtemstimer.c timercancel.c timercreate.c timerdelete.c \
    timerfireafter.c timerfirewhen.c timergetinfo.c timerident.c timerreset.c \
    timerserver.c timerserverfireafter.c timerserverfirewhen.c

MESSAGE_QUEUE_C_FILES = msg.c msgqallocate.c msgqbroadcast.c msgqcreate.c \
    msgqdelete.c msgqflush.c msgqgetnumberpending.c msgqident.c \
    msgqreceive.c msgqsend.c msgqsubmit.c msgqtranslatereturncode.c \
    msgqurgent.c

SEMAPHORE_C_FILES = sem.c semcreate.c semdelete.c semident.c semobtain.c \
    semrelease.c semflush.c semtranslatereturncode.c

EVENT_C_FILES = event.c eventreceive.c eventseize.c eventsend.c \
    eventsurrender.c eventtimeout.c

SIGNAL_C_FILES = signal.c signalcatch.c signalsend.c

REGION_C_FILES = region.c regioncreate.c regiondelete.c regionextend.c \
    regiongetsegment.c regiongetsegmentsize.c regionident.c \
    regionreturnsegment.c regiongetinfo.c

PARTITION_C_FILES = part.c partcreate.c partdelete.c partgetbuffer.c \
    partident.c partreturnbuffer.c

DPMEM_C_FILES = dpmem.c dpmemcreate.c dpmemdelete.c dpmemexternal2internal.c \
    dpmemident.c dpmeminternal2external.c

STD_C_FILES = attr.c $(TASK_C_FILES) $(RATEMON_C_FILES) $(INTR_C_FILES) \
    $(CLOCK_C_FILES) $(TIMER_C_FILES) $(SEMAPHORE_C_FILES) \
    $(MESSAGE_QUEUE_C_FILES) $(EVENT_C_FILES) $(SIGNAL_C_FILES) \
    $(PARTITION_C_FILES) $(REGION_C_FILES) $(DPMEM_C_FILES)

if HAS_MP
C_FILES = $(STD_C_FILES) $(MP_C_FILES)
else
C_FILES = $(STD_C_FILES)
endif

C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

OBJS = $(C_O_FILES)

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D__RTEMS_INSIDE__

all-local: ${ARCH} $(LIB)

$(LIB): ${OBJS}
	$(make-library)


EXTRA_DIST = $(STD_C_FILES) $(MP_C_FILES)

include $(top_srcdir)/automake/local.am
