dnl
dnl RTEMS Include paths.
dnl
dnl This is messy because the linkcmds have no clear and defined structure.
dnl There are BSP named linkcmds files, plain linkcmds file, configure template
dnl .in files configure changes and shared versions of these. The approach to have
dnl a single file for a number of BSPs in a family is sound, the lack of rules
dnl means we have a range of variants and this results in following code.
dnl
dnl The preinstall hid a number of questionable things and preinstall has gone.
dnl
AC_DEFUN([RTEMS_BSP_LINKCMDS],
[
AC_REQUIRE([RTEMS_SOURCE_TOP])
AC_REQUIRE([RTEMS_BUILD_TOP])

AC_MSG_CHECKING([BSP linkcmds])

RTEMS_BSP_ARCH_PATH="${RTEMS_SOURCE_ROOT}/bsps/${RTEMS_CPU}"
RTEMS_BSP_ARCH_LINKCMDS_PATH="${RTEMS_BSP_ARCH_PATH}/shared/start"
RTEMS_BSP_LIBBSP_PATH="${RTEMS_BUILD_ROOT}/lib/libbsp/${RTEMS_CPU}/${RTEMS_BSP_FAMILY}"

AC_SUBST(RTEMS_BSP_ARCH_LINKCMDS_PATH)
AC_SUBST(RTEMS_BSP_LIBBSP_PATH)
])
