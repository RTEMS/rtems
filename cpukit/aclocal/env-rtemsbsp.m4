dnl $Id$

dnl Pass a single BSP via an environment variable
dnl used by per BSP configure scripts
AC_DEFUN(RTEMS_ENV_RTEMSBSP,
[dnl
AC_BEFORE([$0], [RTEMS_ENABLE_RTEMSBSP])dnl
AC_BEFORE([$0], [RTEMS_PROJECT_ROOT])dnl

AC_ARG_VAR([CPU_CFLAGS],[])
AC_ARG_VAR([CFLAGS_OPTIMIZE_V],[])
AC_ARG_VAR([CFLAGS_DEBUG_V],[])
AC_ARG_VAR([CFLAGS_PROFILE_V],[])

AC_ARG_VAR([RTEMS_BSP],[RTEMS_BSP to build])
AC_MSG_CHECKING([for RTEMS_BSP])
AC_CACHE_VAL(rtems_cv_RTEMS_BSP,
[dnl
  test -n "${RTEMS_BSP}" && rtems_cv_RTEMS_BSP="$RTEMS_BSP";
])dnl
if test -z "$rtems_cv_RTEMS_BSP"; then
  AC_MSG_ERROR([Missing RTEMS_BSP])
fi
RTEMS_BSP="$rtems_cv_RTEMS_BSP"
AC_MSG_RESULT(${RTEMS_BSP})
AC_SUBST(RTEMS_BSP)

PROJECT_INCLUDE="\$(PROJECT_ROOT)/$RTEMS_BSP/lib/include"
AC_SUBST(PROJECT_INCLUDE)

project_libdir="${PROJECT_ROOT}/$RTEMS_BSP/lib"   
AC_SUBST(project_libdir)

RTEMS_ROOT="$PROJECT_ROOT/c/$RTEMS_BSP"
AC_SUBST(RTEMS_ROOT)

GCC_SPECS="-isystem \$(PROJECT_INCLUDE)"
AC_SUBST(GCC_SPECS)

RTEMS_ENABLE_BARE
AC_SUBST(BARE_CPU_MODEL)
AC_SUBST(BARE_CPU_CFLAGS)

AM_CONDITIONAL([MULTILIB],[false])

includedir="\${exec_prefix}/${RTEMS_BSP}/lib/include"
libdir="\${exec_prefix}/${RTEMS_BSP}/lib"
])
