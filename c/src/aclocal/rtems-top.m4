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

AC_SUBST([RTEMS_TOPdir],["$1"])

# HACK: The sed pattern in rtems_updir matches c/src/
rtems_updir=m4_if([$2],[],[`echo "$1/" | sed s,^\.\.\/\.\.\/,,`],[$2/])

AC_SUBST([RTEMS_ROOT],[${rtems_updir}'$(top_builddir)'])

AS_IF([test -n "${with_target_subdir}"],
  [project_top="../${with_project_top}"],
  [project_top="${with_project_top}"])
AC_SUBST([PROJECT_TOPdir],[${project_top}${rtems_updir}'$(top_builddir)'])

AC_SUBST([PROJECT_ROOT],[${with_project_root}${rtems_updir}'$(top_builddir)'])

AC_SUBST([dirstamp],[\${am__leading_dot}dirstamp])
])dnl
