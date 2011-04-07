##
##  $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

lib_LIBRARIES = librtems.a

cpukitdir=$(srcdir)/../../../cpukit
librtems_a_CPPFLAGS  = -D__RTEMS_VIOLATE_KERNEL_VISIBILITY__
librtems_a_CPPFLAGS += -I$(srcdir)/sched_cpu
librtems_a_CPPFLAGS += -I$(cpukitdir)/include
librtems_a_CPPFLAGS += -I$(cpukitdir)/score/include
librtems_a_CPPFLAGS += -I$(cpukitdir)/score/inline
librtems_a_CPPFLAGS += -I$(cpukitdir)/rtems/include
librtems_a_CPPFLAGS += -I$(cpukitdir)/rtems/inline
librtems_a_CPPFLAGS += -I$(cpukitdir)/sapi/include
librtems_a_CPPFLAGS += -I$(cpukitdir)/sapi/inline
librtems_a_CPPFLAGS += -I$(cpukitdir)/libcsupport/include
librtems_a_CPPFLAGS += -I$(cpukitdir)/libmisc/stringto

## Have own version of these
librtems_a_SOURCES = rtems_init.c wkspace.c
librtems_a_SOURCES += sched_cpu/cpu_asm.c
librtems_a_SOURCES += sched_cpu/cpu.c

## CPUKIT/SCORE/SRC
librtems_a_SOURCES += $(cpukitdir)/score/src/interr.c
librtems_a_SOURCES += $(cpukitdir)/score/src/percpu.c
librtems_a_SOURCES += $(cpukitdir)/score/src/apiext.c
librtems_a_SOURCES += $(cpukitdir)/score/src/apimutexallocate.c
librtems_a_SOURCES += $(cpukitdir)/score/src/apimutex.c
librtems_a_SOURCES += $(cpukitdir)/score/src/apimutexlock.c
librtems_a_SOURCES += $(cpukitdir)/score/src/apimutexunlock.c
librtems_a_SOURCES += $(cpukitdir)/score/src/chainappend.c
librtems_a_SOURCES += $(cpukitdir)/score/src/chain.c
librtems_a_SOURCES += $(cpukitdir)/score/src/chainextract.c
librtems_a_SOURCES += $(cpukitdir)/score/src/chainget.c
librtems_a_SOURCES += $(cpukitdir)/score/src/chaininsert.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coremutex.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coremutexflush.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coremutexseize.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coremutexseizeintr.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coremutexsurrender.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coresem.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coresemflush.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coresemseize.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coresemsurrender.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coretod.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coretodget.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coretodgetuptime.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coretodgetuptimetimespec.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coretodmsecstoticks.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coretodset.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coretodtickle.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coretodtickspersec.c
librtems_a_SOURCES += $(cpukitdir)/score/src/coretodusectoticks.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectallocate.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectapimaximumclass.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectclose.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectextendinformation.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectfree.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectget.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectgetinfo.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectgetinfoid.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectgetisr.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectgetnameasstring.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectgetnext.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectgetnoprotection.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectidtoname.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectinitializeinformation.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectnamespaceremove.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectnametoid.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectnametoidstring.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectsetname.c
librtems_a_SOURCES += $(cpukitdir)/score/src/objectshrinkinformation.c
librtems_a_SOURCES += $(cpukitdir)/score/src/scheduler.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulerpriorityallocate.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulerpriorityblock.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulerpriority.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulerpriorityenqueue.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulerpriorityenqueuefirst.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulerpriorityextract.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulerpriorityfree.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulerpriorityschedule.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulerpriorityunblock.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulerpriorityupdate.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulerpriorityyield.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulersimpleblock.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulersimple.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulersimpleenqueue.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulersimpleenqueuefirst.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulersimpleextract.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulersimplereadyqueueenqueue.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulersimplereadyqueueenqueuefirst.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulersimpleschedule.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulersimpleunblock.c
librtems_a_SOURCES += $(cpukitdir)/score/src/schedulersimpleyield.c
librtems_a_SOURCES += $(cpukitdir)/score/src/thread.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadchangepriority.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadclearstate.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadclose.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadcreateidle.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threaddelayended.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threaddispatch.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadget.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadhandler.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadinitialize.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadloadenv.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadready.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadreset.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadrestart.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadsetpriority.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadsetstate.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadsettransient.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadstackallocate.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadstackfree.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadstart.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadstartmultitasking.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadtickletimeslice.c
librtems_a_SOURCES += $(cpukitdir)/score/src/iterateoverthreads.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadq.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadblockingoperationcancel.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqdequeue.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqdequeuefifo.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqdequeuepriority.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqenqueue.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqenqueuefifo.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqenqueuepriority.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqextract.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqextractfifo.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqextractpriority.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqextractwithproxy.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqfirst.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqfirstfifo.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqfirstpriority.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqflush.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqprocesstimeout.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqrequeue.c
librtems_a_SOURCES += $(cpukitdir)/score/src/threadqtimeout.c
librtems_a_SOURCES += $(cpukitdir)/score/src/timespecaddto.c
librtems_a_SOURCES += $(cpukitdir)/score/src/timespecdividebyinteger.c
librtems_a_SOURCES += $(cpukitdir)/score/src/timespecdivide.c
librtems_a_SOURCES += $(cpukitdir)/score/src/timespecfromticks.c
librtems_a_SOURCES += $(cpukitdir)/score/src/timespecgreaterthan.c
librtems_a_SOURCES += $(cpukitdir)/score/src/timespecisvalid.c
librtems_a_SOURCES += $(cpukitdir)/score/src/timespeclessthan.c
librtems_a_SOURCES += $(cpukitdir)/score/src/timespecsubtract.c
librtems_a_SOURCES += $(cpukitdir)/score/src/timespectoticks.c
librtems_a_SOURCES += $(cpukitdir)/score/src/userextaddset.c
librtems_a_SOURCES += $(cpukitdir)/score/src/userext.c
librtems_a_SOURCES += $(cpukitdir)/score/src/userextremoveset.c
librtems_a_SOURCES += $(cpukitdir)/score/src/userextthreadbegin.c
librtems_a_SOURCES += $(cpukitdir)/score/src/userextthreadcreate.c
librtems_a_SOURCES += $(cpukitdir)/score/src/userextthreaddelete.c
librtems_a_SOURCES += $(cpukitdir)/score/src/userextthreadrestart.c
librtems_a_SOURCES += $(cpukitdir)/score/src/userextthreadstart.c
librtems_a_SOURCES += $(cpukitdir)/score/src/userextthreadswitch.c
librtems_a_SOURCES += $(cpukitdir)/score/src/watchdogadjust.c
librtems_a_SOURCES += $(cpukitdir)/score/src/watchdogadjusttochain.c
librtems_a_SOURCES += $(cpukitdir)/score/src/watchdog.c
librtems_a_SOURCES += $(cpukitdir)/score/src/watchdoginsert.c
librtems_a_SOURCES += $(cpukitdir)/score/src/watchdognanoseconds.c
librtems_a_SOURCES += $(cpukitdir)/score/src/watchdogremove.c
librtems_a_SOURCES += $(cpukitdir)/score/src/watchdogreport.c
librtems_a_SOURCES += $(cpukitdir)/score/src/watchdogreportchain.c
librtems_a_SOURCES += $(cpukitdir)/score/src/watchdogtickle.c
librtems_a_SOURCES += $(cpukitdir)/sapi/src/debug.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/clockgetsecondssinceepoch.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/clockgettickspersecond.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/clockgettickssinceboot.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/clockgettod.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/clockgettodtimeval.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/clockgetuptime.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/clockset.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/clocksetnsecshandler.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/clocktick.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/clocktodtoseconds.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/clocktodvalidate.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/sem.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/semcreate.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/semdata.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/semdelete.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/semflush.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/semident.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/semobtain.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/semrelease.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/semtranslatereturncode.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/tasks.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/taskdata.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/taskmode.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/taskvariable_invoke_dtor.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/taskcreate.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/taskstart.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/taskident.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/taskdelete.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/taskresume.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/tasksuspend.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/tasksetpriority.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/taskmode.c
librtems_a_SOURCES += $(cpukitdir)/rtems/src/taskwakeafter.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assoclocalbyname.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assoclocalbyremotebitfield.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assoclocalbyremote.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assocnamebad.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assocnamebylocalbitfield.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assocnamebylocal.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assocnamebyremotebitfield.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assocnamebyremote.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assocptrbylocal.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assocptrbyname.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assocptrbyremote.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assocremotebylocalbitfield.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assocremotebylocal.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/assocremotebyname.c
librtems_a_SOURCES += $(cpukitdir)/libcsupport/src/error.c
librtems_a_SOURCES += $(cpukitdir)/libmisc/stringto/stringtodouble.c
librtems_a_SOURCES += $(cpukitdir)/libmisc/stringto/stringtofloat.c
librtems_a_SOURCES += $(cpukitdir)/libmisc/stringto/stringtoint.c
librtems_a_SOURCES += $(cpukitdir)/libmisc/stringto/stringtolong.c
librtems_a_SOURCES += $(cpukitdir)/libmisc/stringto/stringtopointer.c
librtems_a_SOURCES += $(cpukitdir)/libmisc/stringto/stringtounsignedchar.c
librtems_a_SOURCES += $(cpukitdir)/libmisc/stringto/stringtounsignedint.c
librtems_a_SOURCES += $(cpukitdir)/libmisc/stringto/stringtounsignedlong.c

schedsim_includedir = $(includedir)/schedsim
schedsim_rtems_includedir = $(includedir)/schedsim/rtems
schedsim_rtems_score_includedir = $(includedir)/schedsim/rtems/score
schedsim_rtems_rtems_includedir = $(includedir)/schedsim/rtems/rtems

schedsim_include_HEADERS  = \
 ${cpukitdir}/rtems/include/rtems.h

schedsim_rtems_include_HEADERS  = \
  ${cpukitdir}/score/include//rtems/seterr.h \
  ${cpukitdir}/score/include/rtems/system.h \
  ${cpukitdir}/score/include/rtems/debug.h \
  ${cpukitdir}/score/include/rtems/bspsmp.h \
  ${cpukitdir}/include/rtems/bspIo.h \
  $(cpukitdir)/sapi/include/rtems/fatal.h \
  $(cpukitdir)/sapi/include/rtems/init.h \
  $(cpukitdir)/sapi/include/rtems/config.h \
  $(cpukitdir)/sapi/include/rtems/sptables.h \
  $(cpukitdir)/sapi/include/rtems/io.h \
  $(cpukitdir)/sapi/include/rtems/mptables.h \
  $(cpukitdir)/sapi/include/rtems/extension.h \
  $(cpukitdir)/sapi/include/rtems/chain.h \
  $(cpukitdir)/sapi/include/confdefs.h \
  $(cpukitdir)/libcsupport/include/rtems/libio_.h \
  $(cpukitdir)/libcsupport/include/rtems/watchdogdrv.h \
  $(cpukitdir)/libcsupport/include/rtems/error.h \
  $(cpukitdir)/libcsupport/include/rtems/framebuffer.h \
  $(cpukitdir)/libcsupport/include/rtems/assoc.h \
  $(cpukitdir)/libcsupport/include/rtems/libio.h \
  $(cpukitdir)/libcsupport/include/rtems/gxx_wrappers.h \
  $(cpukitdir)/libcsupport/include/rtems/libcsupport.h \
  $(cpukitdir)/libcsupport/include/rtems/termiostypes.h \
  $(cpukitdir)/libcsupport/include/rtems/malloc.h \
  sched_cpu/rtems/stringto.h \
  sched_cpu/rtems/asm.h 

schedsim_rtems_score_include_HEADERS = \
  ${cpukitdir}/score/include/rtems/score/address.h \
  ${cpukitdir}/score/include/rtems/score/coremutex.h \
  ${cpukitdir}/score/include/rtems/score/corerwlock.h \
  ${cpukitdir}/score/include/rtems/score/threadsync.h \
  ${cpukitdir}/score/include/rtems/score/priority.h \
  ${cpukitdir}/score/include/rtems/score/sysstate.h \
  ${cpukitdir}/score/include/rtems/score/threadmp.h \
  ${cpukitdir}/score/include/rtems/score/mppkt.h \
  ${cpukitdir}/score/include/rtems/score/stack.h \
  ${cpukitdir}/score/include/rtems/score/context.h \
  ${cpukitdir}/score/include/rtems/score/heap.h \
  ${cpukitdir}/score/include/rtems/score/coresem.h \
  ${cpukitdir}/score/include/rtems/score/corebarrier.h \
  ${cpukitdir}/score/include/rtems/score/corespinlock.h \
  ${cpukitdir}/score/include/rtems/score/prioritybitmap.h \
  ${cpukitdir}/score/include/rtems/score/timestamp.h \
  ${cpukitdir}/score/include/rtems/score/basedefs.h \
  ${cpukitdir}/score/include/rtems/score/wkspace.h \
  ${cpukitdir}/score/include/rtems/score/percpu.h \
  ${cpukitdir}/score/include/rtems/score/watchdog.h \
  ${cpukitdir}/score/include/rtems/score/tod.h \
  ${cpukitdir}/score/include/rtems/score/protectedheap.h \
  ${cpukitdir}/score/include/rtems/score/timestamp64.h \
  ${cpukitdir}/score/include/rtems/score/chain.h \
  ${cpukitdir}/score/include/rtems/score/apiext.h \
  ${cpukitdir}/score/include/rtems/score/userext.h \
  ${cpukitdir}/score/include/rtems/score/threadq.h \
  ${cpukitdir}/score/include/rtems/score/apimutex.h \
  ${cpukitdir}/score/include/rtems/score/scheduler.h \
  ${cpukitdir}/score/include/rtems/score/isr.h \
  ${cpukitdir}/score/include/rtems/score/mpci.h \
  ${cpukitdir}/score/include/rtems/score/object.h \
  ${cpukitdir}/score/include/rtems/score/copyrt.h \
  ${cpukitdir}/score/include/rtems/score/states.h \
  ${cpukitdir}/score/include/rtems/score/objectmp.h \
  ${cpukitdir}/score/include/rtems/score/thread.h \
  ${cpukitdir}/score/include/rtems/score/coremsg.h \
  ${cpukitdir}/score/include/rtems/score/schedulerpriority.h \
  ${cpukitdir}/score/include/rtems/score/timespec.h \
  ${cpukitdir}/score/include/rtems/score/tqdata.h \
  ${cpukitdir}/score/include/rtems/score/bitfield.h \
  ${cpukitdir}/score/include/rtems/score/interr.h \
  $(cpukitdir)/score/inline/rtems/score/address.inl \
  $(cpukitdir)/score/inline/rtems/score/corespinlock.inl \
  $(cpukitdir)/score/inline/rtems/score/priority.inl \
  $(cpukitdir)/score/inline/rtems/score/threadmp.inl \
  $(cpukitdir)/score/inline/rtems/score/chain.inl \
  $(cpukitdir)/score/inline/rtems/score/heap.inl \
  $(cpukitdir)/score/inline/rtems/score/scheduler.inl \
  $(cpukitdir)/score/inline/rtems/score/threadq.inl \
  $(cpukitdir)/score/inline/rtems/score/corebarrier.inl \
  $(cpukitdir)/score/inline/rtems/score/isr.inl \
  $(cpukitdir)/score/inline/rtems/score/schedulerpriority.inl \
  $(cpukitdir)/score/inline/rtems/score/tod.inl \
  $(cpukitdir)/score/inline/rtems/score/coremsg.inl \
  $(cpukitdir)/score/inline/rtems/score/mppkt.inl \
  $(cpukitdir)/score/inline/rtems/score/stack.inl \
  $(cpukitdir)/score/inline/rtems/score/tqdata.inl \
  $(cpukitdir)/score/inline/rtems/score/coremutex.inl \
  $(cpukitdir)/score/inline/rtems/score/object.inl \
  $(cpukitdir)/score/inline/rtems/score/states.inl \
  $(cpukitdir)/score/inline/rtems/score/watchdog.inl \
  $(cpukitdir)/score/inline/rtems/score/corerwlock.inl \
  $(cpukitdir)/score/inline/rtems/score/objectmp.inl \
  $(cpukitdir)/score/inline/rtems/score/sysstate.inl \
  $(cpukitdir)/score/inline/rtems/score/wkspace.inl \
  $(cpukitdir)/score/inline/rtems/score/coresem.inl \
  $(cpukitdir)/score/inline/rtems/score/prioritybitmap.inl \
  $(cpukitdir)/score/inline/rtems/score/thread.inl \
  sched_cpu/rtems/score/cpu_asm.h \
  sched_cpu/rtems/score/cpuopts.h \
  sched_cpu/rtems/score/cpu.h \
  sched_cpu/rtems/score/types.h \
  sched_cpu/rtems/score/no_cpu.h 


schedsim_rtems_rtems_include_HEADERS = \
  ${cpukitdir}/rtems/include/rtems/rtems/event.h \
  ${cpukitdir}/rtems/include/rtems/rtems/cache.h \
  ${cpukitdir}/rtems/include/rtems/rtems/timer.h \
  ${cpukitdir}/rtems/include/rtems/rtems/regionmp.h \
  ${cpukitdir}/rtems/include/rtems/rtems/config.h \
  ${cpukitdir}/rtems/include/rtems/rtems/ratemon.h \
  ${cpukitdir}/rtems/include/rtems/rtems/rtemsapi.h \
  ${cpukitdir}/rtems/include/rtems/rtems/eventset.h \
  ${cpukitdir}/rtems/include/rtems/rtems/mp.h \
  ${cpukitdir}/rtems/include/rtems/rtems/dpmem.h \
  ${cpukitdir}/rtems/include/rtems/rtems/status.h \
  ${cpukitdir}/rtems/include/rtems/rtems/sem.h \
  ${cpukitdir}/rtems/include/rtems/rtems/barriermp.h \
  ${cpukitdir}/rtems/include/rtems/rtems/types.h \
  ${cpukitdir}/rtems/include/rtems/rtems/intr.h \
  ${cpukitdir}/rtems/include/rtems/rtems/signalmp.h \
  ${cpukitdir}/rtems/include/rtems/rtems/eventmp.h \
  ${cpukitdir}/rtems/include/rtems/rtems/part.h \
  ${cpukitdir}/rtems/include/rtems/rtems/message.h \
  ${cpukitdir}/rtems/include/rtems/rtems/region.h \
  ${cpukitdir}/rtems/include/rtems/rtems/object.h \
  ${cpukitdir}/rtems/include/rtems/rtems/partmp.h \
  ${cpukitdir}/rtems/include/rtems/rtems/msgmp.h \
  ${cpukitdir}/rtems/include/rtems/rtems/clock.h \
  ${cpukitdir}/rtems/include/rtems/rtems/modes.h \
  ${cpukitdir}/rtems/include/rtems/rtems/taskmp.h \
  ${cpukitdir}/rtems/include/rtems/rtems/asr.h \
  ${cpukitdir}/rtems/include/rtems/rtems/semmp.h \
  ${cpukitdir}/rtems/include/rtems/rtems/options.h \
  ${cpukitdir}/rtems/include/rtems/rtems/attr.h \
  ${cpukitdir}/rtems/include/rtems/rtems/support.h \
  ${cpukitdir}/rtems/include/rtems/rtems/signal.h \
  ${cpukitdir}/rtems/include/rtems/rtems/barrier.h \
  ${cpukitdir}/rtems/include/rtems/rtems/tasks.h \
  $(cpukitdir)/rtems/inline/rtems/rtems/asr.inl  \
  $(cpukitdir)/rtems/inline/rtems/rtems/eventset.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/ratemon.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/tasks.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/attr.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/message.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/region.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/timer.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/barrier.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/modes.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/sem.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/dpmem.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/options.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/status.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/event.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/part.inl \
  $(cpukitdir)/rtems/inline/rtems/rtems/support.inl

schedsim_include_HEADERS += rtems_sched.h

include $(top_srcdir)/../../automake/host.am
