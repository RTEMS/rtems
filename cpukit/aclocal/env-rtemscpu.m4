dnl $Id$

AC_DEFUN(RTEMS_ENV_RTEMSCPU,
[AC_REQUIRE([RTEMS_ENABLE_MULTILIB])

if test x"$multilib" = x"yes"; then
## FIXME: There is no multilib BSP
  AS_IF([test -n "$with_multisubdir"],
    [MULTIBUILDTOP=`echo "/$with_multisubdir" | sed 's,/[[^\\/]]*,../,g'`])
  AC_SUBST(MULTIBUILDTOP)

  AS_IF([test -n "$with_multisubdir"],
    [MULTISUBDIR="/$with_multisubdir"])
  AC_SUBST(MULTISUBDIR)

  GCC_SPECS="-isystem \$(PROJECT_INCLUDE)"
  AC_SUBST(GCC_SPECS)

  PROJECT_INCLUDE="\$(PROJECT_ROOT)/lib/include"
  AC_SUBST(PROJECT_INCLUDE)

  project_libdir="\$(PROJECT_ROOT)/lib"
  AC_SUBST(project_libdir)

  RTEMS_ROOT="${PROJECT_ROOT}"
  AC_SUBST(RTEMS_ROOT)

  includedir="\${exec_prefix}/lib/include"
  libdir="${libdir}\$(MULTISUBDIR)"
else
  RTEMS_ENV_RTEMSBSP
  RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
fi
])
