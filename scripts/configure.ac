## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-scripts],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([binutils])
RTEMS_TOP([..])
AC_PREFIX_DEFAULT([/opt/rtems])
AM_INIT_AUTOMAKE([no-define foreign 1.7.2])

AC_CANONICAL_HOST

test -f ./setup.cache || cp ${srcdir}/setup.def ./setup.cache
. ./setup.cache

AC_MSG_CHECKING(for rpm SPECS directory)
# Allow users to override RPM_SPECSdir from the environment.
if test -z "$RPM_SPECSdir" ;
then
# SuSE uses /usr/src/packages
# redhat uses /usr/src/redhat
# MDK is reported to use /usr/src/RPM
# others might use /usr/src
  rpmpath="/usr:/usr/local"
  rpmdirs="src/packages:src/redhat:src/RPM:src"
  save_IFS="$IFS"; IFS=":"
  for d in $rpmdirs; do
    for p in $rpmpath; do
      list="$p/$d:$list"
    done
  done
  for d in $list; do
    if test -d $d/SPECS; then
      RPM_SPECSdir="$d/SPECS";
      break;
    fi
  done
  IFS="$save_IFS"
fi

if test x"$RPM_SPECSdir" = x"" ; then
AC_MSG_ERROR([not found])
fi
AC_MSG_RESULT($RPM_SPECSdir)
AC_SUBST(RPM_SPECSdir)

AC_MSG_CHECKING(for RPM CPU type)
case "$host" in
  i?86-*linux*)  	RPM_CPU=i386 ;;
  i?86-*cygwin*) 	RPM_CPU=i386 ;;
  sparc-*solaris*)      RPM_CPU=sparc ;;
  *)
    AC_MSG_ERROR([RPM CPU for ${host} is unknown])
    ;;
esac
AC_MSG_RESULT($RPM_CPU)

GCCNEWLIBVERS="gcc-${gcc_version}-newlib-${newlib_version}"
AC_SUBST(GCCNEWLIBVERS)
GCC3NEWLIBVERS="gcc-${gcc3_version}-newlib-${gcc3newlib_version}"
AC_SUBST(GCC3NEWLIBVERS)
BINUTILSVERS="binutils-${binutils_version}"
AC_SUBST(BINUTILSVERS)
GDBVERS="gdb-${gdb_version}"
AC_SUBST(GDBVERS)
BSPVERS="${rtems_version}"
AC_SUBST(RTEMS_VERSION)
AC_SUBST(BSPVERS)
AC_SUBST(RPM_CPU)

AC_CONFIG_FILES([Makefile
binutils/Makefile
gccnewlib/Makefile
gcc3newlib/Makefile
gdb/Makefile
rtems/Makefile
autotools/Makefile
autotools/automake-rtems.spec
autotools/autoconf-rtems.spec
cpukit/Makefile
rtemsdoc/Makefile
])

AC_CONFIG_FILES([common/common.add common/clean.add])
AC_CONFIG_FILES([buildalltar],[chmod +x buildalltar])
AC_CONFIG_FILES([buildall],[chmod +x buildall])
AC_CONFIG_FILES([gdb/mkspec],[chmod +x gdb/mkspec])
AC_CONFIG_FILES([binutils/mkspec],[chmod +x binutils/mkspec])
AC_CONFIG_FILES([mkgccnewlibspec],[chmod +x mkgccnewlibspec])
AC_CONFIG_FILES([mkbspspec],[chmod +x mkbspspec])
AC_CONFIG_FILES([gcc3newlib/mkspec],[chmod +x gcc3newlib/mkspec])
AC_CONFIG_FILES([cpukit/mkspec],[chmod +x cpukit/mkspec])
AC_CONFIG_FILES([rtemsdoc/mkspec],[chmod +x rtemsdoc/mkspec])

AC_CONFIG_COMMANDS([default],[[chmod 755 buildall
chmod 755 buildalltar
]],[[]])
AC_OUTPUT
