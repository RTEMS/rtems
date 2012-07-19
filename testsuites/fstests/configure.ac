## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-fstests],
        [_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([imfs_support])
RTEMS_TOP([../..],[..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE

RTEMS_ENV_RTEMSBSP

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
AC_CONFIG_HEADER([config.h])

RTEMS_CHECK_CPUOPTS([RTEMS_POSIX_API])
AM_CONDITIONAL(HAS_POSIX,test x"${rtems_cv_RTEMS_POSIX_API}" = x"yes")

# but newlib doesn't have sys/mman.h
AC_CHECK_HEADERS([sys/mman.h])

# FIXME: newlib should supply declaration of pthread_rwlock_unlock()
AC_CHECK_DECLS([pthread_rwlock_unlock],[],[],[[#include <pthread.h>]])

# FIXME: newlib should supply declaration of pthread_atfork()
AC_CHECK_DECLS([pthread_atfork],[],[],[[#include <pthread.h>]])

# FIXME: RTEMS presumes pthread_attr_getcputime to be IEEE Std 1003.1
# Likely an anachronism in RTEMS.
AC_CHECK_DECLS([pthread_attr_getcputime],[],[],[[#include <pthread.h>]])

# FIXME: RTEMS presumes pthread_attr_getcputime to be IEEE Std 1003.1
# Likely an anachronism in RTEMS.
AC_CHECK_DECLS([pthread_attr_getcputime],[],[],[[#include <pthread.h>]])

# FIXME: RTEMS presumes pthread_attr_setcputime to be IEEE Std 1003.1
# Likely an anachronism in RTEMS.
AC_CHECK_DECLS([pthread_attr_setcputime],[],[],[[#include <pthread.h>]])

# Added to pthreads after initial revision.  May not be in toolset
AC_CHECK_DECLS([pthread_attr_getstack],[],[],[[#include <pthread.h>]])

# Added to pthreads after initial revision.  May not be in toolset
AC_CHECK_DECLS([pthread_attr_setstack],[],[],[[#include <pthread.h>]])

# Added to pthreads after initial revision.  May not be in toolset
AC_CHECK_DECLS([pthread_attr_getguardsize],[],[],[[#include <pthread.h>]])

# Added to pthreads after initial revision.  May not be in toolset
AC_CHECK_DECLS([pthread_attr_setguardsize],[],[],[[#include <pthread.h>]])

# FIXME: adjtime is a non-standardized BSD/Linux extension
# RTEMS should not rely on adjtime
AC_CHECK_DECLS([adjtime],[],[],[[#include <sys/time.h>]])

# FIXME: IEEE Std 1003.1-2008 mandates mprotect in sys/mman.h,
# RTEMS provides a stub, despite newlib doesn't have sys/mman.h
AC_CHECK_DECLS([mprotect],[],[],[[#include <sys/mman.h>]])

# FIXME: IEEE Std 1003.1-2008 mandates seteuid in unistd.h
# RTEMS provides it, despite newlib doesn't declare it.
AC_CHECK_DECLS([seteuid],[],[],[#include <unistd.h>])

# FIXME: We should get rid of this. It's a cludge.
AC_CHECK_SIZEOF([off_t])
AC_CHECK_SIZEOF([blksize_t])
AC_CHECK_SIZEOF([blkcnt_t])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
fsfseeko01/Makefile
fsdosfssync01/Makefile
imfs_fserror/Makefile
imfs_fslink/Makefile
imfs_fspatheval/Makefile
imfs_fspermission/Makefile
imfs_fsrdwr/Makefile
imfs_fssymlink/Makefile
imfs_fstime/Makefile
mdosfs_fserror/Makefile
mdosfs_fspatheval/Makefile
mdosfs_fsrdwr/Makefile
mdosfs_fstime/Makefile
mimfs_fserror/Makefile
mimfs_fslink/Makefile
mimfs_fspatheval/Makefile
mimfs_fspermission/Makefile
mimfs_fsrdwr/Makefile
mimfs_fssymlink/Makefile
mimfs_fstime/Makefile
mrfs_fserror/Makefile
mrfs_fslink/Makefile
mrfs_fspatheval/Makefile
mrfs_fspermission/Makefile
mrfs_fsrdwr/Makefile
mrfs_fssymlink/Makefile
mrfs_fstime/Makefile
fsnofs01/Makefile
fsimfsgeneric01/Makefile
fsbdpart01/Makefile

])
AC_OUTPUT
