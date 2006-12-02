## Process this file with autoconf to produce a configure script.
## 
## configure.ac,v 1.6.2.2 2003/08/11 14:38:01 ralf Exp

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-mips-rbtx4938],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.10])

RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
