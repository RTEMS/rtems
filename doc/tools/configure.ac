dnl Process this file with autoconf to produce a configure script.
dnl NOTE: This is a very coarse port to automake
AC_PREREQ(2.57)

AC_INIT([rtems-doc-tools],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([word-replace2])
AC_CONFIG_AUX_DIR([../..])
RTEMS_VERSIONING

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
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
