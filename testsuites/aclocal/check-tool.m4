## Check for a cross tool, similar to AC_CHECK_TOOL, but do not fall back to
## the un-prefixed version of PROG-TO-CHECK-FOR.
dnl RTEMS_CHECK_TOOL(VARIABLE, PROG-TO-CHECK-FOR[, VALUE-IF-NOT-FOUND [, PATH]])
AC_DEFUN([RTEMS_CHECK_TOOL],
[
  AS_IF([test "x$build_alias" != "x$host_alias"],
    [rtems_tool_prefix=${ac_tool_prefix}])
  AC_CHECK_PROG($1, ${rtems_tool_prefix}$2, ${rtems_tool_prefix}$2, $3, $4)
])
