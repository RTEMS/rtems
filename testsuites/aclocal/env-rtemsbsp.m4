dnl Pass a single BSP via an environment variable
dnl used by per BSP configure scripts
AC_DEFUN([RTEMS_ENV_RTEMSBSP],
[dnl
AC_BEFORE([$0], [RTEMS_ENABLE_RTEMSBSP])dnl
AC_BEFORE([$0], [RTEMS_PROJECT_ROOT])dnl
AC_BEFORE([$0], [RTEMS_CHECK_CUSTOM_BSP])dnl

AC_ARG_VAR([RTEMS_BSP_FAMILY],[RTEMS's BSP directory])

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

AC_SUBST([PROJECT_INCLUDE],["\$(PROJECT_ROOT)/lib/include"])
AC_SUBST([PROJECT_LIB],["\$(PROJECT_ROOT)/lib"])
AC_SUBST([PROJECT_RELEASE],["\$(PROJECT_ROOT)"])

AC_SUBST([rtems_testsdir],["\$(libdir)/rtems-][_RTEMS_API][/tests"])
])
