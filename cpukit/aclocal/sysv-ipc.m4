dnl
dnl $Id$
dnl
dnl Check for System V IPC calls used by Unix simulators
dnl
dnl 98/07/17 Dario Alcocer     alcocer@netcom.com
dnl 	     Ralf Corsepius    corsepiu@faw.uni-ulm.de
dnl
dnl Note: $host_os should probably *not* ever be used here to
dnl determine if host supports System V IPC calls, since some
dnl (e.g. FreeBSD 2.x) are configured by default to include only
dnl a subset of the System V IPC calls.  Therefore, to make sure
dnl all of the required calls are found, test for each call explicitly.
dnl
dnl All of the calls use IPC_PRIVATE, so tests will not unintentionally
dnl modify any existing key sets.  See the man pages for semget, shmget, 
dnl msgget, semctl, shmctl and msgctl for details.

AC_DEFUN([RTEMS_UNION_SEMUN],
[
AC_CACHE_CHECK([whether $host defines union semun],
  rtems_cv_HAS_UNION_SEMUN,
  [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>]], [[union semun arg ;]])],[rtems_cv_HAS_UNION_SEMUN="yes"],[rtems_cv_HAS_UNION_SEMUN="no"])
])
if test "$rtems_cv_HAS_UNION_SEMUN" = "yes"; then
    AC_DEFINE(HAS_UNION_SEMUN,[1],[if having union semum])
fi
])

AC_DEFUN([RTEMS_SYSV_SEM],
[AC_REQUIRE([AC_PROG_CC]) 
AC_CACHE_CHECK(whether $host supports System V semaphores,
rtems_cv_sysv_sem,
[
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#if !HAS_UNION_SEMUN
  union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
  } ;
#endif
int main () {
  union semun arg ;

  int id=semget(IPC_PRIVATE,1,IPC_CREAT|0400);
  if (id == -1)
    exit(1);
  arg.val = 0; /* avoid implicit type cast to union */
  if (semctl(id, 0, IPC_RMID, arg) == -1)
    exit(1);
  exit(0);
}
]])],[rtems_cv_sysv_sem="yes"],[rtems_cv_sysv_sem="no"],[:])
])
])

AC_DEFUN([RTEMS_SYSV_SHM],
[AC_REQUIRE([AC_PROG_CC]) 
# AC_REQUIRE([RTEMS_CANONICAL_HOST])
AC_CACHE_CHECK(whether $host supports System V shared memory,
rtems_cv_sysv_shm,
[
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
int main () {
  int id=shmget(IPC_PRIVATE,1,IPC_CREAT|0400);
  if (id == -1)
    exit(1);
  if (shmctl(id, IPC_RMID, 0) == -1)
    exit(1);
  exit(0);
}
]])],[rtems_cv_sysv_shm="yes"],[rtems_cv_sysv_shm="no"],[:])
])
])

AC_DEFUN([RTEMS_SYSV_MSG],
[AC_REQUIRE([AC_PROG_CC]) 
# AC_REQUIRE([RTEMS_CANONICAL_HOST])
AC_CACHE_CHECK(whether $host supports System V messages,
rtems_cv_sysv_msg,
[
AC_RUN_IFELSE([AC_LANG_SOURCE([[
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
int main () {
  int id=msgget(IPC_PRIVATE,IPC_CREAT|0400);
  if (id == -1)
    exit(1);
  if (msgctl(id, IPC_RMID, 0) == -1)
    exit(1);
  exit(0);
}
]])],[rtems_cv_sysv_msg="yes"],[rtems_cv_sysv_msg="no"],[:])
])
])

AC_DEFUN([RTEMS_CHECK_SYSV_UNIX],
[# AC_REQUIRE([RTEMS_CANONICAL_HOST])
if test "$RTEMS_CPU" = "unix" ; then
  RTEMS_UNION_SEMUN
  RTEMS_SYSV_SEM
  if test "$rtems_cv_sysv_sem" != "yes" ; then
    AC_MSG_ERROR([System V semaphores don't work, required by simulator])
  fi
  RTEMS_SYSV_SHM
  if test "$rtems_cv_sysv_shm" != "yes" ; then
    AC_MSG_ERROR([System V shared memory doesn't work, required by simulator])
  fi
  RTEMS_SYSV_MSG
  if test "$rtems_cv_sysv_msg" != "yes" ; then
    AC_MSG_ERROR([System V messages don't work, required by simulator])
  fi
fi
])
