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
include_rtems_rtems_HEADERS =

include_rtems_rtems_HEADERS += include/rtems/rtems/asr.h
include_rtems_rtems_HEADERS += include/rtems/rtems/attr.h
include_rtems_rtems_HEADERS += include/rtems/rtems/barrier.h
include_rtems_rtems_HEADERS += include/rtems/rtems/cache.h
include_rtems_rtems_HEADERS += include/rtems/rtems/clock.h
include_rtems_rtems_HEADERS += include/rtems/rtems/config.h
include_rtems_rtems_HEADERS += include/rtems/rtems/dpmem.h
include_rtems_rtems_HEADERS += include/rtems/rtems/event.h
include_rtems_rtems_HEADERS += include/rtems/rtems/eventset.h
include_rtems_rtems_HEADERS += include/rtems/rtems/intr.h
include_rtems_rtems_HEADERS += include/rtems/rtems/message.h
include_rtems_rtems_HEADERS += include/rtems/rtems/modes.h
include_rtems_rtems_HEADERS += include/rtems/rtems/object.h
include_rtems_rtems_HEADERS += include/rtems/rtems/options.h
include_rtems_rtems_HEADERS += include/rtems/rtems/part.h
include_rtems_rtems_HEADERS += include/rtems/rtems/ratemon.h
include_rtems_rtems_HEADERS += include/rtems/rtems/region.h
include_rtems_rtems_HEADERS += include/rtems/rtems/rtemsapi.h
include_rtems_rtems_HEADERS += include/rtems/rtems/sem.h
include_rtems_rtems_HEADERS += include/rtems/rtems/signal.h
include_rtems_rtems_HEADERS += include/rtems/rtems/status.h
include_rtems_rtems_HEADERS += include/rtems/rtems/support.h
include_rtems_rtems_HEADERS += include/rtems/rtems/tasks.h
include_rtems_rtems_HEADERS += include/rtems/rtems/timer.h
include_rtems_rtems_HEADERS += include/rtems/rtems/types.h
include_rtems_rtems_HEADERS += mainpage.h

if HAS_MP
## We only build multiprocessing related files if HAS_MP was defined
include_rtems_rtems_HEADERS += include/rtems/rtems/eventmp.h
include_rtems_rtems_HEADERS += include/rtems/rtems/mp.h
include_rtems_rtems_HEADERS += include/rtems/rtems/msgmp.h
include_rtems_rtems_HEADERS += include/rtems/rtems/partmp.h
include_rtems_rtems_HEADERS += include/rtems/rtems/regionmp.h
include_rtems_rtems_HEADERS += include/rtems/rtems/semmp.h
include_rtems_rtems_HEADERS += include/rtems/rtems/signalmp.h
include_rtems_rtems_HEADERS += include/rtems/rtems/taskmp.h
endif

include_rtems_rtems_HEADERS += inline/rtems/rtems/asr.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/attr.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/barrier.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/dpmem.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/event.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/eventset.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/message.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/modes.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/options.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/part.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/ratemon.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/region.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/sem.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/status.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/support.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/tasks.inl
include_rtems_rtems_HEADERS += inline/rtems/rtems/timer.inl

## src
librtems_a_SOURCES = src/rtemsbuildid.c src/rtemsbuildname.c \
    src/rtemsobjectgetname.c src/rtemsobjectsetname.c \
    src/rtemsobjectidapimaximum.c src/rtemsobjectidapiminimum.c \
    src/rtemsobjectidgetapi.c src/rtemsobjectidgetclass.c \
    src/rtemsobjectidgetindex.c src/rtemsobjectidgetnode.c \
    src/rtemsobjectapiminimumclass.c src/rtemsobjectapimaximumclass.c \
    src/rtemsobjectgetapiname.c src/rtemsobjectgetapiclassname.c \
    src/rtemsobjectgetclassinfo.c src/rtemsobjectgetclassicname.c

## TASK_C_FILES
librtems_a_SOURCES += src/tasks.c src/taskcreate.c src/taskdelete.c \
    src/taskgetnote.c src/taskident.c src/taskinitusers.c \
    src/taskissuspended.c src/taskmode.c src/taskrestart.c src/taskresume.c \
    src/taskself.c src/tasksetnote.c src/tasksetpriority.c src/taskstart.c \
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
librtems_a_SOURCES += src/intrbody.c src/intrcatch.c

## BARRIER_C_FILES
librtems_a_SOURCES += src/barrier.c src/barriercreate.c src/barrierdelete.c \
   src/barrierident.c src/barriertranslatereturncode.c src/barrierrelease.c \
   src/barrierwait.c src/barrierdata.c

## CLOCK_C_FILES
librtems_a_SOURCES += src/clockget.c src/clockgetsecondssinceepoch.c \
    src/clockgettickspersecond.c src/clockgettickssinceboot.c \
    src/clockgettod.c src/clockgettodtimeval.c src/clockgetuptime.c \
    src/clockset.c src/clocksetnsecshandler.c src/clocktick.c \
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
    src/msgqsend.c src/msgqtranslatereturncode.c src/msgqurgent.c \
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

## WORKSPACE_FILES
librtems_a_SOURCES += src/workspace.c

librtems_a_SOURCES += src/attr.c

if HAS_MP
# We only build multiprocessing related files if HAS_MP was defined
librtems_a_SOURCES += src/eventmp.c src/mp.c src/msgmp.c src/partmp.c \
    src/regionmp.c src/semmp.c src/signalmp.c src/taskmp.c
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/automake/local.am
