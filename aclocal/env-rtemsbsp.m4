dnl $Id$

dnl Pass a single BSP via an environment variable
dnl used by per BSP configure scripts
AC_DEFUN(RTEMS_ENV_RTEMSBSP,
[dnl
AC_BEFORE([$0], [RTEMS_ENABLE_RTEMSBSP])dnl
AC_BEFORE([$0], [RTEMS_PROJECT_ROOT])dnl
AC_BEFORE([$0], [RTEMS_CHECK_CUSTOM_BSP])dnl

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

RTEMS_BSP_SPECS="-specs bsp_specs -qrtems"
AC_SUBST(RTEMS_BSP_SPECS)

GCC_SPECS="-B\$(PROJECT_ROOT)/lib/ -B\$(PROJECT_ROOT)/$RTEMS_BSP/lib/"
AC_SUBST(GCC_SPECS)

PROJECT_INCLUDE="\$(PROJECT_ROOT)/$RTEMS_BSP/lib/include"
AC_SUBST(PROJECT_INCLUDE)

PROJECT_RELEASE="\$(PROJECT_ROOT)/$RTEMS_BSP"   
AC_SUBST(PROJECT_RELEASE)

RTEMS_ROOT=$PROJECT_ROOT/c/$RTEMS_BSP
AC_SUBST(RTEMS_ROOT)

RTEMS_ENABLE_BARE
AC_SUBST(BARE_CPU_MODEL)
AC_SUBST(BARE_CPU_CFLAGS)

AM_CONDITIONAL([MULTILIB],[false])

includedir="\${exec_prefix}/${RTEMS_BSP}/lib/include"
libdir="\${exec_prefix}/${RTEMS_BSP}/lib"

bsplibdir="\${exec_prefix}/${RTEMS_BSP}/lib"
AC_SUBST(bsplibdir)
])
