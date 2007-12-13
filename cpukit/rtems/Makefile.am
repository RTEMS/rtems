##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

AM_CPPFLAGS += -D__RTEMS_INSIDE__

noinst_LIBRARIES = librtems.a
librtems_a_CPPFLAGS = $(AM_CPPFLAGS)

# include

include_HEADERS = include/rtems.h

# include/rtems/rtems
include_rtems_rtemsdir = $(includedir)/rtems/rtems

include_rtems_rtems_HEADERS = include/rtems/rtems/asr.h \
    include/rtems/rtems/attr.h include/rtems/rtems/barrier.h \
    include/rtems/rtems/cache.h \
    include/rtems/rtems/clock.h include/rtems/rtems/config.h \
    include/rtems/rtems/dpmem.h include/rtems/rtems/event.h \
    include/rtems/rtems/eventset.h include/rtems/rtems/intr.h \
    include/rtems/rtems/message.h include/rtems/rtems/modes.h \
    include/rtems/rtems/options.h include/rtems/rtems/part.h \
    include/rtems/rtems/ratemon.h include/rtems/rtems/region.h \
    include/rtems/rtems/rtemsapi.h include/rtems/rtems/sem.h \
    include/rtems/rtems/signal.h include/rtems/rtems/status.h \
    include/rtems/rtems/support.h include/rtems/rtems/tasks.h \
    include/rtems/rtems/timer.h include/rtems/rtems/types.h

if HAS_MP
## We only build multiprocessing related files if HAS_MP was defined
include_rtems_rtems_HEADERS += include/rtems/rtems/eventmp.h \
    include/rtems/rtems/mp.h include/rtems/rtems/msgmp.h \
    include/rtems/rtems/partmp.h include/rtems/rtems/regionmp.h \
    include/rtems/rtems/semmp.h include/rtems/rtems/signalmp.h \
    include/rtems/rtems/taskmp.h
endif

include_rtems_rtems_HEADERS += inline/rtems/rtems/asr.inl \
    inline/rtems/rtems/attr.inl inline/rtems/rtems/barrier.inl \
    inline/rtems/rtems/dpmem.inl \
    inline/rtems/rtems/event.inl inline/rtems/rtems/eventset.inl \
    inline/rtems/rtems/message.inl inline/rtems/rtems/modes.inl \
    inline/rtems/rtems/options.inl inline/rtems/rtems/part.inl \
    inline/rtems/rtems/ratemon.inl inline/rtems/rtems/region.inl \
    inline/rtems/rtems/sem.inl inline/rtems/rtems/status.inl \
    inline/rtems/rtems/support.inl inline/rtems/rtems/tasks.inl \
    inline/rtems/rtems/timer.inl

## src
librtems_a_SOURCES = src/rtemsobjectgetname.c

## TASK_C_FILES 
librtems_a_SOURCES += src/tasks.c src/taskcreate.c src/taskdelete.c \
    src/taskgetnote.c src/taskident.c src/taskinitusers.c \
    src/taskissuspended.c src/taskmode.c src/taskrestart.c src/taskresume.c \
    src/tasksetnote.c src/tasksetpriority.c src/taskstart.c \
    src/tasksuspend.c src/taskwakeafter.c src/taskwakewhen.c \
    src/taskvariableadd.c src/taskvariabledelete.c src/taskvariableget.c \
    src/taskvariable_invoke_dtor.c src/taskdata.c

## RATEMON_C_FILES 
librtems_a_SOURCES += src/ratemon.c src/ratemoncancel.c src/ratemoncreate.c \
    src/ratemondelete.c src/ratemongetstatus.c src/ratemongetstatistics.c \
    src/ratemonresetstatistics.c src/ratemonresetall.c \
    src/ratemonreportstatistics.c src/ratemonident.c \
    src/ratemonperiod.c src/ratemontimeout.c src/ratemondata.c

## INTR_C_FILES 
librtems_a_SOURCES += src/intr.c src/intrbody.c src/intrcatch.c

## BARRIER_C_FILES
librtems_a_SOURCES += src/barrier.c src/barriercreate.c src/barrierdelete.c \
   src/barrierident.c src/barriertranslatereturncode.c src/barrierrelease.c \
   src/barrierwait.c src/barrierdata.c

## CLOCK_C_FILES
librtems_a_SOURCES += src/rtclock.c src/clockget.c src/clockset.c \
    src/clocktick.c src/clocksetnsecshandler.c src/clockgetuptime.c \
    src/clocktodtoseconds.c src/clocktodvalidate.c

## TIMER_C_FILES
librtems_a_SOURCES += src/rtemstimer.c src/timercancel.c src/timercreate.c \
    src/timerdelete.c src/timerfireafter.c src/timerfirewhen.c \
    src/timergetinfo.c src/timerident.c src/timerreset.c src/timerserver.c \
    src/timerserverfireafter.c src/timerserverfirewhen.c src/rtemstimerdata.c

## MESSAGE_QUEUE_C_FILES
librtems_a_SOURCES += src/msg.c src/msgqallocate.c src/msgqbroadcast.c \
    src/msgqcreate.c src/msgqdelete.c src/msgqflush.c \
    src/msgqgetnumberpending.c src/msgqident.c src/msgqreceive.c \
    src/msgqsend.c c src/msgqtranslatereturncode.c src/msgqurgent.c \
    src/msgdata.c

## SEMAPHORE_C_FILES
librtems_a_SOURCES += src/sem.c src/semcreate.c src/semdelete.c src/semident.c \
    src/semobtain.c src/semrelease.c src/semflush.c \
    src/semtranslatereturncode.c src/semdata.c

## EVENT_C_FILES
librtems_a_SOURCES += src/event.c src/eventreceive.c src/eventseize.c \
    src/eventsend.c src/eventsurrender.c src/eventtimeout.c src/eventdata.c

## SIGNAL_C_FILES
librtems_a_SOURCES += src/signal.c src/signalcatch.c src/signalsend.c

## REGION_C_FILES
librtems_a_SOURCES += src/region.c src/regioncreate.c src/regiondelete.c \
    src/regionextend.c src/regiongetsegment.c src/regiongetsegmentsize.c \
    src/regionident.c src/regionreturnsegment.c src/regiongetinfo.c \
    src/regiongetfreeinfo.c src/regionresizesegment.c \
    src/regionprocessqueue.c src/regiondata.c

## PARTITION_C_FILES
librtems_a_SOURCES += src/part.c src/partcreate.c src/partdelete.c \
    src/partgetbuffer.c src/partident.c src/partreturnbuffer.c src/partdata.c

## DPMEM_C_FILES
librtems_a_SOURCES += src/dpmem.c src/dpmemcreate.c src/dpmemdelete.c \
    src/dpmemexternal2internal.c src/dpmemident.c \
    src/dpmeminternal2external.c src/dpmemdata.c

librtems_a_SOURCES += src/attr.c src/rtemsidtoname.c

if HAS_MP
# We only build multiprocessing related files if HAS_MP was defined
librtems_a_SOURCES += src/eventmp.c src/mp.c src/msgmp.c src/partmp.c \
    src/regionmp.c src/semmp.c src/signalmp.c src/taskmp.c
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
