m4_define([M4_RTEMS_API],[4.7])
m4_define([M4_RTEMS_VERSION],[4.6.99.3])
m4_define([M4_RTEMS_RELEASE],[0])

AC_PREREQ([2.69])
AC_INIT([rtemsrpms],[M4_RTEMS_RELEASE],[http://www.rtems.org/bugzilla])
AC_CONFIG_AUX_DIR(.)

AC_CANONICAL_HOST

AC_PREFIX_DEFAULT([/opt/rtems-][M4_RTEMS_API])

AM_INIT_AUTOMAKE([1.12.2])

rpmprefix="[rtems-]M4_RTEMS_API[-]"

case "$prefix" in
 /usr/local)
  rpmprefix="local-"
  ;;
 /opt/rtems)
  ;;
 NONE|$ac_default_prefix)
  prefix=$ac_default_prefix
  ;;
 *)
  rpmprefix="custom-"
  ;;
esac

AC_ARG_ENABLE([rpmprefix],
[  --enable-rpmprefix=<rpmprefix>      prefix rpms],[
  case "$enable_rpmprefix" in
  yes) # ignore, use default
    ;;
  no) # use default
    ;;
  *) # presume user knows what he is doing
    rpmprefix=$enable_rpmprefix;;
  esac
])

AC_MSG_CHECKING([for rpm database])
rpmdbpath=
AC_ARG_ENABLE([rpmdbpath],
[  --enable-rpmdbpath=<rpmdbpath>      rpm database path],[
  case "$enable_rpmdbpath" in
  yes) # ignore, use default
    ;;
  no) # use default
    ;;
  *) # presume user knows what he is doing
    rpmdbpath="--dbpath $enable_rpmdbpath";;
  esac
])
AS_IF([test -z $rpmdbpath],
      [AC_MSG_RESULT([system])],
      [AC_MSG_RESULT([$rpmdbpath])])

AC_MSG_CHECKING([for bootstrap])
bootstrap=no
AC_ARG_ENABLE([bootstrap],
[  --enable-bootstrap=<bootstap>      bootstrap RTEMS],[
  case "$enable_bootstrap" in
  yes|no) # ignore, use default
   bootstrap=$enable_bootstrap;;
  *) # use default
   ;;
  esac
])
AC_MSG_RESULT([$bootstrap])

dnl The total list:
dnl  arm avr h8300 i386 m68k mips powerpc sh sparc tic4x
AC_MSG_CHECKING([for targets])
AC_ARG_ENABLE([targets],
  AS_HELP_STRING([--enable-targets=<targets>],[build for targets <targets>]),
  [targets="$enable_targets"],
  [targets="arm i386 m68k mips powerpc sh sparc tic4x"]
)
AC_MSG_RESULT([$targets])

AC_MSG_CHECKING([for bsps])
bsps="%{nil}"
AC_ARG_ENABLE([bsps],
[  --enable-bsps=<yes/no/bsps>    BSP list],[
  case "$enable_bsps" in
  yes) # ignore, use default
   bsps="all"
   ;;
  no | none)
   bsps="%{nil}"
   ;;
  *) # take a the list to build
   bsps="$enable_bsps"
   ;;
  esac
])
AS_IF([test x"$bsps" = x"%{nil}"],
      [AC_MSG_RESULT([none])],
      [AC_MSG_RESULT([$bsps])])

AC_SUBST(RTEMS_API, [M4_RTEMS_API])
AC_SUBST(RTEMS_VERSION, [M4_RTEMS_VERSION])
AC_SUBST(RTEMS_RELEASE, [M4_RTEMS_RELEASE])
AC_SUBST(PREFIX, [$prefix])
AC_SUBST(TARGETS, [$targets])
AC_SUBST(RPMPREFIX, [$rpmprefix])
AC_SUBST(RPMDBPATH, [$rpmdbpath])
AC_SUBST(RTEMS_BOOTSTRAP, [$bootstrap])
AC_SUBST(RTEMS_BSP, [$bsps])

AC_CONFIG_FILES([Makefile rtems.spec])

AC_OUTPUT
