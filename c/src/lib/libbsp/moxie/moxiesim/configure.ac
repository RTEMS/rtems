## Process this file with autoconf to produce a configure script.
##
## $Id: configure.ac,v 1.20 2009/11/28 06:28:39 ralf Exp $

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-moxie-moxiesim],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP
RTEMS_BSP_LINKCMDS

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE


RTEMS_BSP_CLEANUP_OPTIONS

AC_MSG_CHECKING([for old moxie assembly syntax])
AC_COMPILE_IFELSE(
  [AC_LANG_PROGRAM(
    [],
    [asm("sub.l \$r0, \$r0");])],
  [HAVE_OLD_MOXIE_ASM=yes],
  [HAVE_OLD_MOXIE_ASM=no])
AC_MSG_RESULT([$HAVE_OLD_MOXIE_ASM])
if test "x$HAVE_OLD_MOXIE_ASM" = xyes; then
  AC_DEFINE(HAVE_OLD_MOXIE_ASM, 
            1, [Define if you are using old moxie asm syntax (sub.l)])
fi

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
