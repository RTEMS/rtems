## $Id$

m4_define([M4_RTEMS_API],[4.7])
m4_define([M4_RTEMS_RELEASE],[0])

AC_PREREQ(2.59)
AC_INIT([rtemsrpms],[M4_RTEMS_RELEASE],[rtems-bugs@rtems.com])
AC_CONFIG_AUX_DIR(.)

AC_CANONICAL_HOST

AC_PREFIX_DEFAULT([/opt/rtems-][M4_RTEMS_API])

AM_INIT_AUTOMAKE([1.9])

AC_ARG_ENABLE([targets],
  AS_HELP_STRING([--enable-targets=<targets>],[build for targets <targets>]),
  [targets="$enable_targets"],
  [targets="i386 m68k powerpc sparc arm mips"]
)

rpmprefix="[rtems-]M4_RTEMS_API[-]"

case "$prefix" in
 /usr/local)
  rpmprefix="local-"
  ;;
 /opt/rtems)
  ;;
 NONE|$ac_default_prefix)
  prefix=$ac_default_prefix
  rpmprefix="rtems-"
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

rpmdbpath=
AC_ARG_ENABLE([rpmdbpath],
[  --enable-rpmdbpath=<rpmdbpath>      rpm database path],[
  case "$enable_rpmdbpath" in
  yes) # ignore, use default
    ;;
  no) # use default
    ;;
  *) # presume user knows what he is doing
    rpmdbpath=$enable_rpmdbpath;;
  esac
])
rpmdbpath="--dbpath $rpmdbpath"

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

bsps="%{nil}"
AC_ARG_ENABLE([bsps],
[  --enable-bsps=<yes/bsps>      BSP list],[
  case "$enable_bsps" in
  yes) # ignore, use default
   ;;
  no)
   bsps="RTEMS_BSPS=\"\""
   ;;
  *) # take a the list to build
   bsps="RTEMS_BSPS=\"$enable_bsps\""
   ;;
  esac
])

AC_SUBST(RTEMS_API, [M4_RTEMS_API])
AC_SUBST(RTEMS_RELEASE, [M4_RTEMS_RELEASE])
AC_SUBST(PREFIX, [$prefix])
AC_SUBST(TARGETS, [$targets])
AC_SUBST(RPMPREFIX, [$rpmprefix])
AC_SUBST(RPMDBPATH, [$rpmdbpath])
AC_SUBST(RTEMS_BOOTSTRAP, [$bootstrap])
AC_SUBST(RTEMS_BSPS, [$bsps])

AC_CONFIG_FILES([Makefile rtems.specs])

AC_OUTPUT
