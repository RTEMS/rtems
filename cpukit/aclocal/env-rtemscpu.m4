dnl $Id$

# PROJECT_ROOT .. Directory to put the temporary installed files into

# FIXME: to be revisited - featureladden, bloated 
AC_DEFUN([RTEMS_ENV_RTEMSCPU],
[
  AC_REQUIRE([RTEMS_ENABLE_MULTILIB])

  # RTEMS_TOPdir points to the top of RTEMS source tree
  # cpukit_topdir points to the top of the cpukit source tree
  cpukit_topdir="/${RTEMS_TOPdir}/"

  AS_IF([test -n "$enable_rtemsbsp"],
  [ # Per BSP
    AC_ARG_VAR([CPU_CFLAGS],[])
    AC_ARG_VAR([CFLAGS_OPTIMIZE_V],
      [CFLAGS for building the optimized variant])
    AC_ARG_VAR([CFLAGS_DEBUG_V],
      [CFLAGS for building the debugging variant])
 
    AC_MSG_CHECKING([for RTEMS_BSP])
    AC_CACHE_VAL(rtems_cv_RTEMS_BSP,[
      test -n "${RTEMS_BSP}" && rtems_cv_RTEMS_BSP="$enable_rtemsbsp"
    ])
    AS_IF([test -z "$rtems_cv_RTEMS_BSP"],[
      AC_MSG_ERROR([Missing RTEMS_BSP])
    ])
    RTEMS_BSP="$rtems_cv_RTEMS_BSP"
    AC_MSG_RESULT(${RTEMS_BSP})

    test x"${with_project_root}" = x && with_project_root=".";
    case x${with_project_root} in
    x[[\\/]]* | x?:[[\\/]]* ) # absolute dir
      PROJECT_ROOT="${with_project_root}"
      ;;
    x* ) # relative dir
      PROJECT_ROOT="\$(top_builddir)${cpukit_topdir}${with_project_root}"
      ;;
    esac

    PROJECT_ROOT="${PROJECT_ROOT}/$RTEMS_BSP"
    AC_SUBST([PROJECT_INCLUDE],["${PROJECT_ROOT}/lib/include"])
    AC_SUBST([PROJECT_LIB],["${PROJECT_ROOT}/lib"])

    includedir="\${exec_prefix}/${RTEMS_BSP}/lib/include"
    libdir="\${exec_prefix}/${RTEMS_BSP}/lib"
  ],[
    AS_IF([test x"$multilib" = x"yes"],
    [
      AS_IF([test -n "$with_multisubdir"],
        [MULTIBUILDTOP=`echo "/$with_multisubdir" | sed 's,/[[^\\/]]*,../,g'`])
      AC_SUBST(MULTIBUILDTOP)
 
      AS_IF([test -n "$with_multisubdir"],
        [MULTISUBDIR="/$with_multisubdir"])
      AC_SUBST(MULTISUBDIR)

      PROJECT_ROOT="\$(top_builddir)${cpukit_topdir}../\$(MULTIBUILDTOP)"

      AC_SUBST([PROJECT_INCLUDE],["${PROJECT_ROOT}lib/include"])
      AC_SUBST([PROJECT_LIB],["${PROJECT_ROOT}lib\$(MULTISUBDIR)"])

      includedir="\${exec_prefix}/include"
      libdir="${libdir}\$(MULTISUBDIR)"
    ],[
      PROJECT_ROOT="\$(top_builddir)${cpukit_topdir}"

      AC_SUBST([PROJECT_INCLUDE],["${PROJECT_ROOT}lib/include"])
      AC_SUBST([PROJECT_LIB],["${PROJECT_ROOT}lib\$(MULTISUBDIR)"])

      includedir="\${exec_prefix}/include"
      libdir="${libdir}\$(MULTISUBDIR)"
    ])
  ])

  AC_SUBST([project_libdir],["\$(libdir)"])
  AC_SUBST([project_includedir],["\$(includedir)"])

  AC_SUBST(PROJECT_ROOT)

  AM_CONDITIONAL([MULTILIB],[test x"$multilib" = x"yes"])
])
