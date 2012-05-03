dnl
dnl Set target tools
dnl

AC_DEFUN([RTEMS_CANONICALIZE_TOOLS],
[AC_REQUIRE([RTEMS_PROG_CC])dnl

dnl FIXME: What shall be done if these tools are not available?
  RTEMS_CHECK_TOOL(AR,ar,no)

dnl special treatment of ranlib
  RTEMS_CHECK_TOOL(RANLIB,ranlib,:)
])
