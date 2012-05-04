dnl Install a BSP's linkcmds from the source-tree into the build-tree
AC_DEFUN([RTEMS_BSP_LINKCMDS],[
LINKCMDS=
for f in "${srcdir}/startup/linkcmds.${RTEMS_BSP}" \
  "${srcdir}/startup/linkcmds.${RTEMS_BSP_FAMILY}" \
  "${srcdir}/startup/linkcmds";
do
  AS_IF([test -f "$f"],[
    LINKCMDS="$f"
    break])
done

AS_IF([test -z "${LINKCMDS}"],[
  AC_MSG_ERROR([can not determine linkcmds])])

AC_CONFIG_LINKS([startup/linkcmds:${LINKCMDS}])
])
