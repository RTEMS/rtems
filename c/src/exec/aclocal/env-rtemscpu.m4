dnl $Id$

AC_DEFUN(RTEMS_ENV_RTEMSCPU,
[AC_REQUIRE([RTEMS_ENABLE_MULTILIB])

if test x"$multilib" = x"yes"; then
## FIXME: There is no multilib BSP
  RTEMS_BSP_SPECS=""
  AC_SUBST(RTEMS_BSP_SPECS)

  if test -n "$with_multisubdir"; then
    MULTIBUILDTOP=`echo "/$with_multisubdir" | sed 's,/[[^\\/]]*,../,g'`
  fi
  AC_SUBST(MULTIBUILDTOP)

  if test -n "$with_multisubdir"; then
    MULTISUBDIR="/$with_multisubdir"
  fi
  AC_SUBST(MULTISUBDIR)

  PROJECT_ROOT="../../${RTEMS_TOPdir}/\$(MULTIBUILDTOP)\$(top_builddir)"
  GCC_SPECS="-isystem \$(PROJECT_ROOT)/lib/include"
  AC_SUBST(GCC_SPECS)

  PROJECT_INCLUDE="\$(PROJECT_ROOT)/lib/include"
  AC_SUBST(PROJECT_INCLUDE)

  PROJECT_RELEASE="\$(PROJECT_ROOT)"
  AC_SUBST(PROJECT_RELEASE)

  RTEMS_ROOT=${PROJECT_ROOT}
  AC_SUBST(RTEMS_ROOT)

  includedir="\${exec_prefix}/lib/include"
  libdir="${libdir}\$(MULTISUBDIR)"
  AM_CONDITIONAL([RTEMS_CONFIG_PER_BSP],[false])
else
  RTEMS_ENV_RTEMSBSP
  RTEMS_CHECK_CUSTOM_BSP(RTEMS_BSP)
fi
])
