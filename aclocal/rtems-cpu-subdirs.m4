AC_DEFUN([_RTEMS_CPU_SUBDIR],
[
$1 )	if test -d ${srcdir}/$1 ; then
  AC_CONFIG_SUBDIRS([$1])
  fi
])

AC_DEFUN([RTEMS_CPU_SUBDIRS],
[
## EDIT: If adding a new cpu to RTEMS, add it to the case block below.
case $RTEMS_CPU in
_RTEMS_CPU_SUBDIR([a29k]);;
_RTEMS_CPU_SUBDIR([arm]);;
_RTEMS_CPU_SUBDIR([c4x]);;
_RTEMS_CPU_SUBDIR([h8300]);;
_RTEMS_CPU_SUBDIR([hppa1.1]);;
_RTEMS_CPU_SUBDIR([i386]);;
_RTEMS_CPU_SUBDIR([i960]);;
_RTEMS_CPU_SUBDIR([m68k]);;
_RTEMS_CPU_SUBDIR([mips]);;
_RTEMS_CPU_SUBDIR([mips64orion]);;
_RTEMS_CPU_SUBDIR([no_cpu]);;
_RTEMS_CPU_SUBDIR([or16]);;
_RTEMS_CPU_SUBDIR([or32]);;
_RTEMS_CPU_SUBDIR([powerpc]);;
_RTEMS_CPU_SUBDIR([sh]);;
_RTEMS_CPU_SUBDIR([sparc]);;
_RTEMS_CPU_SUBDIR([unix]);;
*) AC_MSG_ERROR([Invalid RTEMS_CPU])
esac
])
