##
## $Id$
##

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

SUBDIRS = . cpu

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/score:
	@$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: include/%.h
	$(INSTALL_DATA) $< $@
$(PROJECT_INCLUDE)/%.inl: $(INLINEdir)/%.inl
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE)
## include

include_rtemsdir = $(includedir)/rtems
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems

include_rtems_HEADERS = include/rtems/debug.h include/rtems/system.h \
    include/rtems/seterr.h

PREINSTALL_FILES += $(include_rtems_HEADERS:include/%=$(PROJECT_INCLUDE)/%)

include_rtems_scoredir = $(includedir)/rtems/score
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score

# We only build multiprocessing related files if HAS_MP was defined
MP_H_FILES = include/rtems/score/mpci.h include/rtems/score/mppkt.h \
    include/rtems/score/objectmp.h include/rtems/score/threadmp.h

# H_FILES that get installed in the rtems/score subdirectoy
STD_H_FILES = include/rtems/score/address.h include/rtems/score/apiext.h \
    include/rtems/score/apimutex.h \
    include/rtems/score/bitfield.h include/rtems/score/chain.h include/rtems/score/context.h \
    include/rtems/score/copyrt.h include/rtems/score/coremsg.h include/rtems/score/coremutex.h \
    include/rtems/score/coresem.h include/rtems/score/heap.h include/rtems/score/interr.h \
    include/rtems/score/isr.h include/rtems/score/object.h include/rtems/score/priority.h \
    include/rtems/score/stack.h include/rtems/score/states.h include/rtems/score/sysstate.h \
    include/rtems/score/thread.h include/rtems/score/threadq.h include/rtems/score/tod.h \
    include/rtems/score/tqdata.h include/rtems/score/userext.h include/rtems/score/watchdog.h \
    include/rtems/score/wkspace.h include/rtems/score/cpuopts.h
include_rtems_score_HEADERS = $(STD_H_FILES)
PREINSTALL_FILES += $(STD_H_FILES:include/%.h=$(PROJECT_INCLUDE)/%.h)

if HAS_MP
include_rtems_score_HEADERS += $(MP_H_FILES)
PREINSTALL_FILES += $(MP_H_FILES:include/%.h=$(PROJECT_INCLUDE)/%.h)
endif

## inline

## We only build multiprocessing related files if HAS_MP was defined
MP_I_FILES = inline/rtems/score/mppkt.inl inline/rtems/score/objectmp.inl inline/rtems/score/threadmp.inl
if INLINE
if HAS_MP
include_rtems_score_HEADERS += $(STD_I_FILES)
PREINSTALL_FILES += $(MP_I_FILES:inline/%.inl=$(PROJECT_INCLUDE)/%.inl)
endif
endif

STD_I_FILES = inline/rtems/score/address.inl inline/rtems/score/chain.inl \
    inline/rtems/score/coremsg.inl inline/rtems/score/coremutex.inl inline/rtems/score/coresem.inl \
    inline/rtems/score/heap.inl inline/rtems/score/isr.inl inline/rtems/score/object.inl \
    inline/rtems/score/priority.inl inline/rtems/score/stack.inl inline/rtems/score/states.inl \
    inline/rtems/score/sysstate.inl inline/rtems/score/thread.inl inline/rtems/score/tod.inl \
    inline/rtems/score/tqdata.inl inline/rtems/score/userext.inl inline/rtems/score/watchdog.inl \
    inline/rtems/score/wkspace.inl
if INLINE
include_rtems_score_HEADERS += $(STD_I_FILES)
PREINSTALL_FILES += $(STD_I_FILES:inline/%.inl=$(PROJECT_INCLUDE)/%.inl)
endif

EXTRA_DIST = $(STD_I_FILES) $(MP_I_FILES)

## macros

## We only build multiprocessing related files if HAS_MP was defined
MP_M_FILES = macros/rtems/score/mppkt.inl macros/rtems/score/objectmp.inl macros/rtems/score/threadmp.inl
if MACROS
if HAS_MP
include_rtems_score_HEADERS += $(MP_M_FILES)
PREINSTALL_FILES += $(MP_M_FILES:macros/%.inl=$(PROJECT_INCLUDE)/%.inl)
endif
endif

STD_M_FILES = macros/rtems/score/address.inl macros/rtems/score/chain.inl \
    macros/rtems/score/coremsg.inl macros/rtems/score/coremutex.inl macros/rtems/score/coresem.inl \
    macros/rtems/score/heap.inl macros/rtems/score/isr.inl macros/rtems/score/object.inl \
    macros/rtems/score/priority.inl macros/rtems/score/stack.inl macros/rtems/score/states.inl \
    macros/rtems/score/sysstate.inl macros/rtems/score/thread.inl macros/rtems/score/tod.inl \
    macros/rtems/score/tqdata.inl macros/rtems/score/userext.inl macros/rtems/score/watchdog.inl \
    macros/rtems/score/wkspace.inl
if MACROS
include_rtems_score_HEADERS += $(STD_M_FILES)
PREINSTALL_FILES += $(MP_M_FILES:inline/%.inl=$(PROJECT_INCLUDE)/%.inl)
endif

EXTRA_DIST += $(STD_M_FILES) $(MP_M_FILES) macros/README

## src

LIB=$(ARCH)/libscore.a

# We only build multiprocessing related files if HAS_MP was defined
MP_C_FILES = src/mpci.c src/objectmp.c src/threadmp.c

CORE_MESSAGE_QUEUE_C_FILES = src/coremsg.c src/coremsgbroadcast.c src/coremsgclose.c \
    src/coremsgflush.c src/coremsgflushwait.c src/coremsginsert.c src/coremsgflushsupp.c \
    src/coremsgseize.c src/coremsgsubmit.c

CORE_MUTEX_C_FILES = src/coremutex.c src/coremutexflush.c src/coremutexseize.c \
    src/coremutexsurrender.c

CORE_SEMAPHORE_C_FILES = src/coresem.c src/coresemflush.c src/coresemseize.c \
    src/coresemsurrender.c

HEAP_C_FILES = src/heap.c src/heapallocate.c src/heapextend.c src/heapfree.c \
    src/heapsizeofuserarea.c src/heapwalk.c src/heapgetinfo.c

OBJECT_C_FILES = src/object.c src/objectallocate.c src/objectallocatebyindex.c \
    src/objectclearname.c src/objectcomparenameraw.c src/objectcomparenamestring.c \
    src/objectcopynameraw.c src/objectcopynamestring.c src/objectextendinformation.c \
    src/objectfree.c src/objectget.c src/objectgetisr.c src/objectgetbyindex.c \
    src/objectgetnext.c src/objectinitializeinformation.c src/objectnametoid.c \
    src/objectshrinkinformation.c src/objectgetnoprotection.c

THREAD_C_FILES = src/thread.c src/threadchangepriority.c src/threadclearstate.c \
    src/threadclose.c src/threadcreateidle.c src/threaddelayended.c src/threaddispatch.c \
    src/threadevaluatemode.c src/threadget.c src/threadhandler.c src/threadidlebody.c \
    src/threadinitialize.c src/threadloadenv.c src/threadready.c src/threadresettimeslice.c \
    src/threadreset.c src/threadrestart.c src/threadresume.c src/threadrotatequeue.c \
    src/threadsetpriority.c src/threadsetstate.c src/threadsettransient.c \
    src/threadstackallocate.c src/threadstackfree.c src/threadstart.c \
    src/threadstartmultitasking.c src/threadsuspend.c src/threadtickletimeslice.c \
    src/threadyieldprocessor.c

THREADQ_C_FILES = src/threadq.c src/threadqdequeue.c src/threadqdequeuefifo.c \
    src/threadqdequeuepriority.c src/threadqenqueue.c src/threadqenqueuefifo.c \
    src/threadqenqueuepriority.c src/threadqextract.c src/threadqextractfifo.c \
    src/threadqextractpriority.c src/threadqextractwithproxy.c src/threadqfirst.c \
    src/threadqfirstfifo.c src/threadqfirstpriority.c src/threadqflush.c src/threadqtimeout.c

TOD_C_FILES = src/coretod.c src/coretodset.c src/coretodtickle.c src/coretodtoseconds.c \
    src/coretodvalidate.c

WATCHDOG_C_FILES = src/watchdog.c src/watchdogadjust.c src/watchdoginsert.c \
    src/watchdogremove.c src/watchdogtickle.c

STD_C_FILES = src/apiext.c src/chain.c $(CORE_MESSAGE_QUEUE_C_FILES) \
    $(CORE_MUTEX_C_FILES) $(CORE_SEMAPHORE_C_FILES) $(HEAP_C_FILES) src/interr.c \
    src/isr.c $(OBJECT_C_FILES) $(THREAD_C_FILES) $(THREADQ_C_FILES) \
    $(TOD_C_FILES) src/userext.c $(WATCHDOG_C_FILES) src/wkspace.c

C_FILES = $(STD_C_FILES)
if HAS_MP
C_FILES += $(MP_C_FILES)
endif
OBJS = $(C_FILES:src/%.c=${ARCH}/%.$(OBJEXT))

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D__RTEMS_INSIDE__

$(LIB): ${OBJS}
	$(make-library)

${ARCH}/%.$(OBJEXT): src/%.c
	test -d $(ARCH) || mkdir $(ARCH)
	${COMPILE} -o $@ -c $<

EXTRA_DIST += $(STD_C_FILES) $(MP_C_FILES) src/Unlimited.txt

all-local: $(PREINSTALL_FILES) ${ARCH} $(LIB)

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
