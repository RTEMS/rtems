# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  csb336 )
    AC_CONFIG_SUBDIRS([csb336]);;
  csb337 )
    AC_CONFIG_SUBDIRS([csb337]);;
  edb7312 )
    AC_CONFIG_SUBDIRS([edb7312]);;
  gba )
    AC_CONFIG_SUBDIRS([gba]);;
  gdbarmsim )
    AC_CONFIG_SUBDIRS([gdbarmsim]);;
  gp32 )
    AC_CONFIG_SUBDIRS([gp32]);;
  gumstix )
    AC_CONFIG_SUBDIRS([gumstix]);;
  lm3s69xx )
    AC_CONFIG_SUBDIRS([lm3s69xx]);;
  lpc24xx )
    AC_CONFIG_SUBDIRS([lpc24xx]);;
  lpc32xx )
    AC_CONFIG_SUBDIRS([lpc32xx]);;
  nds )
    AC_CONFIG_SUBDIRS([nds]);;
  rtl22xx )
    AC_CONFIG_SUBDIRS([rtl22xx]);;
  smdk2410 )
    AC_CONFIG_SUBDIRS([smdk2410]);;
  stm32f4 )
    AC_CONFIG_SUBDIRS([stm32f4]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
