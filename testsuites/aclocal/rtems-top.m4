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

AC_SUBST([RTEMS_TOPdir],["$1"])

rtems_updir=m4_if([$2],[],[],[$2/])

AC_ARG_ENABLE([rtems-root],[
AS_HELP_STRING(--enable-rtems-root,directory containing make/custom)],
[case ${enable_rtems_root} in
  [[\\/$]]* | ?:[[\\/]]* ) # absolute directory
   rtems_rootdir=${enable_rtems_root}
   RTEMS_RTEMS=${enable_rtems_root}
   ;;
  *) # relative directory
   rtems_rootdir=${enable_rtems_root}${rtems_updir}
   RTEMS_ROOT=${enable_rtems_root}${rtems_updir}'$(top_builddir)'
   ;;
esac],[
rtems_rootdir=${rtems_updir}
RTEMS_ROOT=${rtems_updir}'$(top_builddir)'
])
AC_SUBST([RTEMS_ROOT])

AC_SUBST([PROJECT_TOPdir],[${with_project_top}${rtems_updir}'$(top_builddir)'])
AC_SUBST([PROJECT_ROOT],[${with_project_root}${rtems_updir}'$(top_builddir)'])

AC_SUBST([dirstamp],[\${am__leading_dot}dirstamp])
])dnl
