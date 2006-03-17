## $Id$

m4_define([RTEMS_API],[4.7])

AC_PREREQ(2.59)
AC_INIT([crossrpms],[0.20060317.0],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([binutils/binutils.add])
AC_CONFIG_AUX_DIR(.)

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AC_PREFIX_DEFAULT([/opt/rtems-][RTEMS_API])

AM_INIT_AUTOMAKE([1.9])

case "$prefix" in
/usr)
  build_infos=0
  sysdirpattern="/%sysdir/d"
  rpmprefix=
  ;;
/usr/local)
  build_infos=0
  sysdirpattern="/%sysdir/d"
  rpmprefix="local-"
  ;;
/opt/rtems)
  build_infos=1
  sysdirpattern="s/^%sysdir/^%dir/g"
  rpmprefix="rtems-"
  ;;
NONE|${ac_default_prefix}) # Default prefix
  build_infos=1
  sysdirpattern="s/^%sysdir/^%dir/g"
  rpmprefix="[rtems-]RTEMS_API[-]"
  ;;
esac
AC_SUBST(sysdirpattern)

AC_ARG_ENABLE([rpmprefix],
[  --enable-rpmprefix=<rpmprefix>      prefix rpms],[
  case "$enable_rpmprefix" in
  yes) # ignore, use default
    ;;
  no) rpmprefix=
    ;;
  *) # presume user knows what he is doing
    rpmprefix=$enable_rpmprefix;;
  esac
])
AC_SUBST(rpmprefix)

AC_ARG_ENABLE([infos],
[  --enable-infos                      enable building infos],[
  case "$enable_infos" in
  yes)
    build_infos=1;;
  no)
    build_infos=0;;
  *)
    AC_MSG_ERROR([Invalid argument to --enable-infos])
  esac
])
AC_SUBST(build_infos)


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

case "${target_os}" in
  rtems|[rtems]RTEMS_API[])
    TARGET_SUBDIRS=rtems
    ;;
  *) # ignore
#    AC_MSG_ERROR([Unsupported target ${target}])
esac
AC_SUBST([TARGET_SUBDIRS])

case "${target_os}" in
  [rtems]RTEMS_API[])
    osversion=[]RTEMS_API[]
    ;;
  *)
  osversion=
esac
AC_SUBST([osversion])

AC_CONFIG_FILES([Makefile])

AC_CONFIG_FILES([cygwin/Makefile])
AC_CONFIG_FILES([mingw32/Makefile])
AC_CONFIG_FILES([solaris2.7/Makefile])
AC_CONFIG_FILES([freebsd5.2/Makefile])
AC_CONFIG_FILES([freebsd6.0/Makefile])
AC_CONFIG_FILES([rtems/Makefile])

AC_OUTPUT
