dnl Install a BSP's linkcmds from the source-tree into the build-tree
AC_DEFUN([RTEMS_BSP_LINKCMDS],[
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])
LINKCMDS=
for f in "${srcdir}/../../../../../../bsps/${RTEMS_CPU}/${RTEMS_BSP_FAMILY}/start/linkcmds.${RTEMS_BSP}" \
  "${srcdir}/../../../../../../bsps/${RTEMS_CPU}/${RTEMS_BSP_FAMILY}/start/linkcmds.${RTEMS_BSP_FAMILY}" \
  "${srcdir}/../../../../../../bsps/${RTEMS_CPU}/${RTEMS_BSP_FAMILY}/start/linkcmds";
do
  AS_IF([test -f "$f"],[
    LINKCMDS="$f"
    break])
done

AS_IF([test -z "${LINKCMDS}"],[
  AC_MSG_ERROR([can not determine linkcmds])])

AC_CONFIG_LINKS([linkcmds:${LINKCMDS}])
])
