## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-tests-psxtmtests],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([psxtmmutex02])
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

OPERATION_COUNT=${OPERATION_COUNT-100}
AC_SUBST(OPERATION_COUNT)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
psxtmbarrier01/Makefile
psxtmbarrier02/Makefile
psxtmbarrier03/Makefile
psxtmkey01/Makefile
psxtmkey02/Makefile
psxtmmq01/Makefile
psxtmmutex01/Makefile
psxtmmutex02/Makefile
psxtmmutex03/Makefile
psxtmmutex04/Makefile
psxtmmutex05/Makefile
psxtmmutex06/Makefile
psxtmmutex07/Makefile
psxtmnanosleep01/Makefile
psxtmnanosleep02/Makefile
psxtmrwlock01/Makefile
psxtmrwlock02/Makefile
psxtmrwlock03/Makefile
psxtmrwlock04/Makefile
psxtmrwlock05/Makefile
psxtmrwlock06/Makefile
psxtmrwlock07/Makefile
psxtmsem01/Makefile
psxtmsem02/Makefile
psxtmsem03/Makefile
psxtmsem04/Makefile
psxtmsem05/Makefile
psxtmsleep01/Makefile
psxtmsleep02/Makefile
psxtmthread01/Makefile
psxtmthread03/Makefile
])
AC_OUTPUT
