dnl
dnl $Id$
dnl 

AC_DEFUN(RTEMS_PROJECT_ROOT,
[dnl
AC_REQUIRE([RTEMS_TOP])
if test "$TARGET_SUBDIR" = "." ; then
PROJECT_ROOT=$RTEMS_TOPdir/'$(top_builddir)';
else
PROJECT_ROOT=../$RTEMS_TOPdir/'$(top_builddir)'
fi
AC_SUBST(PROJECT_ROOT)

RTEMS_ROOT=$RTEMS_TOPdir/'$(top_builddir)'/c/$RTEMS_BSP
AC_SUBST(RTEMS_ROOT)

INSTALL_CHANGE="\$(KSH) \$(PROJECT_ROOT)/tools/build/install-if-change"
AC_SUBST(INSTALL_CHANGE)

PACKHEX="\$(PROJECT_ROOT)/tools/build/packhex"
AC_SUBST(PACKHEX)
])

