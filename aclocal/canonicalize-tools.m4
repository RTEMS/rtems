dnl
dnl $Id$
dnl
dnl Set target tools
dnl

AC_DEFUN(RTEMS_HOST_AR,
[AC_CHECK_TOOL(AR,ar,no)])

AC_DEFUN(RTEMS_HOST_RANLIB,
[AC_CHECK_TOOL(RANLIB,ranlib,:)])

AC_DEFUN(RTEMS_GCC_PRINT,
[
    $1=`$CC --print-prog-name=$2`
])

AC_DEFUN(RTEMS_CANONICALIZE_TOOLS,
[AC_REQUIRE([RTEMS_PROG_CC])dnl

dnl FIXME: What shall be done if these tools are not available?
  RTEMS_HOST_AR(AR,ar,no)
  AC_CHECK_TOOL(AS,as,no)
  AC_CHECK_TOOL(LD,ld,no)
  AC_CHECK_TOOL(NM,nm,no)

dnl special treatment of ranlib
  RTEMS_HOST_RANLIB

dnl NOTE: These may not be available if not using gnutools
  AC_CHECK_TOOL(OBJCOPY,objcopy,no)
  AC_CHECK_TOOL(SIZE,size,no)
  AC_CHECK_TOOL(STRIP,strip,:)
])
