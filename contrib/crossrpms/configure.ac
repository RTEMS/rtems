## $Id$

AC_PREREQ(2.59)
AC_INIT([crossbuild],[0.20060211.0],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([binutils/binutils.add])
AC_CONFIG_AUX_DIR(.)

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define foreign 1.9])

RTEMS_ENABLE_RPMPREFIX

AS_IF([test "${host}" != "${build}"],[
AC_MSG_CHECKING(for RPM OS)
case "$host" in
  i?86-*cygwin*) 	RPM_OS=cygwin ;;
  i?86-*mingw*)		RPM_OS=mingw32 ;;
  sparc-*solaris*)	RPM_OS=solaris2.7 ;;
  i?86-*freebsd5*)	RPM_OS=freebsd5.2 ;;
  i?86-*freebsd*)	RPM_OS=freebsd6.0 ;;
  *)
    AC_MSG_ERROR([Unsupported host ${host}])
    ;;
esac
AC_MSG_RESULT($RPM_OS)
])
AC_SUBST(RPM_OS)

AC_CONFIG_FILES([Makefile])

AC_CONFIG_FILES([cygwin/Makefile])
AC_CONFIG_FILES([mingw32/Makefile])
AC_CONFIG_FILES([solaris2.7/Makefile])
AC_CONFIG_FILES([freebsd5.2/Makefile])
AC_CONFIG_FILES([freebsd6.0/Makefile])
AC_CONFIG_FILES([rtems/Makefile])

AC_OUTPUT
