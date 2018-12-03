dnl
dnl PROJECT_TOPdir     .. relative path to the top of the build-tree
dnl PROJECT_ROOT       .. relative path to the top of the temporary
dnl                       installation directory inside the build-tree
dnl RTEMS_TOPdir       .. relative path of a subpackage's configure.ac to the
dnl                       toplevel configure.ac of the source-tree
dnl RTEMS_ROOT         .. path to the top of a bsp's build directory
dnl                       [Applied by custom/*.cfg, deprecated otherwise]
dnl

AC_DEFUN([RTEMS_PROJECT_ROOT],
[dnl
AC_REQUIRE([RTEMS_TOP])
BIN2C=rtems-bin2c
AC_SUBST(BIN2C)
])

