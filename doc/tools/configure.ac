dnl Process this file with autoconf to produce a configure script.
dnl NOTE: This is a very coarse port to automake
AC_PREREQ([2.68])

AC_INIT([rtems-doc-tools],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([word-replace2])
RTEMS_TOP([../..])

AM_INIT_AUTOMAKE([no-define foreign 1.11.1])
dnl Checks for programs.
AC_PROG_CC

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(limits.h)

dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.
AC_CHECK_FUNCS(strdup strstr)

AC_CONFIG_FILES([Makefile
bmenu/Makefile
texi2www/Makefile
])
AC_OUTPUT
