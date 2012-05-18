# AC_DISABLE_OPTION_CHECKING is not available before 2.62
AC_PREREQ(2.62)

dnl
dnl RTEMS_TOP($1)
dnl 
dnl $1 .. relative path from this configure.ac to the toplevel configure.ac
dnl
AC_DEFUN([RTEMS_TOP],
[dnl
AC_REQUIRE([RTEMS_VERSIONING])
AC_REQUIRE([AC_DISABLE_OPTION_CHECKING])
AC_CONFIG_AUX_DIR([$1])
AC_CHECK_PROGS(MAKE, gmake make)
AC_BEFORE([$0], [AM_INIT_AUTOMAKE])dnl

AC_PREFIX_DEFAULT([/opt/rtems-][_RTEMS_API])

RTEMS_TOPdir="$1";
AC_SUBST(RTEMS_TOPdir)

dots=`echo $with_target_subdir|\
sed -e 's,^\.$,,' -e 's%^\./%%' -e 's%[[^/]]$%&/%' -e 's%[[^/]]*/%../%g'`
PROJECT_TOPdir=${dots}${RTEMS_TOPdir}/'$(top_builddir)'
AC_SUBST(PROJECT_TOPdir)

PROJECT_ROOT="${RTEMS_TOPdir}/\$(top_builddir)"
AC_SUBST(PROJECT_ROOT)

AC_MSG_CHECKING([for RTEMS Version])
AC_MSG_RESULT([_RTEMS_VERSION])
pkgdatadir="${datadir}"/rtems[]_RTEMS_API;
AC_SUBST([pkgdatadir])
])dnl
