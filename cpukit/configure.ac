## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-cpukit],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
AC_CONFIG_SRCDIR([score])
RTEMS_TOP([..],[])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define nostdinc subdir-objects foreign 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_MULTILIB
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

RTEMS_CHECK_NEWLIB

# BSD-isms, used throughout the sources
# Not really used by this configure script
# FIXME: They should be eliminated if possible.
AC_CHECK_FUNCS([strsep strcasecmp snprintf])
AC_CHECK_FUNCS([strdup strndup strncasecmp])
AC_CHECK_FUNCS([bcopy bcmp])
AC_CHECK_FUNCS([isascii fileno])

# <FIXME>
#   Check for functions supplied by newlib >= 1.17.0
# Newlib's posix/ directory
AC_CHECK_FUNCS([readdir_r isatty])
AC_CHECK_FUNCS([creat \
  opendir closedir readdir rewinddir scandir seekdir \
  sleep \
  telldir \
  usleep],,
  [RTEMS_TOOL_CHAIN_ERROR])
AC_CHECK_FUNCS([__assert])
AC_CHECK_FUNCS([execl execlp execle execv execvp execve])
AC_CHECK_FUNCS([regcomp regexec regerror regfree])

# Mandated by POSIX, decls not present in some versions of newlib
AC_CHECK_DECLS([flockfile],[AC_CHECK_FUNCS([flockfile])],,[#include <stdio.h>])
AC_CHECK_DECLS([funlockfile],[AC_CHECK_FUNCS([funlockfile])],,[#include <stdio.h>])
AC_CHECK_DECLS([ftrylockfile],[AC_CHECK_FUNCS([ftrylockfile])],,[#include <stdio.h>])

# Newlib proprietary
AC_CHECK_MEMBER([struct _Thread_queue_Queue._name],[],[RTEMS_TOOL_CHAIN_ERROR],[#include <sys/lock.h>])

# Mandated by POSIX, older newlibs bogusly provided CLOCK_PROCESS_CPUTIME+CLOCK_THREAD_CPUTIME
AC_CHECK_DECL([CLOCK_PROCESS_CPUTIME_ID],[],[RTEMS_TOOL_CHAIN_ERROR],[#include <time.h>])
AC_CHECK_DECL([CLOCK_THREAD_CPUTIME_ID],[],[RTEMS_TOOL_CHAIN_ERROR],[#include <time.h>])

# Mandated by POSIX, decls not present in some versions of newlib,
# some versions stubbed in newlib's rtems crt0
RTEMS_CHECK_FUNC([seteuid],[#include <unistd.h>])
RTEMS_CHECK_FUNC([geteuid],[#include <unistd.h>])
RTEMS_CHECK_FUNC([setegid],[#include <unistd.h>])
RTEMS_CHECK_FUNC([getegid],[#include <unistd.h>])
RTEMS_CHECK_FUNC([setuid],[#include <unistd.h>])
RTEMS_CHECK_FUNC([getuid],[#include <unistd.h>])
RTEMS_CHECK_FUNC([setgid],[#include <unistd.h>])
RTEMS_CHECK_FUNC([getgid],[#include <unistd.h>])
RTEMS_CHECK_FUNC([setsid],[#include <unistd.h>])
RTEMS_CHECK_FUNC([getsid],[#include <unistd.h>])
RTEMS_CHECK_FUNC([setpgid],[#include <unistd.h>])
RTEMS_CHECK_FUNC([getpgid],[#include <unistd.h>])
RTEMS_CHECK_FUNC([setpgrp],[#include <unistd.h>])
RTEMS_CHECK_FUNC([getpgrp],[#include <unistd.h>])

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

# Mandated by POSIX, not declared in some versions of newlib.
AC_CHECK_DECLS([getrusage],,,[#include sys/resource.h])

# Newlib's unix/ directory
AC_CHECK_FUNCS([ttyname getcwd])
# </FIXME>

# Check if the installed toolchain provides these headers
# and error out if not.
AC_CHECK_HEADERS([errno.h sched.h semaphore.h sys/cdefs.h sys/queue.h tar.h threads.h],,
  [RTEMS_TOOL_CHAIN_ERROR])

## error out if libc doesn't provide stdint.h
AS_IF([test x"${ac_cv_header_stdint_h}" != xyes],
[RTEMS_TOOL_CHAIN_ERROR])

## error out if libc doesn't provide inttypes.h
AS_IF([test x"${ac_cv_header_inttypes_h}" != xyes],
[RTEMS_TOOL_CHAIN_ERROR])

AC_HEADER_STDBOOL
AS_IF([test x"${ac_cv_header_stdbool_h}" != xyes],
[RTEMS_TOOL_CHAIN_ERROR])

AC_CHECK_TYPES([ uint8_t,  int8_t])
AC_CHECK_TYPES([uint16_t, int16_t])
AC_CHECK_TYPES([uint32_t, int32_t])
AC_CHECK_TYPES([uint64_t, int64_t])
AC_CHECK_TYPES([uintmax_t, intmax_t])
AC_CHECK_TYPES([uintptr_t, intptr_t])

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
])

AC_CHECK_HEADER([signal.h],[
  AC_CHECK_TYPES([sighandler_t])
])

if test x"$RTEMS_USE_NEWLIB" = xyes ; then
  AC_CHECK_DECLS([__getreent],[],[RTEMS_TOOL_CHAIN_ERROR],[#include <sys/reent.h>])
fi

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

# FIXME: These checks are only in here to provide
# configuration-time diagnostics and are not really used.
AC_CHECK_DECLS([_POSIX_LOGIN_NAME_MAX],,,[#include <limits.h>])
AC_CHECK_DECLS([CHAR_BIT],,,[#include <limits.h>])

# FIXME: We should get rid of this.
# So far, only used in libfs/src/nfsclient/src/dirutils.c
AC_CHECK_SIZEOF([mode_t])
AC_CHECK_SIZEOF([off_t])

# FIXME: We should get rid of this. It's a cludge.
AC_CHECK_SIZEOF([time_t])

AC_CHECK_SIZEOF([size_t])

# FIXME: Mandatory in SUSv4, optional in SUSv3.
#   Not implemented in GCC/newlib, so far.
AC_CHECK_DECLS([WORD_BIT],,,[#include <limits.h>])
AC_CHECK_DECLS([LONG_BIT],,,[#include <limits.h>])

## BSD-ism, excluded from POSIX, but available on most platforms
AC_CHECK_DECLS([sbrk],,,[#include <unistd.h>])
AC_CHECK_DECLS([rcmd],,,[#include <unistd.h>])

## Check if libc provides BSD's strlcpy/strlcat
AC_CHECK_FUNCS(strlcpy strlcat)

## Check if libc provides decl of utime
## FIXME: utime has been deprecated in SUSv4.
##        and is likely to be removed in future versions.
## FIXME (BUG in newlib): SUSv4 saids including <utime.h> should be sufficient.
AC_CHECK_DECLS([utime],,,[#include <sys/types.h>
#include <utime.h>])
## Check if libc provides decl of utimes
AC_CHECK_DECLS([utimes],,,[#include <sys/time.h>])

# ... far too many conditionals ...
AM_CONDITIONAL(LIBRPC,[test x"$rtems_cv_HAS_NETWORKING" = x"yes"])
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

AM_CONDITIONAL([LIBDOSFS],[dnl
test x"$ac_cv_type_uint8_t" = xyes \
&& test x"$ac_cv_type_uint16_t" = xyes])

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

AC_MSG_CHECKING([whether CPU supports SHA])
case $RTEMS_CPU in
  m32c)
   HAVE_SHA=no ;;
  *)
   HAVE_SHA=yes ;;
esac
AM_CONDITIONAL(SHA,[test x"$HAVE_SHA" = x"yes"])
AC_MSG_RESULT([$HAVE_SHA])

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
  sparc)
   HAVE_LIBDRVMGR=yes ;;
  *)
   HAVE_LIBDRVMGR=no ;;
esac
AM_CONDITIONAL(LIBDRVMGR,[test x"$HAVE_LIBDRVMGR" = x"yes"])
AC_MSG_RESULT([$HAVE_LIBDRVMGR])


RTEMS_AMPOLISH3

# Explicitly list all Makefiles here
AC_CONFIG_FILES([
Doxyfile
Makefile
dev/Makefile
dtc/libfdt/Makefile
rtems/Makefile
sapi/Makefile
score/Makefile
score/cpu/Makefile
score/cpu/arm/Makefile
score/cpu/bfin/Makefile
score/cpu/epiphany/Makefile
score/cpu/i386/Makefile
score/cpu/lm32/Makefile
score/cpu/m68k/Makefile
score/cpu/m32c/Makefile
score/cpu/mips/Makefile
score/cpu/moxie/Makefile
score/cpu/nios2/Makefile
score/cpu/or1k/Makefile
score/cpu/powerpc/Makefile
score/cpu/sh/Makefile
score/cpu/sparc/Makefile
score/cpu/sparc64/Makefile
score/cpu/v850/Makefile
score/cpu/no_cpu/Makefile
posix/Makefile
libblock/Makefile
libdrvmgr/Makefile
libfs/Makefile
libfs/src/nfsclient/Makefile
libgnat/Makefile
libcrypt/Makefile
libcsupport/Makefile
libnetworking/Makefile
libpci/Makefile
librpc/Makefile
libmisc/Makefile
libi2c/Makefile
libmd/Makefile
libdl/Makefile
libstdthreads/Makefile
libdebugger/Makefile
zlib/Makefile
ftpd/Makefile
telnetd/Makefile
pppd/Makefile
mghttpd/Makefile
wrapup/Makefile])

AC_CONFIG_COMMANDS([preinstall-stamp],
[test -z "$with_multisubdir" && ${MAKE} preinstall-stamp],
[MAKE=${MAKE}
with_multisubdir="$with_multisubdir"])

AC_OUTPUT
