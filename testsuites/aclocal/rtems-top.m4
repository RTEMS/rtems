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
AC_REQUIRE([AM_SET_LEADING_DOT])
AC_REQUIRE([AC_DISABLE_OPTION_CHECKING])
AC_CONFIG_AUX_DIR([$1])
AC_CHECK_PROGS(MAKE, gmake make)
AC_BEFORE([$0], [AM_INIT_AUTOMAKE])dnl

AC_PREFIX_DEFAULT([/opt/rtems-][_RTEMS_API])

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
   RTEMS_ROOT=${enable_rtems_root}
   ;;
  *) # relative directory
   rtems_rootdir=${rtems_updir}${enable_rtems_root}
   RTEMS_ROOT='$(top_builddir)'/${rtems_updir}${enable_rtems_root}
   ;;
esac],[
rtems_rootdir=${rtems_updir}
RTEMS_ROOT='$(top_builddir)'/${rtems_updir}
])
AC_SUBST([RTEMS_ROOT])

AS_IF([test -n "${with_target_subdir}"],
  [project_top="../${with_project_top}"],
  [project_top="${with_project_top}"])
AC_SUBST([PROJECT_TOPdir],[${project_top}${rtems_updir}'$(top_builddir)'])

AC_ARG_ENABLE([cpukit-root],[
AS_HELP_STRING(--enable-cpukit-root,directory containing lib/librtemscpu.a)],
[case ${enable_cpukit_root} in
  [[\\/$]]* | ?:[[\\/]]* ) # absolute directory
   cpukit_rootdir=${enable_cpukit_root}
   CPUKIT_ROOT=${enable_cpukit_root}
   ;;
  *) # relative directory
   cpukit_rootdir=${rtems_updir}${enable_cpukit_root}
   CPUKIT_ROOT='$(top_builddir)'/${rtems_updir}${enable_cpukit_root}
   ;;
esac],[
cpukit_rootdir=
CPUKIT_ROOT=
])
AC_SUBST([CPUKIT_ROOT])

AC_ARG_ENABLE([project-root],[
AS_HELP_STRING(--enable-project-root,directory containing lib/librtemsbsp.a)],
[case ${enable_project_root} in
  [[\\/$]]* | ?:[[\\/]]* ) # absolute directory
   project_rootdir=${enable_project_root}
   PROJECT_ROOT=${enable_project_root}
   ;;
  *) # relative directory
   project_rootdir=${rtems_updir}${enable_project_root}
   PROJECT_ROOT='$(top_builddir)'/${rtems_updir}${enable_project_root}
   ;;
esac],[
project_rootdir=
PROJECT_ROOT=
])
AC_SUBST([PROJECT_ROOT])
])dnl
