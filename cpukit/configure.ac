## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-cpukit],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
AC_CONFIG_SRCDIR([score])
RTEMS_TOP([..],[])
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define nostdinc subdir-objects foreign 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_MULTIPROCESSING
RTEMS_ENABLE_POSIX
RTEMS_ENABLE_RTEMS_DEBUG
RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_PARAVIRT
RTEMS_ENABLE_PROFILING
RTEMS_ENABLE_DRVMGR

RTEMS_ENV_RTEMSCPU
RTEMS_CHECK_RTEMS_DEBUG

AC_DEFUN([RTEMS_TOOL_CHAIN_ERROR],AC_MSG_ERROR([please update your tool chain via the RSB <https://docs.rtems.org/branches/master/rsb/quick-start.html>]))

# Is this a supported CPU?
AC_MSG_CHECKING([if cpu $RTEMS_CPU is supported])
if test -d "$srcdir/score/cpu/$RTEMS_CPU"; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_ERROR(no)
fi

RTEMS_PROG_CC_FOR_TARGET
AM_PROG_CC_C_O
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS
AC_PROG_RANLIB

AC_MSG_CHECKING([for a need to avoid a long argument list])

hack_to_avoid_long_arg_list=no
case "${build_os}" in
  *cygwin*|*mingw*|*msys*)
     hack_to_avoid_long_arg_list=yes
     ;;
   *)
     ;;
esac

AC_MSG_RESULT([${hack_to_avoid_long_arg_list}])
AM_CONDITIONAL([HACK_TO_AVOID_LONG_ARG_LIST], [test "${hack_to_avoid_long_arg_list}" = yes])

RTEMS_CHECK_NEWLIB

# Newlib proprietary
AC_CHECK_MEMBER([struct _Thread_queue_Queue._name],[],[RTEMS_TOOL_CHAIN_ERROR],[#include <sys/lock.h>])

# pthread-functions not declared in some versions of newlib.
RTEMS_CHECK_FUNC([pthread_attr_getguardsize],[#include <pthread.h>])
RTEMS_CHECK_FUNC([pthread_attr_setguardsize],[#include <pthread.h>])
RTEMS_CHECK_FUNC([pthread_attr_setstack],[#include <pthread.h>])
RTEMS_CHECK_FUNC([pthread_attr_getstack],[#include <pthread.h>])

# These are SMP related and were added to newlib by RTEMS.
RTEMS_CHECK_FUNC([pthread_attr_setaffinity_np],[
  #define _GNU_SOURCE
  #include <pthread.h>])
RTEMS_CHECK_FUNC([pthread_attr_getaffinity_np],[
  #define _GNU_SOURCE
  #include <pthread.h>])
RTEMS_CHECK_FUNC([pthread_setaffinity_np],[
  #define _GNU_SOURCE
  #include <pthread.h>])
RTEMS_CHECK_FUNC([pthread_getaffinity_np],[
  #define _GNU_SOURCE
  #include <pthread.h>])
RTEMS_CHECK_FUNC([pthread_getattr_np],[
  #define _GNU_SOURCE
  #include <pthread.h>])

AC_LANG_PUSH(C)
AC_MSG_CHECKING([for mprotect(const void *, ...)])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <sys/mman.h>
int mprotect(const void *, size_t, int);
])],[
AC_MSG_RESULT([yes])
AC_DEFINE(HAVE_MPROTECT_CONST, [], [mprotect(const void *, ...)])
],[
AC_MSG_RESULT([no])
])
AC_MSG_CHECKING([for pthread_mutex_getprioceiling(const pthread_mutex_t *, ...)])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <pthread.h>
int pthread_mutex_getprioceiling(const pthread_mutex_t *__restrict, int *);
])],[
AC_MSG_RESULT([yes])
AC_DEFINE(HAVE_PTHREAD_MUTEX_GETCEILING_CONST, [], [pthread_mutex_getprioceiling(const pthread_mutex_t *, ...)])
],[
AC_MSG_RESULT([no])
])
AC_MSG_CHECKING([for pthread_setschedparam(..., const struct sched_param *)])
AC_COMPILE_IFELSE([AC_LANG_SOURCE([
#include <pthread.h>
int pthread_setschedparam(pthread_t, int, const struct sched_param *);
])],[
AC_MSG_RESULT([yes])
AC_DEFINE(HAVE_PTHREAD_SETSCHEDPARAM_CONST, [], [pthread_setschedparam(..., const struct sched_param *)])
],[
AC_MSG_RESULT([no])
])
AC_LANG_POP(C)

# Some toolchain sanity checks and diagnostics
RTEMS_CHECK_GCC_SANITY

# These are conditionally defined by the toolchain
# FIXME: we should either conditionally compile those parts in
# RTEMS depending on them, or abort - For now, simply check.
AC_CHECK_HEADER([pthread.h],[
  AC_CHECK_TYPES([pthread_rwlock_t])
  AC_CHECK_TYPES([pthread_barrier_t])
  AC_CHECK_TYPES([pthread_spinlock_t])
  AC_CHECK_TYPES([struct _pthread_cleanup_context],[],[RTEMS_TOOL_CHAIN_ERROR],[#include <pthread.h>])
  AC_CHECK_TYPES([struct _Priority_Node],[],[RTEMS_TOOL_CHAIN_ERROR],[#include <pthread.h>])
])

RTEMS_CHECK_MULTIPROCESSING
RTEMS_CHECK_POSIX_API
RTEMS_CHECK_NETWORKING
RTEMS_CHECK_SMP
if test "${RTEMS_HAS_SMP}" = "yes"; then
  AC_CHECK_HEADERS([stdatomic.h],[],[RTEMS_TOOL_CHAIN_ERROR])
fi

rtems_major=`echo _RTEMS_VERSION | sed "s/\..*//"`
rtems_minor=`echo _RTEMS_VERSION | sed "s/[[0-9]][[0-9]]*\.//;s/\..*//"`
rtems_revision=`echo _RTEMS_VERSION | sed "s/[[0-9]][[0-9]]*\.//;s/[[0-9]][[0-9]]*\.//;s/\..*//"`

_RTEMS_CPUOPT_INIT

RTEMS_CPUOPT([RTEMS_DEBUG],
  [test x"${enable_rtems_debug}" = x"yes"],
  [1],
  [if RTEMS_DEBUG is enabled])

RTEMS_CPUOPT([RTEMS_MULTIPROCESSING],
  [test x"$enable_multiprocessing" = xyes],
  [1],
  [if multiprocessing is enabled])

RTEMS_CPUOPT([RTEMS_NEWLIB],
  [test x"$RTEMS_USE_NEWLIB" = xyes],
  [1],
  [if using newlib])

RTEMS_CPUOPT([RTEMS_POSIX_API],
  [test x"$rtems_cv_HAS_POSIX_API" = xyes],
  [1],
  [if posix api is supported])

RTEMS_CPUOPT([RTEMS_SMP],
  [test x"$RTEMS_HAS_SMP" = xyes],
  [1],
  [if SMP is enabled])

RTEMS_CPUOPT([RTEMS_PARAVIRT],
  [test x"$RTEMS_HAS_PARAVIRT" = xyes],
  [1],
  [PARAVIRT is enabled])

RTEMS_CPUOPT([RTEMS_PROFILING],
  [test x"$RTEMS_HAS_PROFILING" = xyes],
  [1],
  [if profiling is enabled])

RTEMS_CPUOPT([RTEMS_NETWORKING],
  [test x"$rtems_cv_HAS_NETWORKING" = xyes],
  [1],
  [if networking is enabled])

RTEMS_CPUOPT([RTEMS_DRVMGR_STARTUP],
  [test x"$enable_drvmgr" = xyes],
  [1],
  [if driver manager api is supported])

RTEMS_CPUOPT([RTEMS_VERSION],
  [true],
  ["]_RTEMS_VERSION["],
  [RTEMS version string])

## Header file differences that need to be known in .h after install

## Deactivate ada bindings
RTEMS_CPUOPT([__RTEMS_ADA__],
  [test x"${enable_ada}" = x"yes"],
  [1],
  [Define to 1 if ada/gnat bindings are built-in])

# These are used to provide <rtems/inttypes.h
AC_CHECK_SIZEOF([mode_t])
AC_CHECK_SIZEOF([off_t])
AC_CHECK_SIZEOF([time_t])
AC_CHECK_SIZEOF([size_t])
AC_CHECK_SIZEOF([blksize_t])
AC_CHECK_SIZEOF([blkcnt_t])

## Provide sizeof(mode_t) information via cpuopts.h
RTEMS_CPUOPT([__RTEMS_SIZEOF_MODE_T__],
  [true],
  [${ac_cv_sizeof_mode_t}],
  [sizeof(mode_t)])

## Provide sizeof(off_t) information via cpuopts.h
RTEMS_CPUOPT([__RTEMS_SIZEOF_OFF_T__],
  [true],
  [${ac_cv_sizeof_off_t}],
  [sizeof(off_t)])

## Provide sizeof(time_t) information via cpuopts.h
RTEMS_CPUOPT([__RTEMS_SIZEOF_TIME_T__],
  [true],
  [${ac_cv_sizeof_time_t}],
  [sizeof(time_t)])

## Provide sizeof(blksize_t) information via cpuopts.h
RTEMS_CPUOPT([__RTEMS_SIZEOF_BLKSIZE_T__],
  [true],
  [${ac_cv_sizeof_blksize_t}],
  [sizeof(blksize_t)])

## Provide sizeof(blkcnt_t) information via cpuopts.h
RTEMS_CPUOPT([__RTEMS_SIZEOF_BLKCNT_T__],
  [true],
  [${ac_cv_sizeof_blkcnt_t}],
  [sizeof(blkcnt_t)])

## Then we propagate a private copy of the value into cpuopts.h
## so it is always available to the RTEMS header files.

RTEMS_CPUOPT([__RTEMS_MAJOR__],
  [true],
  [$rtems_major],
  [major version portion of an RTEMS release])

RTEMS_CPUOPT([__RTEMS_MINOR__],
  [true],
  [$rtems_minor],
  [minor version portion of an RTEMS release])

RTEMS_CPUOPT([__RTEMS_REVISION__],
  [true],
  [$rtems_revision],
  [revision version portion of an RTEMS release])

_RTEMS_CPUOPT_FINI

AC_ENABLE_MULTILIB([Makefile],[..])

# libmisc/shell/* wants to assign file descriptors to stdio file descriptors.
AC_MSG_CHECKING([for assignable stdio])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM(
    [#include <stdio.h>],
    [stdin = fopen("/tmp", "r")])],
  [HAVE_ASSIGNABLE_STDIO=yes],
  [HAVE_ASSIGNABLE_STDIO=no])
AC_MSG_RESULT([$HAVE_ASSIGNABLE_STDIO])

# libmisc/serdbg exploits weak symbols
RTEMS_CHECK_GCC_WEAK

# Used by legacy network stack
AC_CHECK_DECLS([rcmd],,,[#include <unistd.h>])

# Ensure that Newlib does not provide things now in <machine/_timecounter.h>
AC_CHECK_DECLS([_Timecounter_Time_second],[RTEMS_TOOL_CHAIN_ERROR],,[#include <sys/time.h>])

# ... far too many conditionals ...
AM_CONDITIONAL(NEWLIB,test x"$RTEMS_USE_NEWLIB" = x"yes")

AM_CONDITIONAL(HAS_MP,test x"$enable_multiprocessing" = x"yes" )
AM_CONDITIONAL(HAS_SMP,[test "$RTEMS_HAS_SMP" = "yes"])

AM_CONDITIONAL(HAS_PTHREADS,test x"$rtems_cv_HAS_POSIX_API" = x"yes")
AM_CONDITIONAL(LIBNETWORKING,test x"$rtems_cv_HAS_NETWORKING" = x"yes")

AM_CONDITIONAL([LIBSHELL],[test x"$HAVE_ASSIGNABLE_STDIO" = x"yes"])
AM_CONDITIONAL([LIBSERDBG],[test x"$rtems_cv_cc_attribute_weak" = x"yes"])
AM_CONDITIONAL([LIBGNAT],[test x"$rtems_cv_HAS_POSIX_API" = x"yes" \
&& test x"$enable_ada" = x"yes"])

AM_CONDITIONAL([LIBUTF8PROC],[test $ac_cv_sizeof_size_t -gt 2])

AC_CONFIG_HEADER(config.h)

## These are needed by the NFS Client
AC_CHECK_PROG(RPCGEN,rpcgen,rpcgen)
AM_CONDITIONAL([RPCTOOLS],[test "$RPCGEN" = rpcgen \
&& test -n "$AWK" \
&& test "$enable_rpcgen" = yes])

# Filter dynamic loading to only build for architectures that have
# reloc backends
AC_MSG_CHECKING([whether CPU supports libdl])
case $RTEMS_CPU in
  arm | i386 | m68k | mips | moxie | powerpc | sparc)
   HAVE_LIBDL=yes ;;
  # bfin has an issue to resolve with libdl. See ticket #2252
  bfin)
   HAVE_LIBDL=no ;;
  # lm32 has an issue to resolve with libdl. See ticket #2283
  lm32)
   HAVE_LIBDL=no ;;
  # v850 has an issue to resolve with libdl. See ticket #2260
  v850)
   HAVE_LIBDL=no ;;
  *)
   HAVE_LIBDL=no ;;
esac
AM_CONDITIONAL(LIBDL,[test x"$HAVE_LIBDL" = x"yes"])
AC_MSG_RESULT([$HAVE_LIBDL])

# Filter debugger to only build for architectures that have a target backend
AC_MSG_CHECKING([whether CPU supports libdebugger])
case $RTEMS_CPU in
  arm | i386)
   HAVE_LIBDEBUGGER=yes ;;
  *)
   HAVE_LIBDEBUGGER=no ;;
esac
AM_CONDITIONAL(LIBDEBUGGER,[test x"$HAVE_LIBDEBUGGER" = x"yes"])
AC_MSG_RESULT([$HAVE_LIBDEBUGGER])

# Filter libpci to only build for architectures that have support for it
AC_MSG_CHECKING([whether CPU supports libpci])
case $RTEMS_CPU in
  sparc)
   HAVE_LIBPCI=yes ;;
  *)
   HAVE_LIBPCI=no ;;
esac
AM_CONDITIONAL(LIBPCI,[test x"$HAVE_LIBPCI" = x"yes"])
AC_MSG_RESULT([$HAVE_LIBPCI])

# Filter libdrvmgr to only build for architectures that have support for it
AC_MSG_CHECKING([whether CPU supports libdrvmgr])
case $RTEMS_CPU in
  riscv | sparc)
   HAVE_LIBDRVMGR=yes ;;
  *)
   HAVE_LIBDRVMGR=no ;;
esac
AM_CONDITIONAL(LIBDRVMGR,[test x"$HAVE_LIBDRVMGR" = x"yes"])
AC_MSG_RESULT([$HAVE_LIBDRVMGR])

AM_CONDITIONAL([CPU_ARM],[test $RTEMS_CPU = "arm"])
AM_CONDITIONAL([CPU_BFIN],[test $RTEMS_CPU = "bfin"])
AM_CONDITIONAL([CPU_EPIPHANY],[test $RTEMS_CPU = "epiphany"])
AM_CONDITIONAL([CPU_I386],[test $RTEMS_CPU = "i386"])
AM_CONDITIONAL([CPU_LM32],[test $RTEMS_CPU = "lm32"])
AM_CONDITIONAL([CPU_M68K],[test $RTEMS_CPU = "m68k"])
AM_CONDITIONAL([CPU_MIPS],[test $RTEMS_CPU = "mips"])
AM_CONDITIONAL([CPU_MOXIE],[test $RTEMS_CPU = "moxie"])
AM_CONDITIONAL([CPU_NIOS2],[test $RTEMS_CPU = "nios2"])
AM_CONDITIONAL([CPU_NO_CPU],[test $RTEMS_CPU = "no_cpu"])
AM_CONDITIONAL([CPU_OR1K],[test $RTEMS_CPU = "or1k"])
AM_CONDITIONAL([CPU_POWERPC],[test $RTEMS_CPU = "powerpc"])
AM_CONDITIONAL([CPU_RISCV],[test $RTEMS_CPU = "riscv"])
AM_CONDITIONAL([CPU_SH],[test $RTEMS_CPU = "sh"])
AM_CONDITIONAL([CPU_SPARC64],[test $RTEMS_CPU = "sparc64"])
AM_CONDITIONAL([CPU_SPARC],[test $RTEMS_CPU = "sparc"])
AM_CONDITIONAL([CPU_V850],[test $RTEMS_CPU = "v850"])
AM_CONDITIONAL([CPU_X86_64],[test $RTEMS_CPU = "x86_64"])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
