AC_DEFUN([_RTEMS_CPU_SUBDIR],
[
$1 )	if test -d ${srcdir}/ifelse([$2],,[$1],[$2/$1]) ; then
  AC_CONFIG_SUBDIRS(ifelse([$2],,[$1],[$2/$1]))
  fi
])

## RTEMS_CPU_SUBDIRS([PREFIX])
AC_DEFUN([RTEMS_CPU_SUBDIRS],
[
## EDIT: If adding a new cpu to RTEMS, add it to the case block below.
case $RTEMS_CPU in
_RTEMS_CPU_SUBDIR([arm],[$1]);;
_RTEMS_CPU_SUBDIR([bfin],[$1]);;
_RTEMS_CPU_SUBDIR([avr],[$1]);;
_RTEMS_CPU_SUBDIR([h8300],[$1]);;
_RTEMS_CPU_SUBDIR([i386],[$1]);;
_RTEMS_CPU_SUBDIR([lm32],[$1]);;
_RTEMS_CPU_SUBDIR([m32c],[$1]);;
_RTEMS_CPU_SUBDIR([m32r],[$1]);;
_RTEMS_CPU_SUBDIR([m68k],[$1]);;
_RTEMS_CPU_SUBDIR([mips],[$1]);;
_RTEMS_CPU_SUBDIR([nios2],[$1]);;
_RTEMS_CPU_SUBDIR([no_cpu],[$1]);;
_RTEMS_CPU_SUBDIR([powerpc],[$1]);;
_RTEMS_CPU_SUBDIR([sh],[$1]);;
_RTEMS_CPU_SUBDIR([sparc],[$1]);;
_RTEMS_CPU_SUBDIR([sparc64],[$1]);;
_RTEMS_CPU_SUBDIR([v850],[$1]);;
*) AC_MSG_ERROR([Invalid RTEMS_CPU <[$]{RTEMS_CPU}>])
esac
])
