## $Id$

m4_define([RTEMS_API],[4.11])

AC_PREREQ([2.68])
AC_INIT([crossrpms],[0.20110310.0],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([binutils/binutils.add])
AC_CONFIG_AUX_DIR(.)

AC_CANONICAL_HOST

AC_PREFIX_DEFAULT([/opt/rtems-][RTEMS_API])

AM_INIT_AUTOMAKE([1.11.1])

AC_ARG_ENABLE([targets],
  AS_HELP_STRING([--enable-targets=<targets>],[build for targets <targets>]),
  [targets="$enable_targets"],
  [targets=rtems]
)

case $targets in
rtems )
  savdir=`pwd`
  cd $srcdir
  targets=
  for d in rtems*/*/Makefile.am; do
      o=`echo $d | sed -e 's,/.*,,'`
      a=`echo $d | sed -e 's,/Makefile\.am,,;s,.*/,,'`
      targets="$targets $a-$o"
  done
  cd $savdir
  ;;
all )
  savdir=`pwd`
  cd $srcdir
  targets=
  for d in rtems*/*/Makefile.am; do
      o=`echo $d | sed -e 's,/.*,,'`
      a=`echo $d | sed -e 's,/Makefile\.am,,;s,.*/,,'`
      targets="$targets $a-$o"
  done
  for d in freebsd*/*/Makefile.am \
    netbsd*/*/Makefile.am \
    cygwin*/*/Makefile.am \
    mingw*/*/Makefile.am; \
  do
      o=`echo $d | sed -e 's,/.*,,'`
      a=`echo $d | sed -e 's,/Makefile\.am,,;s,.*/,,'`
      targets="$targets $a-pc-$o"
  done
  for d in solaris*/*/Makefile.am; \
  do
      o=`echo $d | sed -e 's,/.*,,'`
      a=`echo $d | sed -e 's,/Makefile\.am,,;s,.*/,,'`
      targets="$targets $a-sun-$o"
  done
  cd $savdir
  ;;
esac

case "$prefix" in
/usr)
  sysdirpattern="/%sysdir/d"
  rpmprefix=
  host_subdirs=
  ;;
/usr/local)
  sysdirpattern="/%sysdir/d"
  rpmprefix="local-"
  host_subdirs=autotools
  ;;
/opt/rtems)
  sysdirpattern="s/^%sysdir/%dir/g"
  rpmprefix="rtems-"
  host_subdirs=autotools
  ;;
NONE|${ac_default_prefix}) # Default prefix
  sysdirpattern="s/^%sysdir/%dir/g"
  rpmprefix="[rtems-]RTEMS_API[-]"
  host_subdirs=autotools
  ;;
*)
  sysdirpattern="s/^%sysdir/%dir/g"
  rpmprefix="custom-"
  host_subdirs=autotools
  ;;
esac
AC_SUBST(sysdirpattern)
AC_SUBST(HOST_SUBDIRS,$host_subdirs)

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

SPECSTRIP_OPTS=

AC_ARG_ENABLE([infos],
[  --enable-infos                      enable building infos],[
  case "$enable_infos" in
  yes)
    SPECSTRIP_OPTS="$SPECSTRIP_OPTS --enable-infos";;
  no)
    SPECSTRIP_OPTS="$SPECSTRIP_OPTS --disable-infos";;
  *)
    AC_MSG_ERROR([Invalid argument to --enable-infos])
  esac
])
AC_SUBST(SPECSTRIP_OPTS)

for f in $targets; do
ac_cv_mytarget=`$SHELL "$ac_aux_dir/config.sub" $f` ||
  AC_MSG_ERROR([$SHELL $ac_aux_dir/config.sub $f failed])
  RTEMS_CANONICAL_SPLIT(mytarget)

  AS_IF([test -d $srcdir/$mytarget_os/$mytarget_cpu],[
    target_subdirs="$target_subdirs $mytarget_os/$mytarget_cpu"],[
    AC_MSG_WARN([ignoring unsupported target $f])
    ])
done
AC_SUBST(TARGET_SUBDIRS,$target_subdirs)
AC_SUBST(rtems_api,RTEMS_API)

AC_CONFIG_FILES([Makefile])

AC_CONFIG_FILES([cygwin/Makefile])
AC_CONFIG_FILES([cygwin/i686/Makefile])

AC_CONFIG_FILES([mingw32/Makefile])
AC_CONFIG_FILES([mingw32/i686/Makefile])

AC_CONFIG_FILES([solaris2.7/Makefile])
AC_CONFIG_FILES([solaris2.7/sparc/Makefile])

AC_CONFIG_FILES([freebsd6.4/Makefile])
AC_CONFIG_FILES([freebsd6.4/i586/Makefile])

AC_CONFIG_FILES([freebsd7.3/Makefile])
AC_CONFIG_FILES([freebsd7.3/i586/Makefile])

AC_CONFIG_FILES([freebsd8.2/Makefile])
AC_CONFIG_FILES([freebsd8.2/i586/Makefile])
AC_CONFIG_FILES([freebsd8.2/x86_64/Makefile])

AC_CONFIG_FILES([netbsd5.1/Makefile])
AC_CONFIG_FILES([netbsd5.1/i386/Makefile])
AC_CONFIG_FILES([netbsd5.1/x86_64/Makefile])

AC_CONFIG_FILES([rtems4.11/Makefile])
AC_CONFIG_FILES([rtems4.11/arm/Makefile])
AC_CONFIG_FILES([rtems4.11/avr/Makefile])
AC_CONFIG_FILES([rtems4.11/bfin/Makefile])
AC_CONFIG_FILES([rtems4.11/h8300/Makefile])
AC_CONFIG_FILES([rtems4.11/i386/Makefile])
AC_CONFIG_FILES([rtems4.11/lm32/Makefile])
AC_CONFIG_FILES([rtems4.11/m32c/Makefile])
AC_CONFIG_FILES([rtems4.11/m32r/Makefile])
AC_CONFIG_FILES([rtems4.11/m68k/Makefile])
AC_CONFIG_FILES([rtems4.11/mips/Makefile])
AC_CONFIG_FILES([rtems4.11/mipstx39/Makefile])
AC_CONFIG_FILES([rtems4.11/powerpc/Makefile])
AC_CONFIG_FILES([rtems4.11/sh/Makefile])
AC_CONFIG_FILES([rtems4.11/sparc/Makefile])
AC_CONFIG_FILES([rtems4.11/sparc64/Makefile])

AC_CONFIG_FILES([autotools/Makefile])

AC_OUTPUT
