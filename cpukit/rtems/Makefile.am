##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/rtems:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@
$(PROJECT_INCLUDE)/%.inl: $(INLINEdir)/%.inl
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)

# include

include_HEADERS = include/rtems.h

PREINSTALL_FILES += $(include_HEADERS:include/%.h=$(PROJECT_INCLUDE)/%.h)

# include/rtems/rtems
include_rtems_rtemsdir = $(includedir)/rtems/rtems
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/rtems

include_H_FILES = include/rtems/rtems/asr.h include/rtems/rtems/attr.h include/rtems/rtems/cache.h \
    include/rtems/rtems/clock.h include/rtems/rtems/config.h include/rtems/rtems/dpmem.h \
    include/rtems/rtems/event.h include/rtems/rtems/eventset.h include/rtems/rtems/intr.h \
    include/rtems/rtems/message.h include/rtems/rtems/modes.h include/rtems/rtems/options.h \
    include/rtems/rtems/part.h include/rtems/rtems/ratemon.h include/rtems/rtems/region.h \
    include/rtems/rtems/rtemsapi.h include/rtems/rtems/sem.h include/rtems/rtems/signal.h \
    include/rtems/rtems/status.h include/rtems/rtems/support.h include/rtems/rtems/tasks.h \
    include/rtems/rtems/timer.h include/rtems/rtems/types.h
PREINSTALL_FILES += $(include_H_FILES:include/%=$(PROJECT_INCLUDE)/%)
include_rtems_rtems_HEADERS = $(include_H_FILES)

## We only build multiprocessing related files if HAS_MP was defined
include_MP_H_FILES = include/rtems/rtems/eventmp.h include/rtems/rtems/mp.h include/rtems/rtems/msgmp.h \
    include/rtems/rtems/partmp.h include/rtems/rtems/regionmp.h include/rtems/rtems/semmp.h \
    include/rtems/rtems/signalmp.h include/rtems/rtems/taskmp.h
if HAS_MP
PREINSTALL_FILES += $(include_MP_H_FILES:include/%=$(PROJECT_INCLUDE)/%)
include_rtems_rtems_HEADERS += $(include_MP_H_FILES)
endif
noinst_HEADERS = $(include_MP_H_FILES)

## macros

macro_H_FILES = macros/rtems/rtems/asr.inl macros/rtems/rtems/attr.inl \
    macros/rtems/rtems/dpmem.inl macros/rtems/rtems/event.inl macros/rtems/rtems/eventset.inl \
    macros/rtems/rtems/message.inl macros/rtems/rtems/modes.inl macros/rtems/rtems/options.inl \
    macros/rtems/rtems/part.inl macros/rtems/rtems/ratemon.inl macros/rtems/rtems/region.inl \
    macros/rtems/rtems/sem.inl macros/rtems/rtems/status.inl macros/rtems/rtems/support.inl \
    macros/rtems/rtems/tasks.inl macros/rtems/rtems/timer.inl
noinst_HEADERS += $(macro_H_FILES)

if MACROS
PREINSTALL_FILES += $(macro_H_FILES:macros/%.inl=$(PROJECT_INCLUDE)/%.inl)
include_rtems_rtems_HEADERS += $(macro_H_FILES)
endif

## inline

inline_H_FILES = inline/rtems/rtems/asr.inl inline/rtems/rtems/attr.inl \
    inline/rtems/rtems/dpmem.inl inline/rtems/rtems/event.inl inline/rtems/rtems/eventset.inl \
    inline/rtems/rtems/message.inl inline/rtems/rtems/modes.inl inline/rtems/rtems/options.inl \
    inline/rtems/rtems/part.inl inline/rtems/rtems/ratemon.inl inline/rtems/rtems/region.inl \
    inline/rtems/rtems/sem.inl inline/rtems/rtems/status.inl inline/rtems/rtems/support.inl \
    inline/rtems/rtems/tasks.inl inline/rtems/rtems/timer.inl

if INLINE
PREINSTALL_FILES += $(inline_H_FILES:inline/%.inl=$(PROJECT_INCLUDE)/%.inl)
include_rtems_rtems_HEADERS += $(inline_H_FILES)
endif
noinst_HEADERS += $(inline_H_FILES)

## src
LIB = $(ARCH)/librtems.a

# We only build multiprocessing related files if HAS_MP was defined
MP_C_FILES = src/eventmp.c src/mp.c src/msgmp.c src/partmp.c src/regionmp.c \
    src/semmp.c src/signalmp.c src/taskmp.c

TASK_C_FILES = src/tasks.c src/taskcreate.c src/taskdelete.c src/taskgetnote.c src/taskident.c \
    src/taskinitusers.c src/taskissuspended.c src/taskmode.c src/taskrestart.c src/taskresume.c \
    src/tasksetnote.c src/tasksetpriority.c src/taskstart.c src/tasksuspend.c \
    src/taskwakeafter.c src/taskwakewhen.c src/taskvariableadd.c src/taskvariabledelete.c \
    src/taskvariableget.c

RATEMON_C_FILES = src/ratemon.c src/ratemoncancel.c src/ratemoncreate.c src/ratemondelete.c \
    src/ratemongetstatus.c src/ratemonident.c src/ratemonperiod.c src/ratemontimeout.c

INTR_C_FILES = src/intr.c src/intrbody.c src/intrcatch.c

CLOCK_C_FILES = src/rtclock.c src/clockget.c src/clockset.c src/clocktick.c

TIMER_C_FILES = src/rtemstimer.c src/timercancel.c src/timercreate.c src/timerdelete.c \
    src/timerfireafter.c src/timerfirewhen.c src/timergetinfo.c src/timerident.c src/timerreset.c \
    src/timerserver.c src/timerserverfireafter.c src/timerserverfirewhen.c

MESSAGE_QUEUE_C_FILES = src/msg.c src/msgqallocate.c src/msgqbroadcast.c src/msgqcreate.c \
    src/msgqdelete.c src/msgqflush.c src/msgqgetnumberpending.c src/msgqident.c \
    src/msgqreceive.c src/msgqsend.c src/msgqsubmit.c src/msgqtranslatereturncode.c \
    src/msgqurgent.c

SEMAPHORE_C_FILES = src/sem.c src/semcreate.c src/semdelete.c src/semident.c src/semobtain.c \
    src/semrelease.c src/semflush.c src/semtranslatereturncode.c

EVENT_C_FILES = src/event.c src/eventreceive.c src/eventseize.c src/eventsend.c \
    src/eventsurrender.c src/eventtimeout.c

SIGNAL_C_FILES = src/signal.c src/signalcatch.c src/signalsend.c

REGION_C_FILES = src/region.c src/regioncreate.c src/regiondelete.c src/regionextend.c \
    src/regiongetsegment.c src/regiongetsegmentsize.c src/regionident.c \
    src/regionreturnsegment.c src/regiongetinfo.c

PARTITION_C_FILES = src/part.c src/partcreate.c src/partdelete.c src/partgetbuffer.c \
    src/partident.c src/partreturnbuffer.c

DPMEM_C_FILES = src/dpmem.c src/dpmemcreate.c src/dpmemdelete.c src/dpmemexternal2internal.c \
    src/dpmemident.c src/dpmeminternal2external.c

STD_C_FILES = src/attr.c $(TASK_C_FILES) $(RATEMON_C_FILES) $(INTR_C_FILES) \
    $(CLOCK_C_FILES) $(TIMER_C_FILES) $(SEMAPHORE_C_FILES) \
    $(MESSAGE_QUEUE_C_FILES) $(EVENT_C_FILES) $(SIGNAL_C_FILES) \
    $(PARTITION_C_FILES) $(REGION_C_FILES) $(DPMEM_C_FILES)

if HAS_MP
C_FILES = $(STD_C_FILES) $(MP_C_FILES)
else
C_FILES = $(STD_C_FILES)
endif

OBJS = $(C_FILES:src/%.c=${ARCH}/%.$(OBJEXT))

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D__RTEMS_INSIDE__

all-local: ${ARCH} $(PREINSTALL_FILES) $(LIB)

${ARCH}/%.o: src/%.c
	test -d $(ARCH) || mkdir $(ARCH)
	${COMPILE} -o $@ -c $<

$(LIB): ${OBJS}
	$(make-library)

EXTRA_DIST = $(STD_C_FILES) $(MP_C_FILES)

include $(top_srcdir)/automake/local.am
