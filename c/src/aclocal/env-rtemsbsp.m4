dnl Pass a single BSP via an environment variable
dnl used by per BSP configure scripts
AC_DEFUN([RTEMS_ENV_RTEMSBSP],
[dnl
AC_BEFORE([$0], [RTEMS_ENABLE_RTEMSBSP])dnl
AC_BEFORE([$0], [RTEMS_PROJECT_ROOT])dnl
AC_BEFORE([$0], [RTEMS_CHECK_CUSTOM_BSP])dnl

# AC_ARG_VAR([CPU_CFLAGS],[CFLAGS specifying CPU-dependent features])
# AC_ARG_VAR([CFLAGS_OPTIMIZE_V],[CFLAGS for building the OPTIMIZE variant])
# AC_ARG_VAR([CFLAGS_DEBUG_V],[CFLAGS for building the DEBUG variant])
AC_ARG_VAR([RTEMS_BSP_FAMILY],[RTEMS's BSP directory])
AC_ARG_VAR([RTEMS_CPU_MODEL],[RTEMS's cpu model])

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

includedir="\${exec_prefix}/${RTEMS_BSP}/lib/include"
libdir="\${exec_prefix}/${RTEMS_BSP}/lib"

AC_SUBST([PROJECT_INCLUDE],["\$(PROJECT_ROOT)/$RTEMS_BSP/lib/include"])
AC_SUBST([PROJECT_LIB],["\$(PROJECT_ROOT)/$RTEMS_BSP/lib"])
AC_SUBST([PROJECT_RELEASE],["\$(PROJECT_ROOT)/$RTEMS_BSP"])

AC_SUBST([project_libdir],["\$(libdir)"])
AC_SUBST([project_includedir],["\$(includedir)"])
])
