## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.57)
AC_INIT([rtems-c-src],[_RTEMS_VERSION],[rtems-bugs@rtems.com])
AC_CONFIG_SRCDIR([optman])
RTEMS_TOP(../..)

RTEMS_ENABLE_CXX
RTEMS_ENABLE_NETWORKING
RTEMS_ENABLE_RDBG

RTEMS_CANONICAL_TARGET_CPU
RTEMS_CANONICAL_HOST

AM_INIT_AUTOMAKE([no-define foreign 1.7.2])
AM_MAINTAINER_MODE

RTEMS_ENABLE_MULTILIB
RTEMS_ENABLE_MULTIPROCESSING
RTEMS_ENV_RTEMSBSP

# Is this a supported CPU?
AC_MSG_CHECKING([if cpu $RTEMS_CPU is supported])
# FIXME: Temporary hack
if test -d "$srcdir/lib/libbsp/$RTEMS_CPU"; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_ERROR(no)
fi

RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)

RTEMS_CHECK_NETWORKING(RTEMS_BSP)
RTEMS_CHECK_POSIX_API(RTEMS_BSP)
AS_IF([test "$HAS_NETWORKING" = "yes"],[
  AS_IF([test "$RTEMS_HAS_RDBG" = "yes"],[
    RTEMS_CHECK_RDBG(RTEMS_BSP)
  ])
])

AC_CHECK_PROG(RPCGEN,rpcgen,rpcgen,no)
AC_PROG_AWK

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
AC_CACHE_CHECK([for RTEMS_CPU_MODEL], [ac_cv_env_RTEMS_CPU_MODEL],
[. ./make/${RTEMS_BSP}.cache])
RTEMS_CPU_MODEL=$ac_cv_env_RTEMS_CPU_MODEL
AC_SUBST(RTEMS_CPU_MODEL)
test -n "${RTEMS_CPU_MODEL}" && \
  ac_configure_args="$ac_configure_args 'RTEMS_CPU_MODEL=${RTEMS_CPU_MODEL}'"

AC_CACHE_CHECK([for RTEMS_BSP_FAMILY],[ac_cv_env_RTEMS_BSP_FAMILY],
[. ./make/${RTEMS_BSP}.cache])
RTEMS_BSP_FAMILY=$ac_cv_env_RTEMS_BSP_FAMILY
AC_SUBST(RTEMS_BSP_FAMILY)
test -n "${RTEMS_BSP_FAMILY}" && \
  ac_configure_args="$ac_configure_args 'RTEMS_BSP_FAMILY=${RTEMS_BSP_FAMILY}'"

AC_CACHE_CHECK([for CPU_CFLAGS],[ac_cv_env_CPU_CFLAGS],
[. ./make/${RTEMS_BSP}.cache])
CPU_CFLAGS=$ac_cv_env_CPU_CFLAGS
AC_SUBST(CPU_CFLAGS)
test -n "${CPU_CFLAGS}" && \
  ac_configure_args="$ac_configure_args 'CPU_CFLAGS=${CPU_CFLAGS}'"

AC_CACHE_CHECK([for CFLAGS_OPTIMIZE_V],[ac_cv_env_CFLAGS_OPTIMIZE_V],
[. ./make/${RTEMS_BSP}.cache])
CFLAGS_OPTIMIZE_V=$ac_cv_env_CFLAGS_OPTIMIZE_V
AC_SUBST(CFLAGS_OPTIMIZE_V)
test -n "${CFLAGS_OPTIMIZE_V}" && \
  ac_configure_args="$ac_configure_args 'CFLAGS_OPTIMIZE_V=${CFLAGS_OPTIMIZE_V}'"

AC_CACHE_CHECK([for CFLAGS_DEBUG_V],[ac_cv_env_CFLAGS_DEBUG_V],
[. ./make/${RTEMS_BSP}.cache])
CFLAGS_DEBUG_V=$ac_cv_env_CFLAGS_DEBUG_V
AC_SUBST(CFLAGS_DEBUG_V)
test -n "${CFLAGS_DEBUG_V}" && \
  ac_configure_args="$ac_configure_args 'CFLAGS_DEBUG_V=${CFLAGS_DEBUG_V}'"

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS


AC_SUBST(CUSTOM_CFG_FILES)
AC_SUBST(cfg_dir)

## Configure ${srcdir}/../../cpukit as exec/ if multilibs are disabled
AS_IF([test x"$multilib" = x"no"],[
  RTEMS_CHECK_MULTIPROCESSING
  RTEMS_CONFIG_SUBDIR([exec],[../../cpukit],
  ['--with-project-root=../../' '--enable-rtemsbsp=$RTEMS_BSP'
  ])
  BSP_SUBDIRS="exec"
])

## Note: the order of the directories below is essential
AC_CONFIG_SUBDIRS([make])
BSP_SUBDIRS="$BSP_SUBDIRS make"
BSP_SUBDIRS="$BSP_SUBDIRS optman"

RTEMS_CPU_SUBDIRS(lib/libcpu)
if test -d "${srcdir}/lib/libcpu/${RTEMS_CPU}"; then
  libcpu_cpu_subdir="${RTEMS_CPU}"
else
  libcpu_cpu_subdir=
fi
AC_SUBST(libcpu_cpu_subdir)

# Is there code where there should be for this BSP?
# The bare bsp is a special case as it is not under the RTEMS_CPU path
if test "$RTEMS_BSP_FAMILY" = "bare" ; then
  AC_CONFIG_SUBDIRS([lib/libbsp/bare])
  AC_SUBST(libbsp_cpu_subdir,bare)
else
  RTEMS_CPU_SUBDIRS([lib/libbsp])
  AC_SUBST(libbsp_cpu_subdir,$RTEMS_CPU)
fi

BSP_SUBDIRS="$BSP_SUBDIRS lib"
BSP_SUBDIRS="$BSP_SUBDIRS libchip"
BSP_SUBDIRS="$BSP_SUBDIRS libnetworking"
BSP_SUBDIRS="$BSP_SUBDIRS librdbg"
                                                         
AS_IF([test "$RTEMS_HAS_CPLUSPLUS" = "yes"],[
  AC_CONFIG_SUBDIRS([librtems++])
  BSP_SUBDIRS="$BSP_SUBDIRS librtems++"
])

BSP_SUBDIRS="$BSP_SUBDIRS support"
BSP_SUBDIRS="$BSP_SUBDIRS wrapup"

AC_ARG_ENABLE([ada],
[AC_HELP_STRING([--enable-ada],[enable ada support])],
[case "${enable_ada}" in
  yes) ;;
  no) ;;
  *)  AC_MSG_ERROR(bad value ${enable_ada} for --enable-ada) ;;
esac],[enable_ada=no])

AS_IF([test x"$enable_ada" = x"yes"],[
  AS_IF([test -d ${srcdir}/ada-tests],[
    AC_CONFIG_SUBDIRS([ada-tests])
    BSP_SUBDIRS="$BSP_SUBDIRS ada-tests"
  ])
])

AS_IF([test -d ${srcdir}/tests],[
    AC_CONFIG_SUBDIRS([tests])
    BSP_SUBDIRS="$BSP_SUBDIRS tests"
])

AC_SUBST([BSP_SUBDIRS],[$BSP_SUBDIRS])

AS_IF([test "$host_cpu" = "sparc"],[
  AC_DEFINE_UNQUOTED([CPU_U32_FIX],[1],
    [whether to enable ipalignment work-around])
])

AM_CONDITIONAL([HAS_NETWORKING],[test "$HAS_NETWORKING" = "yes"])
AM_CONDITIONAL([HAS_RDBG],[test "$HAS_RDBG" = "yes"])
AM_CONDITIONAL(HAS_POSIX,test "$HAS_POSIX_API" = "yes")
AM_CONDITIONAL([HAS_MP],[test "$HAS_MP" = "yes"])

# The posix bsp doesn't support libchip
# FIXME: We should use a feature based check, here
AM_CONDITIONAL([LIBCHIP],[test "$RTEMS_BSP_FAMILY" != "posix"])

# The bare bsp doesn't have libbsp.a
# FIXME: We should use a feature based check, here
AM_CONDITIONAL([HAS_LIBBSP],[test "$RTEMS_BSP_FAMILY" != "bare"])
AM_CONDITIONAL([MULTILIB],[test x"${enable_multilib}" = x"yes"])

AM_CONDITIONAL(RPCTOOLS, test "$rpctools" != "no")

RTEMS_PROJECT_ROOT

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile],
[${MAKE} make/${RTEMS_BSP}.cache],
[RTEMS_BSP=${RTEMS_BSP}
 MAKE=${MAKE}])

AC_CONFIG_FILES([
support/Makefile 

libnetworking/Makefile
libnetworking/pppd/Makefile
libnetworking/rtems_servers/Makefile
libnetworking/rtems_telnetd/Makefile
libnetworking/rtems_webserver/Makefile
libnetworking/wrapup/Makefile

librdbg/Makefile
librdbg/include/Makefile
librdbg/include/rdbg/Makefile
librdbg/include/rdbg/i386/Makefile
librdbg/include/rdbg/powerpc/Makefile
librdbg/include/rdbg/m68k/Makefile
librdbg/src/Makefile
librdbg/src/i386/Makefile
librdbg/src/i386/any/Makefile
librdbg/src/powerpc/Makefile
librdbg/src/powerpc/new_exception_processing/Makefile
librdbg/src/m68k/Makefile
librdbg/src/m68k/any/Makefile

libchip/Makefile
libchip/network/Makefile
libchip/rtc/Makefile
libchip/serial/Makefile
libchip/ide/Makefile

optman/Makefile

lib/Makefile

lib/libcpu/Makefile

lib/libbsp/Makefile
lib/libbsp/shmdr/Makefile
lib/libbsp/shared/Makefile

wrapup/Makefile
])

AC_OUTPUT
