## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ([2.68])
AC_INIT([rtems-c-src-tests-fstests],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([imfs_support])
RTEMS_TOP([../..],[..])

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.11.1])
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
imfs_fsfchx/Makefile 
imfs_fsfile01/Makefile 
imfs_fsfile02/Makefile 
imfs_fsfstest/Makefile 
imfs_fsimfs01/Makefile 
imfs_fsimfs02/Makefile 
imfs_fslink/Makefile 
imfs_fsmkdir/Makefile 
imfs_fsopen/Makefile 
imfs_fsrdwrv/Makefile 
imfs_fsreaddir/Makefile 
imfs_fsrename/Makefile 
imfs_fsrmdir/Makefile 
imfs_fsrwfile/Makefile 
imfs_fsstat/Makefile 
imfs_fssymlink/Makefile 
imfs_fstruncate/Makefile 
mdosfs_fsfchx/Makefile 
mdosfs_fsfile01/Makefile 
mdosfs_fsfile02/Makefile 
mdosfs_fsfstest/Makefile 
mdosfs_fsimfs01/Makefile 
mdosfs_fsimfs02/Makefile 
mdosfs_fslink/Makefile 
mdosfs_fsmkdir/Makefile 
mdosfs_fsopen/Makefile 
mdosfs_fsrdwrv/Makefile 
mdosfs_fsreaddir/Makefile 
mdosfs_fsrename/Makefile 
mdosfs_fsrmdir/Makefile 
mdosfs_fsrwfile/Makefile 
mdosfs_fsstat/Makefile 
mdosfs_fssymlink/Makefile 
mdosfs_fstruncate/Makefile 
mimfs_fsfchx/Makefile 
mimfs_fsfile01/Makefile 
mimfs_fsfile02/Makefile 
mimfs_fsfstest/Makefile 
mimfs_fsimfs01/Makefile 
mimfs_fsimfs02/Makefile 
mimfs_fslink/Makefile 
mimfs_fsmkdir/Makefile 
mimfs_fsopen/Makefile 
mimfs_fsrdwrv/Makefile 
mimfs_fsreaddir/Makefile 
mimfs_fsrename/Makefile 
mimfs_fsrmdir/Makefile 
mimfs_fsrwfile/Makefile 
mimfs_fsstat/Makefile 
mimfs_fssymlink/Makefile 
mimfs_fstruncate/Makefile 
mrfs_fsfchx/Makefile 
mrfs_fsfile01/Makefile 
mrfs_fsfile02/Makefile 
mrfs_fsfstest/Makefile 
mrfs_fsimfs01/Makefile 
mrfs_fsimfs02/Makefile 
mrfs_fslink/Makefile 
mrfs_fsmkdir/Makefile 
mrfs_fsopen/Makefile 
mrfs_fsrdwrv/Makefile 
mrfs_fsreaddir/Makefile 
mrfs_fsrename/Makefile 
mrfs_fsrmdir/Makefile 
mrfs_fsrwfile/Makefile 
mrfs_fsstat/Makefile 
mrfs_fssymlink/Makefile 
mrfs_fstruncate/Makefile 

])
AC_OUTPUT
