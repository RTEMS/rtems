## Process this file with autoconf to produce a configure script.

##
## This script configures the configuration files below make/
##

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-make],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp.cfg.in])
RTEMS_TOP(../../..)
AC_SUBST(RTEMS_TOPdir,../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE([no-define foreign 1.12.2])
AM_MAINTAINER_MODE

RTEMS_VERSIONING
RTEMS_ENABLE_MULTIPROCESSING
RTEMS_ENABLE_POSIX
RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_CXX

RTEMS_ENV_RTEMSBSP

AC_PATH_PROG(CAT,cat)
AC_PATH_PROG(CP,cp)
AC_PATH_PROG(MV,mv)
AC_PATH_PROG(LN,ln)
AC_PROG_LN_S
AC_PATH_PROG(CHMOD,chmod)
AC_PATH_PROG(SORT,sort)

AC_PATH_PROG(TOUCH,touch)
AC_PATH_PROG(CMP,cmp)

AC_PATH_PROG(SED,sed)
AC_PATH_PROGS(M4,gm4 m4)

RTEMS_PATH_KSH

## BEGIN configure.target.in

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET

## check for g++
RTEMS_PROG_CXX_FOR_TARGET

RTEMS_CANONICALIZE_TOOLS
RTEMS_CHECK_TOOL(SIZE,size,:)
RTEMS_CHECK_TOOL(NM,nm,:)
RTEMS_CHECK_TOOL(OBJCOPY,objcopy,:)

## if this is an i386, does gas have good code16 support?
RTEMS_I386_GAS_CODE16

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

RTEMS_CHECK_MULTIPROCESSING(RTEMS_BSP)
RTEMS_CHECK_NETWORKING(RTEMS_BSP)
RTEMS_CHECK_POSIX_API(RTEMS_BSP)
RTEMS_CHECK_CXX(RTEMS_BSP)

AC_SUBST(rtems_cv_prog_cc_cross)
AC_SUBST(RTEMS_HAS_CPLUSPLUS)
AC_SUBST(program_prefix)
AC_SUBST(CC_CFLAGS_DEFAULT)
AC_SUBST(CC_CFLAGS_DEBUG_V)
AC_SUBST(RTEMS_API,[_RTEMS_API])

cfg_dir="${RTEMS_TOPdir}/make"
AC_SUBST(cfg_dir)

# Symlink leaf.cfg from the source directory into the build directory.
AC_CONFIG_LINKS([leaf.cfg:leaf.cfg])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
])
AC_OUTPUT
