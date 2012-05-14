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
# Was CFLAGS set?
rtems_cv_CFLAGS_set="${CFLAGS+set}"
dnl check target cc
RTEMS_PROG_CC
dnl check if the target compiler may use --pipe
RTEMS_GCC_PIPE
test "$rtems_cv_gcc_pipe" = "yes" && CC="$CC --pipe"

# Append warning flags if CFLAGS wasn't set.
AS_IF([test "$GCC" = yes && test "$rtems_cv_CFLAGS_set" != set],
[CFLAGS="$CFLAGS -Wall -Wimplicit-function-declaration -Wstrict-prototypes -Wnested-externs"])

RTEMS_CPPFLAGS="-I\$(top_builddir) -I\$(PROJECT_INCLUDE)"
AC_SUBST(RTEMS_CPPFLAGS)

AS_IF([test "$GCC" = yes],[
  RTEMS_RELLDFLAGS="-qnolinkcmds -nostdlib -r"
])
AC_SUBST(RTEMS_RELLDFLAGS)
])
