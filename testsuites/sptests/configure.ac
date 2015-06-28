## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-sptests],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
AC_CONFIG_SRCDIR([sp01])
AC_CONFIG_HEADER([config.h])
RTEMS_TOP([../..],[..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_CXX
RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_RTEMS_TEST_NO_PAUSE

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET
RTEMS_PROG_CXX_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_CHECK_CXX(RTEMS_BSP)

AM_CONDITIONAL([HAS_CPLUSPLUS],[test $HAS_CPLUSPLUS = "yes"])

# FIXME: We should get rid of this. It's a cludge.
AC_CHECK_SIZEOF([time_t])

AC_CHECK_TYPES([struct _Thread_queue_Queue],[],[],[#include <sys/lock.h>])
AM_CONDITIONAL(HAS__THREAD_QUEUE_QUEUE,test x"${ac_cv_type_struct__Thread_queue_Queue}" = x"yes")

# Added to newlib pthreads for RTEMS SMP (np), may not be present
AC_CHECK_HEADERS([sys/cpuset.h])
AM_CONDITIONAL(HAS_CPUSET,test x"${ac_cv_header_sys_cpuset_h}" = x"yes")

# Some tests should not be built/run in SMP configurations
RTEMS_CHECK_CPUOPTS([RTEMS_SMP])
AM_CONDITIONAL(HAS_SMP,test "$rtems_cv_RTEMS_SMP" = "yes")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
spsyslock01/Makefile
sptasknopreempt01/Makefile
spintrcritical23/Makefile
sptimecounter01/Makefile
sptimecounter02/Makefile
sptimecounter03/Makefile
spatomic01/Makefile
spglobalcon01/Makefile
spintrcritical22/Makefile
spsem03/Makefile
spresource01/Makefile
spmrsp01/Makefile
spscheduler01/Makefile
spfatal28/Makefile
spthreadlife01/Makefile
spprofiling01/Makefile
spcache01/Makefile
sptls03/Makefile
spcpucounter01/Makefile
sptls02/Makefile
sptls01/Makefile
spintrcritical20/Makefile
spintrcritical21/Makefile
spcontext01/Makefile
spfatal26/Makefile
spinternalerror02/Makefile
spinternalerror01/Makefile
speventsystem01/Makefile
speventtransient01/Makefile
spintrcritical18/Makefile
sp01/Makefile
sp02/Makefile
sp03/Makefile
sp04/Makefile
sp05/Makefile
sp06/Makefile
sp07/Makefile
sp08/Makefile
sp10/Makefile
sp11/Makefile
sp12/Makefile
sp13/Makefile
sp14/Makefile
sp15/Makefile
sp16/Makefile
sp17/Makefile
sp18/Makefile
sp19/Makefile
sp20/Makefile
sp21/Makefile
sp22/Makefile
sp23/Makefile
sp24/Makefile
sp25/Makefile
sp26/Makefile
sp27/Makefile
sp27a/Makefile
sp28/Makefile
sp29/Makefile
sp30/Makefile
sp31/Makefile
sp32/Makefile
sp33/Makefile
sp34/Makefile
sp35/Makefile
sp36/Makefile
sp37/Makefile
sp38/Makefile
sp40/Makefile
sp41/Makefile
sp42/Makefile
sp43/Makefile
sp44/Makefile
sp45/Makefile
sp46/Makefile
sp47/Makefile
sp48/Makefile
sp49/Makefile
sp50/Makefile
sp51/Makefile
sp52/Makefile
sp53/Makefile
sp54/Makefile
sp55/Makefile
sp56/Makefile
sp57/Makefile
sp58/Makefile
sp59/Makefile
sp60/Makefile
sp62/Makefile
sp63/Makefile
sp64/Makefile
sp65/Makefile
sp66/Makefile
sp67/Makefile
sp68/Makefile
sp69/Makefile
sp70/Makefile
sp71/Makefile
sp72/Makefile
sp73/Makefile
sp74/Makefile
sp75/Makefile
sp76/Makefile
sp77/Makefile
sp2038/Makefile
spassoc01/Makefile
spcbssched01/Makefile
spcbssched02/Makefile
spcbssched03/Makefile
spchain/Makefile
spclockget/Makefile
spclock_err01/Makefile
spclock_err02/Makefile
spcoverage/Makefile
spedfsched01/Makefile
spedfsched02/Makefile
spedfsched03/Makefile
sperror01/Makefile
sperror02/Makefile
sperror03/Makefile
spevent_err03/Makefile
spfatal01/Makefile
spfatal02/Makefile
spfatal03/Makefile
spfatal04/Makefile
spfatal05/Makefile
spfatal06/Makefile
spfatal07/Makefile
spfatal08/Makefile
spfatal09/Makefile
spfatal10/Makefile
spfatal11/Makefile
spfatal12/Makefile
spfatal13/Makefile
spfatal14/Makefile
spfatal15/Makefile
spfatal16/Makefile
spfatal17/Makefile
spfatal18/Makefile
spfatal19/Makefile
spfatal20/Makefile
spfatal24/Makefile
spfatal25/Makefile
spfatal27/Makefile
spfifo01/Makefile
spfifo02/Makefile
spfifo03/Makefile
spfifo04/Makefile
spfifo05/Makefile
spfreechain01/Makefile
spintr_err01/Makefile
spintrcritical01/Makefile
spintrcritical02/Makefile
spintrcritical03/Makefile
spintrcritical04/Makefile
spintrcritical05/Makefile
spintrcritical06/Makefile
spintrcritical07/Makefile
spintrcritical08/Makefile
spintrcritical09/Makefile
spintrcritical10/Makefile
spintrcritical11/Makefile
spintrcritical12/Makefile
spintrcritical13/Makefile
spintrcritical14/Makefile
spintrcritical15/Makefile
spintrcritical16/Makefile
spintrcritical17/Makefile
spheapprot/Makefile
spmkdir/Makefile
spmountmgr01/Makefile
spmsgq_err01/Makefile
spmsgq_err02/Makefile
spnotepad01/Makefile
spnotepad02/Makefile
spnsext01/Makefile
spobjgetnext/Makefile
sppagesize/Makefile
spport_err01/Makefile
spprintk/Makefile
spprivenv01/Makefile
spqreslib/Makefile
spratemon_err01/Makefile
sprbtree01/Makefile
spsem_err01/Makefile
spsem_err02/Makefile
spsem01/Makefile
spsem02/Makefile
spsignal_err01/Makefile
spsimplesched01/Makefile
spsimplesched02/Makefile
spsimplesched03/Makefile
spsize/Makefile
spstkalloc/Makefile
spstkalloc02/Makefile
spthreadq01/Makefile
sptimespec01/Makefile
spwatchdog/Makefile
spwkspace/Makefile
sptask_err01/Makefile
sptask_err02/Makefile
sptask_err03/Makefile
sptask_err04/Makefile
sptimer_err01/Makefile
sptimer_err02/Makefile
spcpuset01/Makefile
spregion_err01/Makefile
sppartition_err01/Makefile
])
AC_OUTPUT
