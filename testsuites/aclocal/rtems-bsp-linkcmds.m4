dnl
dnl RTEMS Include paths.
dnl
dnl This is messy because the linkcmds have no clearly defined structure.
dnl There are BSPs named linkcmds file, plain linkcmds file, configure template,
dnl .in file and configure changes, and we have shared versions of these. The approach of
dnl using a single file for a number of BSPs in a family is sound. The lack of rules
dnl means we have a range of variants, and this results in the following code.
dnl
dnl The preinstall hid a number of questionable things and preinstall is past.
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
