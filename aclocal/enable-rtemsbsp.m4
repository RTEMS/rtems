dnl $Id$

dnl Override the set of BSPs to be built.
dnl used by the toplevel configure script
dnl RTEMS_ENABLE_RTEMSBSP(rtems_bsp_list)
AC_DEFUN(RTEMS_ENABLE_RTEMSBSP,
[
AC_BEFORE([$0], [RTEMS_ENV_RTEMSBSP])dnl
AC_ARG_ENABLE(rtemsbsp,
[  --enable-rtemsbsp=bsp1 bsp2 ..      BSPs to include in build],
[case "${enableval}" in
  yes|no) AC_MSG_ERROR([missing argument to --enable-rtemsbsp=\"bsp1 bsp2\"]);;
  *) $1=$enableval;;
esac],[$1=""])
])

dnl Pass a single BSP via an environment variable
dnl used by per BSP configure scripts
AC_DEFUN(RTEMS_ENV_RTEMSBSP,
[dnl
AC_BEFORE([$0], [RTEMS_ENABLE_RTEMSBSP])dnl
AC_BEFORE([$0], [RTEMS_PROJECT_ROOT])dnl
AC_BEFORE([$0], [RTEMS_CHECK_CUSTOM_BSP])dnl

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

RTEMS_ENABLE_BARE
AC_SUBST(BARE_CPU_MODEL)
AC_SUBST(BARE_CPU_CFLAGS)

RTEMS_ROOT=$PROJECT_ROOT/c/$RTEMS_BSP
AC_SUBST(RTEMS_ROOT)
AM_CONDITIONAL(RTEMS_CONFIG_PER_BSP, test x = x);
])

AC_DEFUN(RTEMS_ENV_RTEMSCPU,
[AC_REQUIRE([RTEMS_ENABLE_MULTILIB])

if test x"$multilib" = x"no"; then
  RTEMS_ENV_RTEMSBSP
  RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
else
  rtems_cv_RTEMS_BSP="multilib"
  RTEMS_BSP="$rtems_cv_RTEMS_BSP"
  AC_SUBST(RTEMS_BSP)
  RTEMS_ROOT=$PROJECT_ROOT/c
  AC_SUBST(RTEMS_ROOT)
fi
])
