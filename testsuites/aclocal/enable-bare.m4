AC_DEFUN(RTEMS_ENABLE_BARE,
[
AC_ARG_ENABLE(bare-cpu-cflags,
[AC_HELP_STRING([--enable-bare-cpu-cflags],
[specify a particular cpu cflag (bare bsp specific)])],
[case "${enableval}" in
  no) BARE_CPU_CFLAGS="" ;;
  *)    BARE_CPU_CFLAGS="${enableval}" ;;
esac],
[BARE_CPU_CFLAGS=""])

AC_ARG_ENABLE(bare-cpu-model,
[AC_HELP_STRING([--enable-bare-cpu-model],
[specify a particular cpu model (bare bsp specific)])],
[case "${enableval}" in
  no)   BARE_CPU_MODEL="" ;;
  *)    BARE_CPU_MODEL="${enableval}" ;;
esac],
[BARE_CPU_MODEL=""])
])

