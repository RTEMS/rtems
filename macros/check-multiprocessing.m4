dnl
dnl $Id$
dnl

AC_DEFUN(RTEMS_CHECK_MULTIPROCESSING,
[dnl
AC_REQUIRE([RTEMS_ENABLE_MULTILIB])dnl
AC_REQUIRE([RTEMS_ENV_RTEMSBSP])dnl
AC_REQUIRE([RTEMS_TOP])dnl
AC_REQUIRE([RTEMS_CHECK_CPU])dnl
AC_REQUIRE([RTEMS_ENABLE_MULTIPROCESSING])dnl
AC_REQUIRE([RTEMS_BSP_ALIAS])dnl

AC_CACHE_CHECK([whether BSP supports multiprocessing],
  rtems_cv_HAS_MP,
  [dnl
    if test x"$multilib" = x"yes"; then
      # FIXME: Currently, multilibs and multiprocessing can not be 
      # build simultaneously
      rtems_cv_HAS_MP="disabled"
    else
      if test -d "$srcdir/${RTEMS_TOPdir}/c/src/lib/libbsp/${RTEMS_CPU}/${RTEMS_BSP_FAMILY}/shmsupp"; then
        if test "$RTEMS_HAS_MULTIPROCESSING" = "yes"; then
          rtems_cv_HAS_MP="yes" ;
        else
          rtems_cv_HAS_MP="disabled";
        fi
      else
        rtems_cv_HAS_MP="no";
      fi
    fi])
if test "$rtems_cv_HAS_MP" = "yes"; then
HAS_MP="yes"
else
HAS_MP="no"
fi
AC_SUBST(HAS_MP)
])

AC_DEFUN(RTEMS_DEFINE_MULTIPROCESSING,
[AC_REQUIRE([RTEMS_CHECK_MULTIPROCESSING])dnl
if test x"${HAS_MP}" = x"yes";
then
  AC_DEFINE_UNQUOTED(RTEMS_MULTIPROCESSING,1,[if multiprocessing is supported])
fi
])
