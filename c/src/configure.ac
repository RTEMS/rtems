## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.52)
AC_INIT([rtems-c-src],[_RTEMS_VERSION],[rtems-bugs@OARcorp.com])
AC_CONFIG_SRCDIR([optman])
RTEMS_TOP(../..)
AC_CONFIG_AUX_DIR(../..)

RTEMS_ENABLE_CXX
RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_RDBG

RTEMS_CANONICAL_TARGET_CPU
RTEMS_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define foreign 1.6])
AM_MAINTAINER_MODE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS

RTEMS_ENABLE_MULTILIB
RTEMS_ENABLE_MULTIPROCESSING
RTEMS_ENV_RTEMSBSP
RTEMS_CHECK_CPU
RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

RTEMS_CHECK_NETWORKING(RTEMS_BSP)

AC_MSG_NOTICE([setting up make/custom])

AS_MKDIR_P([make/custom])

# Collect and copy the BSP's make/custom/*.cfg files from the toplevel make 
# directory into the build tree's <bsp>/make/custom subdirectories
cfg_dir="${RTEMS_TOPdir}/make"
CUSTOM_CFG_FILES=""
f="${RTEMS_BSP}.cfg"
while test -n "$f"; do
  if test -r "${srcdir}/${cfg_dir}/custom/$f"; then
    cp ${srcdir}/${cfg_dir}/custom/$f make/custom/$f
    CUSTOM_CFG_FILES="$CUSTOM_CFG_FILES make/custom/$f"
    f=`grep "^include.*make/custom" ${srcdir}/${cfg_dir}/custom/$f \
    | sed \
      -e 's%^.*custom\/%%' \
      -e s%\$\(RTEMS_HOST\)%${RTEMS_HOST}% \
      -e 's%default\.cfg%%'`;
  else break; fi;
done
cp ${srcdir}/make/custom/default.cfg.in make/custom/default.cfg

AC_MSG_NOTICE([creating make/${RTEMS_BSP}.cache])
${MAKE-make} -f ${srcdir}/make/bsp.mak \
RTEMS_BSP=${RTEMS_BSP} \
RTEMS_HOST=${RTEMS_HOST} \
BARE_CPU_MODEL=${BARE_CPU_MODEL} \
RTEMS_ROOT=. make/${RTEMS_BSP}.cache

# Read back the cache
RTEMS_CHECK_BSP_CACHE(RTEMS_BSP)

AC_SUBST(CUSTOM_CFG_FILES)
AC_SUBST(cfg_dir)

## Configure ${srcdir}/../../cpukit as exec/ if multilibs are enabled
AS_IF([test x"$multilib" = x"no"],[
  RTEMS_CHECK_MULTIPROCESSING
  RTEMS_CONFIG_SUBDIR([exec],[../../cpukit],
  [--with-project-root="../../" --enable-rtemsbsp=$RTEMS_BSP
  ])
  BSP_SUBDIRS="exec"
])

## Note: the order of the directories below is essential
AC_CONFIG_SUBDIRS([make])
BSP_SUBDIRS="$BSP_SUBDIRS make"
AC_CONFIG_SUBDIRS([optman])
BSP_SUBDIRS="$BSP_SUBDIRS optman"
AC_CONFIG_SUBDIRS([lib/libcpu])
BSP_SUBDIRS="$BSP_SUBDIRS lib/libcpu"
AC_CONFIG_SUBDIRS([lib/libbsp])
BSP_SUBDIRS="$BSP_SUBDIRS lib/libbsp"
AC_CONFIG_SUBDIRS([libmisc])
BSP_SUBDIRS="$BSP_SUBDIRS libmisc"

AS_IF([test "$RTEMS_CPU" != "unix"],[
## HACK: Suppress libchip for unix
  AC_CONFIG_SUBDIRS([libchip])
  BSP_SUBDIRS="$BSP_SUBDIRS libchip"
])

AS_IF([test "$HAS_NETWORKING" = "yes"],[
  AC_CONFIG_SUBDIRS(libnetworking)
  BSP_SUBDIRS="$BSP_SUBDIRS libnetworking"
  AS_IF([test "$RTEMS_HAS_RDBG" = "yes"],[
    RTEMS_CHECK_RDBG(RTEMS_BSP)
  ])
])

AS_IF([test "$HAS_RDBG" = "yes"],[
  AC_CONFIG_SUBDIRS([librdbg])
  BSP_SUBDIRS="$BSP_SUBDIRS librdbg"
])

AS_IF([test "$RTEMS_HAS_CPLUSPLUS" = "yes"],[
  AC_CONFIG_SUBDIRS([librtems++])
  BSP_SUBDIRS="$BSP_SUBDIRS librtems++"
])

BSP_SUBDIRS="$BSP_SUBDIRS wrapup"
AC_CONFIG_SUBDIRS([tests])
BSP_SUBDIRS="$BSP_SUBDIRS tests"

AC_SUBST([BSP_SUBDIRS],[$BSP_SUBDIRS])

AM_CONDITIONAL([HAS_NETWORKING],[test "$HAS_NETWORKING" = "yes"])
AM_CONDITIONAL([HAS_RDBG],[test "$HAS_RDBG" = "yes"])

# The posix bsp doesn't support libchip
# FIXME: We should use a feature based check, here
AM_CONDITIONAL([LIBCHIP],[test "$RTEMS_BSP_FAMILY" != "posix"])

# The bare bsp doesn't have libbsp.a
# FIXME: We should use a feature based check, here
AM_CONDITIONAL([HAS_LIBBSP],[! test "$RTEMS_BSP_FAMILY" = "bare"])
AM_CONDITIONAL([MULTILIB],[test x"${enable_multilib}" = x"yes"])

RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile],
[${MAKE} make/${RTEMS_BSP}.cache],
[RTEMS_BSP=${RTEMS_BSP}
 MAKE=${MAKE}])

AC_CONFIG_FILES([wrapup/Makefile])

AC_OUTPUT
