dnl $Id$

m4_define([RTEMS_API],[4.7])

dnl
dnl RTEMS_TOP($1)
dnl 
dnl $1 .. relative path from this configure.in to the toplevel configure.in
dnl
AC_DEFUN([RTEMS_TOP],
[dnl
AC_REQUIRE([RTEMS_VERSIONING])
AC_REQUIRE([AM_SET_LEADING_DOT])
AC_CONFIG_AUX_DIR([$1])
AC_CHECK_PROGS(MAKE, gmake make)
AC_BEFORE([$0], [AM_INIT_AUTOMAKE])dnl

AC_PREFIX_DEFAULT([/opt/rtems-][RTEMS_API])

## HACK to allow gnu-make conditionals in automake-Makefiles.
ENDIF=endif
AC_SUBST(ENDIF)

RTEMS_TOPdir="$1";
AC_SUBST(RTEMS_TOPdir)

## with_target_subdirs is handled implicitly by autoconf
dots=`echo $with_target_subdir|\
sed -e 's,^\.$,,' -e 's%^\./%%' -e 's%[[^/]]$%&/%' -e 's%[[^/]]*/%../%g'`
PROJECT_TOPdir=${dots}${RTEMS_TOPdir}/'$(top_builddir)'
AC_SUBST([PROJECT_TOPdir])

RTEMS_ROOT=$with_rtems_root`echo "$1/" | sed 's,^../,,'`'$(top_builddir)'
AC_SUBST(RTEMS_ROOT)

PROJECT_ROOT=$with_project_root`echo "$1/" | sed 's,^../,,'`'$(top_builddir)'
AC_SUBST(PROJECT_ROOT)

AC_SUBST([dirstamp],[\${am__leading_dot}dirstamp])
])dnl
