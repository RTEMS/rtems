## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-scripts-ada],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([binutils])
AM_INIT_AUTOMAKE([no-define foreign 1.6])

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

# Some linux distributions use /usr/src/packages
# redhat uses /usr/src/redhat
# others might use /usr/src
AC_MSG_CHECKING(for rpm SPECS directory)
if test -d /usr/src/packages/SPECS;
then
  RPM_SPECSdir=/usr/src/packages/SPECS;
elif test -d /usr/src/redhat/SPECS;
then
  RPM_SPECSdir=/usr/src/redhat/SPECS;
elif test -d /usr/src/SPECS;
then
  RPM_SPECSdir=/usr/src/SPECS;
fi
if test x"$RPM_SPECSdir" = x"" ; then
AC_MSG_ERROR(not found)
fi
AC_MSG_RESULT($RPM_SPECSdir)
AC_SUBST(RPM_SPECSdir)

GNATNEWLIBVERS="gnat-${gnat_version}-newlib-${newlib_version}"
AC_SUBST(GNATNEWLIBVERS)
BINUTILSVERS="gnat-${gnat_version}-binutils-${binutils_version}"
AC_SUBST(BINUTILSVERS)
GDBVERS="gnat-${gnat_version}-gdb-${gdb_version}"
AC_SUBST(GDBVERS)
BSPVERS="${rtems_version}"
AC_SUBST(RTEMS_VERSION)
AC_SUBST(BSPVERS)

AC_CONFIG_FILES([Makefile
buildall
mkbinutilspec
mkgnatnewlibspec
mkgdbspec
mkbspspec
binutils/Makefile
gnatnewlib/Makefile
gdb/Makefile])
AC_CONFIG_COMMANDS([default],[[]],[[chmod 755 buildall
]])
AC_OUTPUT
