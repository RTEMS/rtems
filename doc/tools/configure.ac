dnl Process this file with autoconf to produce a configure script.
dnl NOTE: This is a very coarse port to automake
AC_PREREQ(2.52)

AC_INIT
AC_CONFIG_SRCDIR([word-replace2])

AM_INIT_AUTOMAKE(rtemsdoc-tools,0.0.0,no)
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
src2html/Makefile
src2html1.4a/Makefile
src2html1.4a/Ctags/Makefile
src2html1.4a/FreeBSD/Makefile
src2html1.4a/ctags-emacs/Makefile
texi2www/Makefile
])
AC_OUTPUT
