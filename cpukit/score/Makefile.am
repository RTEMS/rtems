##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am

SUBDIRS = . cpu

## include

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = include/rtems/debug.h include/rtems/system.h \
    include/rtems/seterr.h

include_rtems_scoredir = $(includedir)/rtems/score

include_rtems_score_HEADERS = include/rtems/score/address.h \
    include/rtems/score/apiext.h include/rtems/score/apimutex.h \
    include/rtems/score/bitfield.h include/rtems/score/chain.h \
    include/rtems/score/context.h include/rtems/score/copyrt.h \
    include/rtems/score/coremsg.h include/rtems/score/coremutex.h \
    include/rtems/score/coresem.h include/rtems/score/heap.h \
    include/rtems/score/interr.h include/rtems/score/isr.h \
    include/rtems/score/object.h include/rtems/score/priority.h \
    include/rtems/score/stack.h include/rtems/score/states.h \
    include/rtems/score/sysstate.h include/rtems/score/thread.h \
    include/rtems/score/threadq.h include/rtems/score/tod.h \
    include/rtems/score/tqdata.h include/rtems/score/userext.h \
    include/rtems/score/watchdog.h include/rtems/score/wkspace.h \
    include/rtems/score/cpuopts.h

if HAS_MP
# We only build multiprocessing related files if HAS_MP was defined
include_rtems_score_HEADERS += include/rtems/score/mpci.h \
    include/rtems/score/mppkt.h include/rtems/score/objectmp.h \
    include/rtems/score/threadmp.h
endif

## inline
if INLINE
include_rtems_score_HEADERS += inline/rtems/score/address.inl \
    inline/rtems/score/chain.inl inline/rtems/score/coremsg.inl \
    inline/rtems/score/coremutex.inl inline/rtems/score/coresem.inl \
    inline/rtems/score/heap.inl inline/rtems/score/isr.inl \
    inline/rtems/score/object.inl inline/rtems/score/priority.inl \
    inline/rtems/score/stack.inl inline/rtems/score/states.inl \
    inline/rtems/score/sysstate.inl inline/rtems/score/thread.inl \
    inline/rtems/score/tod.inl inline/rtems/score/tqdata.inl \
    inline/rtems/score/userext.inl inline/rtems/score/watchdog.inl \
    inline/rtems/score/wkspace.inl

if HAS_MP
## We only build multiprocessing related files if HAS_MP was defined
include_rtems_score_HEADERS += inline/rtems/score/mppkt.inl \
    inline/rtems/score/objectmp.inl inline/rtems/score/threadmp.inl
endif
else
## macros
include_rtems_score_HEADERS += macros/rtems/score/address.inl \
    macros/rtems/score/chain.inl macros/rtems/score/coremsg.inl \
    macros/rtems/score/coremutex.inl macros/rtems/score/coresem.inl \
    macros/rtems/score/heap.inl macros/rtems/score/isr.inl \
    macros/rtems/score/object.inl macros/rtems/score/priority.inl \
    macros/rtems/score/stack.inl macros/rtems/score/states.inl \
    macros/rtems/score/sysstate.inl macros/rtems/score/thread.inl \
    macros/rtems/score/tod.inl macros/rtems/score/tqdata.inl \
    macros/rtems/score/userext.inl macros/rtems/score/watchdog.inl \
    macros/rtems/score/wkspace.inl
if HAS_MP
## We only build multiprocessing related files if HAS_MP was defined
include_rtems_score_HEADERS += macros/rtems/score/mppkt.inl \
    macros/rtems/score/objectmp.inl macros/rtems/score/threadmp.inl
endif
endif

EXTRA_DIST = macros/README

## src

# We only build multiprocessing related files if HAS_MP was defined
MP_C_FILES = src/mpci.c src/objectmp.c src/threadmp.c

CORE_MESSAGE_QUEUE_C_FILES = src/coremsg.c src/coremsgbroadcast.c \
    src/coremsgclose.c src/coremsgflush.c src/coremsgflushwait.c \
    src/coremsginsert.c src/coremsgflushsupp.c src/coremsgseize.c \
    src/coremsgsubmit.c

CORE_MUTEX_C_FILES = src/coremutex.c src/coremutexflush.c \
    src/coremutexseize.c src/coremutexsurrender.c

CORE_SEMAPHORE_C_FILES = src/coresem.c src/coresemflush.c src/coresemseize.c \
    src/coresemsurrender.c

HEAP_C_FILES = src/heap.c src/heapallocate.c src/heapextend.c src/heapfree.c \
    src/heapsizeofuserarea.c src/heapwalk.c src/heapgetinfo.c

OBJECT_C_FILES = src/object.c src/objectallocate.c \
    src/objectallocatebyindex.c src/objectclearname.c \
    src/objectcomparenameraw.c src/objectcomparenamestring.c \
    src/objectcopynameraw.c src/objectcopynamestring.c \
    src/objectextendinformation.c src/objectfree.c src/objectget.c \
    src/objectgetisr.c src/objectgetbyindex.c src/objectgetnext.c \
    src/objectinitializeinformation.c src/objectnametoid.c \
    src/objectshrinkinformation.c src/objectgetnoprotection.c \
    src/objectidtoname.c

THREAD_C_FILES = src/thread.c src/threadchangepriority.c \
    src/threadclearstate.c src/threadclose.c src/threadcreateidle.c \
    src/threaddelayended.c src/threaddispatch.c src/threadevaluatemode.c \
    src/threadget.c src/threadhandler.c src/threadidlebody.c \
    src/threadinitialize.c src/threadloadenv.c src/threadready.c \
    src/threadresettimeslice.c src/threadreset.c src/threadrestart.c \
    src/threadresume.c src/threadrotatequeue.c src/threadsetpriority.c \
    src/threadsetstate.c src/threadsettransient.c src/threadstackallocate.c \
    src/threadstackfree.c src/threadstart.c src/threadstartmultitasking.c \
    src/threadsuspend.c src/threadtickletimeslice.c \
    src/threadyieldprocessor.c src/iterateoverthreads.c

THREADQ_C_FILES = src/threadq.c src/threadqdequeue.c \
    src/threadqdequeuefifo.c src/threadqdequeuepriority.c \
    src/threadqenqueue.c src/threadqenqueuefifo.c \
    src/threadqenqueuepriority.c src/threadqextract.c \
    src/threadqextractfifo.c src/threadqextractpriority.c \
    src/threadqextractwithproxy.c src/threadqfirst.c src/threadqfirstfifo.c \
    src/threadqfirstpriority.c src/threadqflush.c src/threadqtimeout.c

TOD_C_FILES = src/coretod.c src/coretodset.c src/coretodtickle.c \
    src/coretodtoseconds.c src/coretodvalidate.c

WATCHDOG_C_FILES = src/watchdog.c src/watchdogadjust.c src/watchdoginsert.c \
    src/watchdogremove.c src/watchdogtickle.c

STD_C_FILES = src/apiext.c src/chain.c $(CORE_MESSAGE_QUEUE_C_FILES) \
    $(CORE_MUTEX_C_FILES) $(CORE_SEMAPHORE_C_FILES) $(HEAP_C_FILES) \
    src/interr.c src/isr.c $(OBJECT_C_FILES) $(THREAD_C_FILES) \
    $(THREADQ_C_FILES) $(TOD_C_FILES) src/userext.c $(WATCHDOG_C_FILES) \
    src/wkspace.c

C_FILES = $(STD_C_FILES)
if HAS_MP
C_FILES += $(MP_C_FILES)
endif

AM_CPPFLAGS += -D__RTEMS_INSIDE__

EXTRA_LIBRARIES = libscore.a
CLEANFILES = libscore.a
libscore_a_SOURCES = $(C_FILES)
libscore_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_OPTIMIZE_V)

EXTRA_LIBRARIES += libscore_g.a
CLEANFILES += libscore_g.a
libscore_g_a_SOURCES = $(libscore_a_SOURCES)
libscore_g_a_CPPFLAGS = $(AM_CPPFLAGS) $(CFLAGS_DEBUG_V)

EXTRA_DIST += $(STD_C_FILES) $(MP_C_FILES) src/Unlimited.txt

all-local: $(PREINSTALL_FILES) libscore$(LIB_VARIANT).a

PREINSTALL_FILES =

$(PROJECT_INCLUDE)/rtems/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems
	@: > $(PROJECT_INCLUDE)/rtems/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/debug.h: include/rtems/debug.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/debug.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/debug.h

$(PROJECT_INCLUDE)/rtems/system.h: include/rtems/system.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/system.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/system.h

$(PROJECT_INCLUDE)/rtems/seterr.h: include/rtems/seterr.h $(PROJECT_INCLUDE)/rtems/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/seterr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/seterr.h

$(PROJECT_INCLUDE)/rtems/score/$(dirstamp):
	@$(mkdir_p) $(PROJECT_INCLUDE)/rtems/score
	@: > $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)

$(PROJECT_INCLUDE)/rtems/score/address.h: include/rtems/score/address.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/address.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/address.h

$(PROJECT_INCLUDE)/rtems/score/apiext.h: include/rtems/score/apiext.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/apiext.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/apiext.h

$(PROJECT_INCLUDE)/rtems/score/apimutex.h: include/rtems/score/apimutex.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/apimutex.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/apimutex.h

$(PROJECT_INCLUDE)/rtems/score/bitfield.h: include/rtems/score/bitfield.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/bitfield.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/bitfield.h

$(PROJECT_INCLUDE)/rtems/score/chain.h: include/rtems/score/chain.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/chain.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/chain.h

$(PROJECT_INCLUDE)/rtems/score/context.h: include/rtems/score/context.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/context.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/context.h

$(PROJECT_INCLUDE)/rtems/score/copyrt.h: include/rtems/score/copyrt.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/copyrt.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/copyrt.h

$(PROJECT_INCLUDE)/rtems/score/coremsg.h: include/rtems/score/coremsg.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/coremsg.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/coremsg.h

$(PROJECT_INCLUDE)/rtems/score/coremutex.h: include/rtems/score/coremutex.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/coremutex.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/coremutex.h

$(PROJECT_INCLUDE)/rtems/score/coresem.h: include/rtems/score/coresem.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/coresem.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/coresem.h

$(PROJECT_INCLUDE)/rtems/score/heap.h: include/rtems/score/heap.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/heap.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/heap.h

$(PROJECT_INCLUDE)/rtems/score/interr.h: include/rtems/score/interr.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/interr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/interr.h

$(PROJECT_INCLUDE)/rtems/score/isr.h: include/rtems/score/isr.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/isr.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/isr.h

$(PROJECT_INCLUDE)/rtems/score/object.h: include/rtems/score/object.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/object.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/object.h

$(PROJECT_INCLUDE)/rtems/score/priority.h: include/rtems/score/priority.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/priority.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/priority.h

$(PROJECT_INCLUDE)/rtems/score/stack.h: include/rtems/score/stack.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/stack.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/stack.h

$(PROJECT_INCLUDE)/rtems/score/states.h: include/rtems/score/states.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/states.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/states.h

$(PROJECT_INCLUDE)/rtems/score/sysstate.h: include/rtems/score/sysstate.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/sysstate.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/sysstate.h

$(PROJECT_INCLUDE)/rtems/score/thread.h: include/rtems/score/thread.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/thread.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/thread.h

$(PROJECT_INCLUDE)/rtems/score/threadq.h: include/rtems/score/threadq.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/threadq.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/threadq.h

$(PROJECT_INCLUDE)/rtems/score/tod.h: include/rtems/score/tod.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/tod.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/tod.h

$(PROJECT_INCLUDE)/rtems/score/tqdata.h: include/rtems/score/tqdata.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/tqdata.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/tqdata.h

$(PROJECT_INCLUDE)/rtems/score/userext.h: include/rtems/score/userext.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/userext.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/userext.h

$(PROJECT_INCLUDE)/rtems/score/watchdog.h: include/rtems/score/watchdog.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/watchdog.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/watchdog.h

$(PROJECT_INCLUDE)/rtems/score/wkspace.h: include/rtems/score/wkspace.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/wkspace.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/wkspace.h

$(PROJECT_INCLUDE)/rtems/score/cpuopts.h: include/rtems/score/cpuopts.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/cpuopts.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/cpuopts.h

if HAS_MP
$(PROJECT_INCLUDE)/rtems/score/mpci.h: include/rtems/score/mpci.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/mpci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/mpci.h

$(PROJECT_INCLUDE)/rtems/score/mppkt.h: include/rtems/score/mppkt.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/mppkt.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/mppkt.h

$(PROJECT_INCLUDE)/rtems/score/objectmp.h: include/rtems/score/objectmp.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/objectmp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/objectmp.h

$(PROJECT_INCLUDE)/rtems/score/threadmp.h: include/rtems/score/threadmp.h $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/threadmp.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/threadmp.h
endif

if INLINE
$(PROJECT_INCLUDE)/rtems/score/address.inl: inline/rtems/score/address.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/address.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/address.inl

$(PROJECT_INCLUDE)/rtems/score/chain.inl: inline/rtems/score/chain.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/chain.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/chain.inl

$(PROJECT_INCLUDE)/rtems/score/coremsg.inl: inline/rtems/score/coremsg.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/coremsg.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/coremsg.inl

$(PROJECT_INCLUDE)/rtems/score/coremutex.inl: inline/rtems/score/coremutex.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/coremutex.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/coremutex.inl

$(PROJECT_INCLUDE)/rtems/score/coresem.inl: inline/rtems/score/coresem.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/coresem.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/coresem.inl

$(PROJECT_INCLUDE)/rtems/score/heap.inl: inline/rtems/score/heap.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/heap.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/heap.inl

$(PROJECT_INCLUDE)/rtems/score/isr.inl: inline/rtems/score/isr.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/isr.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/isr.inl

$(PROJECT_INCLUDE)/rtems/score/object.inl: inline/rtems/score/object.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/object.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/object.inl

$(PROJECT_INCLUDE)/rtems/score/priority.inl: inline/rtems/score/priority.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/priority.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/priority.inl

$(PROJECT_INCLUDE)/rtems/score/stack.inl: inline/rtems/score/stack.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/stack.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/stack.inl

$(PROJECT_INCLUDE)/rtems/score/states.inl: inline/rtems/score/states.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/states.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/states.inl

$(PROJECT_INCLUDE)/rtems/score/sysstate.inl: inline/rtems/score/sysstate.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/sysstate.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/sysstate.inl

$(PROJECT_INCLUDE)/rtems/score/thread.inl: inline/rtems/score/thread.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/thread.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/thread.inl

$(PROJECT_INCLUDE)/rtems/score/tod.inl: inline/rtems/score/tod.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/tod.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/tod.inl

$(PROJECT_INCLUDE)/rtems/score/tqdata.inl: inline/rtems/score/tqdata.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/tqdata.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/tqdata.inl

$(PROJECT_INCLUDE)/rtems/score/userext.inl: inline/rtems/score/userext.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/userext.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/userext.inl

$(PROJECT_INCLUDE)/rtems/score/watchdog.inl: inline/rtems/score/watchdog.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/watchdog.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/watchdog.inl

$(PROJECT_INCLUDE)/rtems/score/wkspace.inl: inline/rtems/score/wkspace.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/wkspace.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/wkspace.inl

if HAS_MP
$(PROJECT_INCLUDE)/rtems/score/mppkt.inl: inline/rtems/score/mppkt.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/mppkt.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/mppkt.inl

$(PROJECT_INCLUDE)/rtems/score/objectmp.inl: inline/rtems/score/objectmp.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/objectmp.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/objectmp.inl

$(PROJECT_INCLUDE)/rtems/score/threadmp.inl: inline/rtems/score/threadmp.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/threadmp.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/threadmp.inl
endif

else
$(PROJECT_INCLUDE)/rtems/score/address.inl: macros/rtems/score/address.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/address.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/address.inl

$(PROJECT_INCLUDE)/rtems/score/chain.inl: macros/rtems/score/chain.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/chain.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/chain.inl

$(PROJECT_INCLUDE)/rtems/score/coremsg.inl: macros/rtems/score/coremsg.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/coremsg.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/coremsg.inl

$(PROJECT_INCLUDE)/rtems/score/coremutex.inl: macros/rtems/score/coremutex.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/coremutex.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/coremutex.inl

$(PROJECT_INCLUDE)/rtems/score/coresem.inl: macros/rtems/score/coresem.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/coresem.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/coresem.inl

$(PROJECT_INCLUDE)/rtems/score/heap.inl: macros/rtems/score/heap.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/heap.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/heap.inl

$(PROJECT_INCLUDE)/rtems/score/isr.inl: macros/rtems/score/isr.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/isr.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/isr.inl

$(PROJECT_INCLUDE)/rtems/score/object.inl: macros/rtems/score/object.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/object.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/object.inl

$(PROJECT_INCLUDE)/rtems/score/priority.inl: macros/rtems/score/priority.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/priority.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/priority.inl

$(PROJECT_INCLUDE)/rtems/score/stack.inl: macros/rtems/score/stack.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/stack.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/stack.inl

$(PROJECT_INCLUDE)/rtems/score/states.inl: macros/rtems/score/states.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/states.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/states.inl

$(PROJECT_INCLUDE)/rtems/score/sysstate.inl: macros/rtems/score/sysstate.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/sysstate.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/sysstate.inl

$(PROJECT_INCLUDE)/rtems/score/thread.inl: macros/rtems/score/thread.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/thread.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/thread.inl

$(PROJECT_INCLUDE)/rtems/score/tod.inl: macros/rtems/score/tod.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/tod.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/tod.inl

$(PROJECT_INCLUDE)/rtems/score/tqdata.inl: macros/rtems/score/tqdata.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/tqdata.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/tqdata.inl

$(PROJECT_INCLUDE)/rtems/score/userext.inl: macros/rtems/score/userext.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/userext.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/userext.inl

$(PROJECT_INCLUDE)/rtems/score/watchdog.inl: macros/rtems/score/watchdog.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/watchdog.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/watchdog.inl

$(PROJECT_INCLUDE)/rtems/score/wkspace.inl: macros/rtems/score/wkspace.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/wkspace.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/wkspace.inl

if HAS_MP
$(PROJECT_INCLUDE)/rtems/score/mppkt.inl: macros/rtems/score/mppkt.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/mppkt.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/mppkt.inl

$(PROJECT_INCLUDE)/rtems/score/objectmp.inl: macros/rtems/score/objectmp.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/objectmp.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/objectmp.inl

$(PROJECT_INCLUDE)/rtems/score/threadmp.inl: macros/rtems/score/threadmp.inl $(PROJECT_INCLUDE)/rtems/score/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/rtems/score/threadmp.inl
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score/threadmp.inl
endif
endif

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
