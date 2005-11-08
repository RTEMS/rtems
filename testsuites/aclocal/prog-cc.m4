dnl
dnl $Id$
dnl 
dnl Check for target gcc
dnl

AC_DEFUN([RTEMS_PROG_CC],
[
AC_BEFORE([$0], [AC_PROG_CPP])dnl
AC_BEFORE([$0], [AC_PROG_CC])dnl
AC_BEFORE([$0], [RTEMS_CANONICALIZE_TOOLS])dnl

RTEMS_CHECK_TOOL(CC,gcc)
test -z "$CC" && \
  AC_MSG_ERROR([no acceptable cc found in \$PATH])
AC_PROG_CC
AC_PROG_CPP
])

AC_DEFUN([RTEMS_PROG_CC_FOR_TARGET],
[
dnl check target cc
RTEMS_PROG_CC

AS_IF([test x"$GCC" = xyes],[
GCCSPECS="-B\$(PROJECT_ROOT)/lib/ -B\$(PROJECT_ROOT)/$RTEMS_BSP/lib/"
GCCSPECS="${GCCSPECS} -specs bsp_specs -qrtems"])
AC_SUBST(GCCSPECS)

if test "$GCC" = yes; then
RTEMS_CFLAGS="$RTEMS_CFLAGS -Wall"
m4_if([$1],,[],[RTEMS_CFLAGS="$RTEMS_CFLAGS $1"])
fi
AC_SUBST(RTEMS_CFLAGS)

AC_SUBST(RTEMS_CPPFLAGS)
])
