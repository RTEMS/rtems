AC_DEFUN([RTEMS_PROJECT_ROOT],
[dnl
AC_REQUIRE([RTEMS_TOP])

PACKHEX="\$(PROJECT_TOPdir)/tools/build/packhex"
AC_SUBST(PACKHEX)

BIN2C="\$(PROJECT_TOPdir)/tools/build/rtems-bin2c"
AC_SUBST(BIN2C)
])

