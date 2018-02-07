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
dnl check if the compiler supports -isystem
RTEMS_GCC_ISYSTEM
dnl check if the target compiler may use --pipe
RTEMS_GCC_PIPE
test "$rtems_cv_gcc_pipe" = "yes" && CC="$CC --pipe"

dnl check if the compiler supports --specs
RTEMS_GCC_SPECS

dnl Compilers that does not support --specs (Clang) still needs -B and -qrtems
GCCSPECS="-B\$(RTEMS_SOURCE_ROOT)/bsps/\$(RTEMS_CPU)/\$(RTEMS_BSP_FAMILY)/start"
AS_IF([test x"$rtems_cv_gcc_specs" = xyes],[
GCCSPECS="${GCCSPECS} -specs bsp_specs"])
AC_SUBST(GCCSPECS)
GCCSPECS="${GCCSPECS} -qrtems"

AS_IF([test "$GCC" = yes],[
  RTEMS_RELLDFLAGS="-qnolinkcmds -nostdlib -r"
])
AC_SUBST(RTEMS_RELLDFLAGS)

RTEMS_INCLUDES
RTEMS_BSP_INCLUDES
])
