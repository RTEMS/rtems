dnl $Id$

m4_define([RTEMS_API],[4.7])

dnl
dnl RTEMS_TOP($1)
dnl 
dnl $1 .. relative path from this configure.ac to the toplevel configure.ac
dnl
AC_DEFUN([RTEMS_TOP],
[dnl
AC_REQUIRE([RTEMS_VERSIONING])
AC_REQUIRE([AM_SET_LEADING_DOT])
AC_CONFIG_AUX_DIR([../$1])
AC_CHECK_PROGS(MAKE, gmake make)
AC_BEFORE([$0], [AM_INIT_AUTOMAKE])dnl

AC_PREFIX_DEFAULT([/opt/rtems-][RTEMS_API])

RTEMS_TOPdir="$1";
AC_SUBST(RTEMS_TOPdir)

## with_target_subdirs is handled implicitly by autoconf
test -n "$with_target_subdir" || with_target_subdir="."

AC_MSG_CHECKING([for RTEMS Version])
AC_MSG_RESULT([_RTEMS_VERSION])

AC_SUBST([dirstamp],[\${am__leading_dot}dirstamp])
])dnl
