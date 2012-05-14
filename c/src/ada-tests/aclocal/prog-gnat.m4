##
## Partially borrowed from gcc-3.2
##
## WARNING: All the stuff below is pretty immature.

AC_DEFUN([RTEMS_PROG_GNAT],
[AC_REQUIRE([AC_CHECK_TOOL_PREFIX])
AC_REQUIRE([RTEMS_PROG_CC_FOR_TARGET])

AC_CHECK_TOOL([GNATMAKE],[gnatmake],[])
AC_CACHE_CHECK([for compiler driver that understands Ada],
  [rtems_cv_prog_CCADA],
[cat >conftest.adb <<EOF
procedure conftest is begin null; end conftest;
EOF
rtems_cv_prog_CCADA=
# Have to do ac_tool_prefix and user overrides by hand.
user_ccada=$CCADA
user_cc=$CC
for cand in ${ac_tool_prefix}$user_ccada	$user_ccada	\
	    ${ac_tool_prefix}$user_cc		$user_cc	\
	    ${ac_tool_prefix}gcc		gcc		\
	    ${ac_tool_prefix}cc			cc		\
	    ${ac_tool_prefix}gnatgcc		gnatgcc		\
	    ${ac_tool_prefix}gnatcc		gnatcc		\
	    ${ac_tool_prefix}adagcc		adagcc		\
	    ${ac_tool_prefix}adac		adac		; do
  # There is a bug in all released versions of GCC which causes the
  # driver to exit successfully when the appropriate language module
  # has not been installed.  This is fixed in 2.95.4, 3.0.2, and 3.1.
  # Therefore we must check for the error message as well as an
  # unsuccessful exit.
  errors=`($cand -c conftest.adb) 2>&1 || echo failure`
  AS_IF([test -z "$errors"],
    [rtems_cv_prog_CCADA=$cand
     break])
done
rm -f conftest.*])
AC_SUBST([CCADA],[$rtems_cv_prog_CCADA])

AS_IF([test -n "$GNATMAKE" && test -n "$CCADA"],
  [HAVE_GNAT=yes],
  [HAVE_GNAT=no])
])
