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

AC_SUBST([RTEMS_TOPdir],["$1"])

# HACK: The sed pattern below strips of "../", corresponding to "cpukit/"
rtems_updir=m4_if([$2],[],[`echo "$1/" | sed 's,^\.\.\/,,'`],[$2/])

AS_IF([test -n "$with_multisubdir"],
  [MULTIBUILDTOP=`echo "/$with_multisubdir" | sed 's,/[[^\\/]]*,../,g'`])
AC_SUBST(MULTIBUILDTOP)
 
AS_IF([test -n "$with_multisubdir"],
  [MULTISUBDIR="/$with_multisubdir"])
AC_SUBST(MULTISUBDIR)

AC_ARG_WITH([project-root],[
AS_HELP_STRING(--with-project-root,directory to pre-install files into)],[
## Make sure to have a terminating '/'
case "${with_project_root}" in
*/) ;;
*) with_project_root="${with_project_root}/" ;;
esac

case "${with_project_root}" in
  [[\\/$]]* | ?:[[\\/]]* ) # absolute directory
   PROJECT_ROOT=${with_project_root}
   ;;
  *) # relative directory
   sav0dir=`pwd` && cd ./${rtems_updir}
   sav1dir=`pwd` && cd ../${MULTIBUILDTOP}
   sav2dir=`pwd` && cd "$sav0dir"
   mydir=`echo "$sav1dir" | sed "s,^$sav2dir${MULTISUBDIR}/,,"`
   PROJECT_ROOT='$(top_builddir)'/${rtems_updir}'../$(MULTIBUILDTOP)'${mydir}/${with_project_root}
   ;;
esac],[
## Defaults: Note: Two different defaults!
## ../ for multilib
## '.' for non-multilib
AS_IF([test "$enable_multilib" = "yes"],[ 
  PROJECT_ROOT='$(top_builddir)'/${rtems_updir}'../$(MULTIBUILDTOP)'],[
  PROJECT_ROOT='$(top_builddir)'/${rtems_updir}])
])

AC_SUBST([PROJECT_INCLUDE],["${PROJECT_ROOT}lib/include"])
AC_SUBST([PROJECT_LIB],["${PROJECT_ROOT}lib\$(MULTISUBDIR)"])

libdir="${libdir}\$(MULTISUBDIR)"

AC_SUBST([project_libdir],["\$(libdir)"])
AC_SUBST([project_includedir],["\$(includedir)"])

AC_SUBST([dirstamp],[\${am__leading_dot}dirstamp])
])dnl
