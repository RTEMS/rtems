## $Id$

AC_PREREQ(2.59)
AC_INIT([rtems-contrib-solaris],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([setup])

AM_INIT_AUTOMAKE([no-define foreign 1.8])

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
elif test -d /usr/local/src/redhat/SPECS;
then
  RPM_SPECSdir=/usr/local/src/redhat/SPECS;
fi
if test x"$RPM_SPECSdir" = x"" ; then
AC_MSG_ERROR(not found)
fi
AC_MSG_RESULT($RPM_SPECSdir)
AC_SUBST(RPM_SPECSdir)

BINUTILS_VERS="${binutils_version}"
AC_SUBST(BINUTILS_VERS)
BINUTILS_RPMVERS=`echo ${binutils_version} | tr '-' '_'`
AC_SUBST(BINUTILS_RPMVERS)
BINUTILS_RPMREL="${binutils_rpm_release}"
AC_SUBST(BINUTILS_RPMREL)

MINGW_VERS="${mingw_version}"
AC_SUBST(MINGW_VERS)
MINGW_RPMVERS=`echo ${mingw_version} | tr '-' '_'`
AC_SUBST(MINGW_RPMVERS)
MINGW_RPMREL="${mingw_rpm_release}"
AC_SUBST(MINGW_RPMREL)

W32API_VERS="${w32api_version}"
AC_SUBST(W32API_VERS)
W32API_RPMVERS=`echo ${w32api_version} | tr '-' '_'`
AC_SUBST(W32API_RPMVERS)
W32API_RPMREL="${w32api_rpm_release}"
AC_SUBST(W32API_RPMREL)

SOLARIS_VERS="${solaris_version}"
AC_SUBST(SOLARIS_VERS)
SOLARIS_RPMVERS=`echo ${solaris_version} | tr '-' '_'`
AC_SUBST(SOLARIS_RPMVERS)
SOLARIS_RPMREL="${solaris_rpm_release}"
AC_SUBST(SOLARIS_RPMREL)

GCC_VERS="${gcc_version}"
AC_SUBST(GCC_VERS)
GCC_RPMVERS=`echo ${gcc_version} | tr '-' '_'`
AC_SUBST(GCC_RPMVERS)
GCC_RPMREL="${gcc_rpm_release}"
AC_SUBST(GCC_RPMREL)

RPM_BUILD_ROOT="$rpm_build_root/%{name}-root"
AC_SUBST(RPM_BUILD_ROOT)

AC_CONFIG_FILES([Makefile
sparc-solaris-binutils.spec
sparc-solaris-libs.spec
sparc-solaris-gcc.spec
])
AC_OUTPUT
