dnl $Id$

AC_DEFUN(RTEMS_ENV_RTEMSCPU,
[AC_REQUIRE([RTEMS_ENABLE_MULTILIB])

if test x"$multilib" = x"yes"; then
  AS_IF([test -n "$with_multisubdir"],
    [MULTIBUILDTOP=`echo "/$with_multisubdir" | sed 's,/[[^\\/]]*,../,g'`])
  AC_SUBST(MULTIBUILDTOP)

  AS_IF([test -n "$with_multisubdir"],
    [MULTISUBDIR="/$with_multisubdir"])
  AC_SUBST(MULTISUBDIR)

  PROJECT_INCLUDE="\$(PROJECT_ROOT)/lib/include"
  AC_SUBST(PROJECT_INCLUDE)

  project_libdir="\$(PROJECT_ROOT)/lib/\$(MULTISUBDIR)"
  AC_SUBST(project_libdir)

  includedir="\${exec_prefix}/lib/include"
  libdir="${libdir}\$(MULTISUBDIR)"
else
  RTEMS_ENV_RTEMSBSP
fi
])
