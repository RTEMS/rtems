dnl $Id$

## Check for a cross tool, similar to AC_CHECK_TOOL, but do not fall back to
## the un-prefixed version of PROG-TO-CHECK-FOR.
## HACK: AC_CHECK_TOOL_PREFIX is an internal macro from autoconf-2.13
dnl RTEMS_CHECK_TOOL(VARIABLE, PROG-TO-CHECK-FOR[, VALUE-IF-NOT-FOUND [, PATH]])
AC_DEFUN(RTEMS_CHECK_TOOL,
[AC_REQUIRE([AC_CHECK_TOOL_PREFIX])dnl
AC_CHECK_PROG($1, ${ac_tool_prefix}$2, ${ac_tool_prefix}$2, $3, $4)
])
