dnl $Id$

dnl canonicalize target name
dnl NOTE: Most rtems targets do not fullfil autoconf's
dnl target naming conventions "processor-vendor-os"
dnl Therefore autoconf's AC_CANONICAL_TARGET will fail for them
dnl and we have to fix it for rtems ourselves 

AC_DEFUN(RTEMS_CANONICAL_TARGET_CPU,
[AC_MSG_CHECKING(rtems target cpu)
changequote(<<, >>)dnl
target_cpu=`echo $target | sed 's%^\([^-]*\)-\(.*\)$%\1%'`
changequote([, ])dnl
AC_MSG_RESULT($target_cpu)
])
