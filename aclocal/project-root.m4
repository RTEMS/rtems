dnl
dnl $Id$
dnl 

dnl
dnl PROJECT_TOPdir     .. relative path to the top of the build-tree
dnl PROJECT_ROOT       .. relative path to the top of the temporary
dnl                       installation directory inside the build-tree
dnl RTEMS_TOPdir       .. relative path of a subpackage's configure.in to the
dnl                       toplevel configure.in of the source-tree
dnl RTEMS_ROOT         .. path to the top of a bsp's build directory
dnl                       [Applied by custom/*.cfg, depredicated otherwise]
dnl

AC_DEFUN(RTEMS_PROJECT_ROOT,
[dnl
AC_REQUIRE([RTEMS_TOP])
if test "$TARGET_SUBDIR" = "." ; then
# Native
PROJECT_TOPdir=${RTEMS_TOPdir}/'$(top_builddir)'
PROJECT_ROOT=${RTEMS_TOPdir}/'$(top_builddir)';
else
# FIXME: Moving the build-tree
# PROJECT_TOPdir=../${RTEMS_TOPdir}/'$(top_builddir)'
# PROJECT_ROOT=${RTEMS_TOPdir}/'$(top_builddir)'

# FIXME: Old, per-bsp building style
PROJECT_TOPdir=../${RTEMS_TOPdir}/'$(top_builddir)'
PROJECT_ROOT=../${RTEMS_TOPdir}/'$(top_builddir)'
fi
AC_SUBST(PROJECT_ROOT)
AC_SUBST(PROJECT_TOPdir)

# FIXME: This should not be here
RTEMS_ROOT=$RTEMS_TOPdir/'$(top_builddir)'/c/$RTEMS_BSP
AC_SUBST(RTEMS_ROOT)

PACKHEX="\$(PROJECT_TOPdir)/tools/build/packhex"
AC_SUBST(PACKHEX)

PROJECT_INCLUDE="\$(PROJECT_ROOT)/$RTEMS_BSP/lib/include"
AC_SUBST(PROJECT_INCLUDE)

PROJECT_RELEASE="\$(PROJECT_ROOT)/$RTEMS_BSP"
AC_SUBST(PROJECT_RELEASE)
])

