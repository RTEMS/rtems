dnl $Id$

AC_DEFUN([RTEMS_ENV_RTEMSCPU],
[
  AC_REQUIRE([RTEMS_ENABLE_MULTILIB])

  AS_IF([test x"$multilib" = x"yes"],
  [
    AS_IF([test -n "$with_multisubdir"],
      [MULTIBUILDTOP=`echo "/$with_multisubdir" | sed 's,/[[^\\/]]*,../,g'`])
    AC_SUBST(MULTIBUILDTOP)
 
    AS_IF([test -n "$with_multisubdir"],
      [MULTISUBDIR="/$with_multisubdir"])
    AC_SUBST(MULTISUBDIR)
 
    AC_SUBST([PROJECT_INCLUDE],["\$(PROJECT_ROOT)/lib/include"])
    AC_SUBST([PROJECT_LIB],["\$(PROJECT_ROOT)/lib/\$(MULTISUBDIR)"])
    AC_SUBST([PROJECT_RELEASE],["\$(PROJECT_ROOT)"])

    includedir="\${exec_prefix}/include"
    libdir="${libdir}\$(MULTISUBDIR)"
  ],
  [
    AC_ARG_VAR([CPU_CFLAGS],[])
    AC_ARG_VAR([CFLAGS_OPTIMIZE_V],
      [CFLAGS for building the optimized variant])
    AC_ARG_VAR([CFLAGS_DEBUG_V],
      [CFLAGS for building the debugging variant])
 
    AC_ARG_VAR([RTEMS_BSP],[RTEMS_BSP to build])
    AC_MSG_CHECKING([for RTEMS_BSP])
    AC_CACHE_VAL(rtems_cv_RTEMS_BSP,[
      test -n "${RTEMS_BSP}" && rtems_cv_RTEMS_BSP="$RTEMS_BSP"
    ])
    AS_IF([test -z "$rtems_cv_RTEMS_BSP"],[
      AC_MSG_ERROR([Missing RTEMS_BSP])
    ])
    RTEMS_BSP="$rtems_cv_RTEMS_BSP"
    AC_MSG_RESULT(${RTEMS_BSP})
    AC_SUBST(RTEMS_BSP)
 
    includedir="\${exec_prefix}/${RTEMS_BSP}/lib/include"
    libdir="\${exec_prefix}/${RTEMS_BSP}/lib"

    AC_SUBST([PROJECT_INCLUDE],["\$(PROJECT_ROOT)/$RTEMS_BSP/lib/include"])
    AC_SUBST([PROJECT_LIB],["\$(PROJECT_ROOT)/$RTEMS_BSP/lib"])
    AC_SUBST([PROJECT_RELEASE],["\$(PROJECT_ROOT)/$RTEMS_BSP"])

    RTEMS_ENABLE_BARE
    AC_SUBST(BARE_CPU_MODEL)
    AC_SUBST(BARE_CPU_CFLAGS)
  ])

  AC_SUBST([project_libdir],["\$(libdir)"])
  AC_SUBST([project_includedir],["\$(includedir)"])

  AM_CONDITIONAL([MULTILIB],[test x"$multilib" = x"yes"])
])
