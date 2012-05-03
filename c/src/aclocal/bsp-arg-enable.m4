dnl
dnl RTEMS_BSP_ARG_ENABLE(FEATURE, HELP-STRING, ACTION-IF-TRUE [, ACTION-IF-FALSE])
dnl 
dnl Accept --enable-FEATURE=<RTEMS_BSP>:<bsp-value> and 
dnl --enable-FEATURE=<value> in BSP-configure scripts.
dnl
dnl Configure scripts will receive <bsp-value> if <RTEMS_BSP> matches the 
dnl actual value of the shell variable RTEMS_BSP (cf. RTEMS_ENV_RTEMSBSP), 
dnl and the raw value passed to --enable-FEATURE otherwise.
dnl
AC_DEFUN([RTEMS_BSP_ARG_ENABLE],
[AC_REQUIRE([RTEMS_ENV_RTEMSBSP])
AC_ARG_ENABLE([$1],[$2],
[enableval=`echo "[$enable_]patsubst([$1], -, _)" | sed -e "s%^${RTEMS_BSP}:%%"`
  eval "[enable_]patsubst([$1], -, _)='$enableval'"
$3],[$4])
])
