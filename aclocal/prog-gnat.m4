## $Id$
##
## WARNING: All the stuff below is pretty immature.

AC_DEFUN([RTEMS_PROG_GNAT],
[AC_REQUIRE([AC_CHECK_TOOL_PREFIX])
AC_REQUIRE([RTEMS_PROG_CC_FOR_TARGET])

AC_ARG_VAR([GNATMAKE],[GNATMAKE compiler command])
AC_ARG_VAR([ADAFLAGS],[ADA compiler flags])

RTEMS_CHECK_TOOL([GNATMAKE],[gnatmake],[])
AS_IF([test -z "$GNATMAKE"],[AC_MSG_ERROR([No GNATMAKE found])])

AC_MSG_CHECKING([whether $GNATMAKE works])
rm -f conftest.*
cat >conftest.adb <<EOF
procedure conftest is begin null; end conftest;
EOF

AS_IF([$GNATMAKE -c conftest.adb 1>>config.log 2>&1],
    [HAVE_GNAT=yes],
    [HAVE_GNAT=no])
AC_MSG_RESULT([$HAVE_GNAT])
rm -f conftest.*
])
