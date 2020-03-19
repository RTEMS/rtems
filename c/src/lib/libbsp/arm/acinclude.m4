# RTEMS_CHECK_BSPDIR(RTEMS_BSP_FAMILY)
AC_DEFUN([RTEMS_CHECK_BSPDIR],
[
  case "$1" in
  altera-cyclone-v )
    AC_CONFIG_SUBDIRS([altera-cyclone-v]);;
  atsam )
    AC_CONFIG_SUBDIRS([atsam]);;
  beagle )
    AC_CONFIG_SUBDIRS([beagle]);;
  csb336 )
    AC_CONFIG_SUBDIRS([csb336]);;
  csb337 )
    AC_CONFIG_SUBDIRS([csb337]);;
  edb7312 )
    AC_CONFIG_SUBDIRS([edb7312]);;
  gumstix )
    AC_CONFIG_SUBDIRS([gumstix]);;
  imx )
    AC_CONFIG_SUBDIRS([imx]);;
  lm3s69xx )
    AC_CONFIG_SUBDIRS([lm3s69xx]);;
  lpc176x )
    AC_CONFIG_SUBDIRS([lpc176x]);;
  lpc24xx )
    AC_CONFIG_SUBDIRS([lpc24xx]);;
  lpc32xx )
    AC_CONFIG_SUBDIRS([lpc32xx]);;
  raspberrypi )
    AC_CONFIG_SUBDIRS([raspberrypi]);;
  realview-pbx-a9 )
    AC_CONFIG_SUBDIRS([realview-pbx-a9]);;
  rtl22xx )
    AC_CONFIG_SUBDIRS([rtl22xx]);;
  smdk2410 )
    AC_CONFIG_SUBDIRS([smdk2410]);;
  stm32f4 )
    AC_CONFIG_SUBDIRS([stm32f4]);;
  tms570 )
    AC_CONFIG_SUBDIRS([tms570]);;
  xen )
    AC_CONFIG_SUBDIRS([xen]);;
  xilinx-zynq )
    AC_CONFIG_SUBDIRS([xilinx-zynq]);;
  xilinx-zynqmp )
    AC_CONFIG_SUBDIRS([xilinx-zynqmp]);;
  *)
    AC_MSG_ERROR([Invalid BSP]);;
  esac
])
