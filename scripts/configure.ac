## $Id$

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([binutils])
AC_PREFIX_DEFAULT([/opt/rtems])
AM_INIT_AUTOMAKE(rtems-scripts,20010516,no)

AC_CANONICAL_HOST

# Check for .exe suffix.
# With autoconf > 2.13 AC_EXEEXT is not what we want to use.
AC_MSG_CHECKING([for executable suffix])
case "$host" in
i?86-*cygwin*)
  EXEEXT=.exe ;;
esac
AC_SUBST(EXEEXT)
AC_MSG_RESULT([$EXEEXT])

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
BINUTILSVERS="binutils-${binutils_version}"
AC_SUBST(BINUTILSVERS)
GDBVERS="gdb-${gdb_version}"
AC_SUBST(GDBVERS)
BSPVERS="${rtems_version}"
AC_SUBST(RTEMS_VERSION)
AC_SUBST(BSPVERS)
AC_SUBST(RPM_CPU)

AC_CONFIG_FILES([Makefile
buildall
buildalltar
mkbinutilspec
mkgccnewlibspec
mkgdbspec
mkbspspec
binutils/Makefile
gccnewlib/Makefile
gdb/Makefile
rtems/Makefile
autotools/Makefile])
AC_CONFIG_COMMANDS([default],[[chmod 755 buildall
chmod 755 buildalltar
]],[[]])
AC_OUTPUT
