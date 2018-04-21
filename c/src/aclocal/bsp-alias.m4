dnl _RTEMS_BSP_ALIAS(BSP_ALIAS,RTEMS_BSP_FAMILY)
dnl Internal subroutine to RTEMS_BSP_ALIAS
AC_DEFUN([_RTEMS_BSP_ALIAS],[
AC_REQUIRE([RTEMS_CANONICAL_TARGET_CPU])
AC_REQUIRE([RTEMS_SOURCE_TOP])
# account for "aliased" bsps which share source code
      for bsp_cfgs in `ls "${RTEMS_SOURCE_ROOT}/bsps/${RTEMS_CPU}"/*/"config/$1.cfg" 2>/dev/null`; do
        $2=`echo "$bsp_cfgs" | sed \
          -e "s,^${RTEMS_SOURCE_ROOT}/bsps/$RTEMS_CPU/,," \
          -e "s,/config/.*\.cfg$,,"`
        break
      done
])

dnl RTEMS_BSP_ALIAS(BSP_ALIAS,RTEMS_BSP_FAMILY)
dnl convert a bsp alias $1 into its bsp directory RTEMS_BSP_FAMILY
AC_DEFUN([RTEMS_BSP_ALIAS],
[_RTEMS_BSP_ALIAS(m4_if([$1],,[$RTEMS_BSP],[$1]),
  m4_if([$2],,[RTEMS_BSP_FAMILY],[$2]))]
)
