## Process this file with autoconf to produce a configure script.
## 
## $Id$

## 
## This script configures the configuration files below make/
## 

AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR([bsp.cfg.in])
RTEMS_TOP(../../..)
AC_CONFIG_AUX_DIR(../../..)

RTEMS_CANONICAL_TARGET_CPU

AM_INIT_AUTOMAKE(rtems-c-src-make,$RTEMS_VERSION,no)
AM_MAINTAINER_MODE

RTEMS_ENABLE_MULTIPROCESSING
RTEMS_ENABLE_POSIX
RTEMS_ENABLE_ITRON
RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_RDBG
RTEMS_ENABLE_INLINES
RTEMS_ENABLE_CXX
RTEMS_ENABLE_GCC28
RTEMS_ENABLE_BARE

RTEMS_ENV_RTEMSBSP

## BEGIN configure.host.in

AC_PATH_PROG(CAT,cat)
AC_PATH_PROG(CP,cp)
AC_PATH_PROG(MV,mv)
AC_PATH_PROG(LN,ln)
AC_PROG_LN_S
AC_PATH_PROG(CHMOD,chmod)
AC_PATH_PROG(SORT,sort)
RTEMS_PATH_PERL

AC_PATH_PROG(TOUCH,touch)
AC_PATH_PROG(CMP,cmp)

AC_PATH_PROG(SED,sed)
AC_PATH_PROGS(M4,gm4 m4)

RTEMS_PATH_KSH

## BEGIN configure.target.in

RTEMS_CHECK_CPU
RTEMS_CANONICAL_HOST

RTEMS_PROJECT_ROOT

RTEMS_PROG_CC_FOR_TARGET

## check for g++
if test "$RTEMS_HAS_CPLUSPLUS" = "yes"; then
  RTEMS_PROG_CXX_FOR_TARGET
fi

RTEMS_CANONICALIZE_TOOLS

## if this is an i386, does gas have good code16 support?
RTEMS_I386_GAS_CODE16

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
RTEMS_BSP_ALIAS(${RTEMS_BSP})
RTEMS_CHECK_MULTIPROCESSING(RTEMS_BSP)
RTEMS_CHECK_NETWORKING(RTEMS_BSP)
RTEMS_CHECK_RDBG(RTEMS_BSP)
RTEMS_CHECK_POSIX_API(RTEMS_BSP)
RTEMS_CHECK_CXX(RTEMS_BSP)
RTEMS_CHECK_ITRON_API(RTEMS_BSP)

AC_SUBST(rtems_cv_prog_cc_cross)
AC_SUBST(RTEMS_HAS_CPLUSPLUS)
AC_SUBST(RTEMS_USE_GCC272)
AC_SUBST(BARE_CPU_CFLAGS)
AC_SUBST(BARE_CPU_MODEL)
AC_SUBST(program_prefix)
AC_SUBST(CC_CFLAGS_DEFAULT)
AC_SUBST(CC_CFLAGS_DEBUG_V)
AC_SUBST(CC_CFLAGS_PROFILE_V)
AC_SUBST(CC_LDFLAGS_PROFILE_V)

# Collect the files needed to build a BSP
cfg_dir="${RTEMS_TOPdir}/make"
CUSTOM_CFG_FILES=""
f="${RTEMS_BSP}.cfg"
while test -n "$f"; do
  if test -r "${srcdir}/${cfg_dir}/custom/$f"; then
    AC_CONFIG_LINKS([custom/$f:${cfg_dir}/custom/$f])
    CUSTOM_CFG_FILES="$CUSTOM_CFG_FILES $f"
    f=`grep "^include.*make/custom" ${srcdir}/${cfg_dir}/custom/$f \
    | sed \
      -e 's%^.*custom\/%%' \
      -e s%\$\(RTEMS_HOST\)%${RTEMS_HOST}% \
      -e 's%default\.cfg%%'`;
  else break; fi;
done
AC_SUBST(CUSTOM_CFG_FILES)
AC_SUBST(cfg_dir)

# Symlink the *.cfg files from the toplevel make directory into the build
# trees c/<bsp>/make subdirectories

AC_CONFIG_LINKS([
  Templates/Makefile.lib:${cfg_dir}/Templates/Makefile.lib
  Templates/Makefile.leaf:${cfg_dir}/Templates/Makefile.leaf
  Templates/Makefile.dir:${cfg_dir}/Templates/Makefile.dir
  compilers/gcc-target-default.cfg:${cfg_dir}/compilers/gcc-target-default.cfg
  directory.cfg:${cfg_dir}/directory.cfg
  lib.cfg:${cfg_dir}/lib.cfg
  README:${cfg_dir}/README
  leaf.cfg:leaf.cfg
],,[
  RTEMS_TOPdir=${RTEMS_TOPdir}
  cfg_dir=${cfg_dir}
  test -d Templates || mkdir Templates
])

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile
Makefile.inc
host.cfg
bsp.cfg
custom/Makefile
custom/default.cfg
compilers/Makefile])
AC_CONFIG_COMMANDS([default],[[${MAKE} $RTEMS_BSP.cache]],[[MAKE=${MAKE-make}
RTEMS_BSP=${RTEMS_BSP}
]])
AC_OUTPUT
