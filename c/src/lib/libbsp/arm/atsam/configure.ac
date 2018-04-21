AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-arm-atsam],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP
RTEMS_BSP_LINKCMDS

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE



RTEMS_BSP_CLEANUP_OPTIONS
AC_ARG_ENABLE(
[chip],
[AS_HELP_STRING([--enable-chip],[select a chip variant (default samv71q21)])],
[case "${enableval}" in
  same70j19) AC_DEFINE([__SAME70J19__],[1],[chip variant]) INTFLASH=0x00080000 ; INTSRAM=0x00040000 ;;
  same70j20) AC_DEFINE([__SAME70J20__],[1],[chip variant]) INTFLASH=0x00100000 ; INTSRAM=0x00060000 ;;
  same70j21) AC_DEFINE([__SAME70J21__],[1],[chip variant]) INTFLASH=0x00200000 ; INTSRAM=0x00060000 ;;
  same70n19) AC_DEFINE([__SAME70N19__],[1],[chip variant]) INTFLASH=0x00080000 ; INTSRAM=0x00040000 ;;
  same70n20) AC_DEFINE([__SAME70N20__],[1],[chip variant]) INTFLASH=0x00100000 ; INTSRAM=0x00060000 ;;
  same70n21) AC_DEFINE([__SAME70N21__],[1],[chip variant]) INTFLASH=0x00200000 ; INTSRAM=0x00060000 ;;
  same70q19) AC_DEFINE([__SAME70Q19__],[1],[chip variant]) INTFLASH=0x00080000 ; INTSRAM=0x00040000 ;;
  same70q20) AC_DEFINE([__SAME70Q20__],[1],[chip variant]) INTFLASH=0x00100000 ; INTSRAM=0x00060000 ;;
  same70q21) AC_DEFINE([__SAME70Q21__],[1],[chip variant]) INTFLASH=0x00200000 ; INTSRAM=0x00060000 ;;
  sams70j19) AC_DEFINE([__SAMS70J19__],[1],[chip variant]) INTFLASH=0x00080000 ; INTSRAM=0x00040000 ;;
  sams70j20) AC_DEFINE([__SAMS70J20__],[1],[chip variant]) INTFLASH=0x00100000 ; INTSRAM=0x00060000 ;;
  sams70j21) AC_DEFINE([__SAMS70J21__],[1],[chip variant]) INTFLASH=0x00200000 ; INTSRAM=0x00060000 ;;
  sams70n19) AC_DEFINE([__SAMS70N19__],[1],[chip variant]) INTFLASH=0x00080000 ; INTSRAM=0x00040000 ;;
  sams70n20) AC_DEFINE([__SAMS70N20__],[1],[chip variant]) INTFLASH=0x00100000 ; INTSRAM=0x00060000 ;;
  sams70n21) AC_DEFINE([__SAMS70N21__],[1],[chip variant]) INTFLASH=0x00200000 ; INTSRAM=0x00060000 ;;
  sams70q19) AC_DEFINE([__SAMS70Q19__],[1],[chip variant]) INTFLASH=0x00080000 ; INTSRAM=0x00040000 ;;
  sams70q20) AC_DEFINE([__SAMS70Q20__],[1],[chip variant]) INTFLASH=0x00100000 ; INTSRAM=0x00060000 ;;
  sams70q21) AC_DEFINE([__SAMS70Q21__],[1],[chip variant]) INTFLASH=0x00200000 ; INTSRAM=0x00060000 ;;
  samv71j19) AC_DEFINE([__SAMV71J19__],[1],[chip variant]) INTFLASH=0x00080000 ; INTSRAM=0x00040000 ;;
  samv71j20) AC_DEFINE([__SAMV71J20__],[1],[chip variant]) INTFLASH=0x00100000 ; INTSRAM=0x00060000 ;;
  samv71j21) AC_DEFINE([__SAMV71J21__],[1],[chip variant]) INTFLASH=0x00200000 ; INTSRAM=0x00060000 ;;
  samv71n19) AC_DEFINE([__SAMV71N19__],[1],[chip variant]) INTFLASH=0x00080000 ; INTSRAM=0x00040000 ;;
  samv71n20) AC_DEFINE([__SAMV71N20__],[1],[chip variant]) INTFLASH=0x00100000 ; INTSRAM=0x00060000 ;;
  samv71n21) AC_DEFINE([__SAMV71N21__],[1],[chip variant]) INTFLASH=0x00200000 ; INTSRAM=0x00060000 ;;
  samv71q19) AC_DEFINE([__SAMV71Q19__],[1],[chip variant]) INTFLASH=0x00080000 ; INTSRAM=0x00040000 ;;
  samv71q20) AC_DEFINE([__SAMV71Q20__],[1],[chip variant]) INTFLASH=0x00100000 ; INTSRAM=0x00060000 ;;
  samv71q21) AC_DEFINE([__SAMV71Q21__],[1],[chip variant]) INTFLASH=0x00200000 ; INTSRAM=0x00060000 ;;
  *) AC_MSG_ERROR([bad value ${enableval} for chip variant]) ;;
esac],
[AC_DEFINE([__SAMV71Q21__],[1],[chip variant]) INTFLASH=0x00200000 ; INTSRAM=0x00060000])

AC_ARG_ENABLE(
[sdram],
[AS_HELP_STRING([--enable-sdram],[select a SDRAM variant (default is42s16100e-7bli)])],
[case "${enableval}" in
  is42s16100e-7bli) AC_DEFINE([ATSAM_SDRAM_IS42S16100E_7BLI],[1],[SDRAM variant]) EXTSDRAM=0x00200000 ;;
  is42s16320f-7bl) AC_DEFINE([ATSAM_SDRAM_IS42S16320F_7BL],[1],[SDRAM variant]) EXTSDRAM=0x04000000 ;;
  mt48lc16m16a2p-6a) AC_DEFINE([ATSAM_SDRAM_MT48LC16M16A2P_6A],[1],[SDRAM variant]) EXTSDRAM=0x02000000 ;;
  *) AC_MSG_ERROR([bad value ${enableval} for SDRAM variant]) ;;
esac],
[AC_DEFINE([ATSAM_SDRAM_IS42S16100E_7BLI],[1],[SDRAM variant]) EXTSDRAM=0x00200000])

RTEMS_BSPOPTS_SET([BOARD_MAINOSC],[*],[12000000])
RTEMS_BSPOPTS_HELP([BOARD_MAINOSC],[Main oscillator frequency in Hz (default 12MHz)])

RTEMS_BSPOPTS_SET([ATSAM_MCK],[*],[123000000])
RTEMS_BSPOPTS_HELP([ATSAM_MCK],
[Frequency of the MCK in Hz. Set to 0 to force application defined speed.
See start/pmc-config.c for available clock configurations.])

RTEMS_BSPOPTS_SET([ATSAM_SLOWCLOCK_USE_XTAL],[*],[1])
RTEMS_BSPOPTS_HELP([ATSAM_SLOWCLOCK_USE_XTAL],
[Use the external crystal as source for the slow clock instead of the internal
RC oscillator. Note that on the ATSAM the NRST pin seems to depend on the slow
clock as well as all watchdogs. If ATSAM_SLOWCLOCK_USE_XTAL is set to 1 without
a external crystal connected, the controller might hang in the switching process
without a working NRST pin. ])

RTEMS_BSPOPTS_SET([ATSAM_CHANGE_CLOCK_FROM_SRAM],[*],[0])
RTEMS_BSPOPTS_HELP([ATSAM_CHANGE_CLOCK_FROM_SRAM],
[Move the functions that set up the clock into the SRAM.
This allows to change the clock frequency even if the application is started from SDRAM.
Requires a TCM_SIZE > 0.])

RTEMS_BSPOPTS_SET([ATSAM_CONSOLE_BAUD],[*],[115200])
RTEMS_BSPOPTS_HELP([ATSAM_CONSOLE_BAUD],[initial baud for console devices (default 115200)])

RTEMS_BSPOPTS_SET([ATSAM_CONSOLE_DEVICE_TYPE],[*],[0])
RTEMS_BSPOPTS_HELP([ATSAM_CONSOLE_DEVICE_TYPE],[device type for /dev/console, use 0 for USART and 1 for UART (default USART)])

RTEMS_BSPOPTS_SET([ATSAM_CONSOLE_DEVICE_INDEX],[*],[1])
RTEMS_BSPOPTS_HELP([ATSAM_CONSOLE_DEVICE_INDEX],[device index for /dev/console (default 1, e.g. USART1)])

RTEMS_BSPOPTS_SET([ATSAM_CONSOLE_USE_INTERRUPTS],[*],[1])
RTEMS_BSPOPTS_HELP([ATSAM_CONSOLE_USE_INTERRUPTS],[use interrupt driven mode for console devices (used by default)])

AC_DEFUN([ATSAM_LINKCMD],[
AC_ARG_VAR([$1],[$2])dnl
[$1]=[$]{[$1]:-[$3]}
])

ATSAM_LINKCMD([ATSAM_MEMORY_TCM_SIZE],[size of tightly coupled memories (TCM) in bytes],[0x00000000])
ATSAM_LINKCMD([ATSAM_MEMORY_INTFLASH_SIZE],[size of internal flash in bytes],[${INTFLASH}])
ATSAM_LINKCMD([ATSAM_MEMORY_INTSRAM_SIZE],[size of internal SRAM in bytes],[${INTSRAM}])
ATSAM_LINKCMD([ATSAM_MEMORY_SDRAM_SIZE],[size of external SDRAM in bytes],[${EXTSDRAM}])
ATSAM_LINKCMD([ATSAM_MEMORY_QSPIFLASH_SIZE],[size of QSPI flash in bytes],[0x00200000])
ATSAM_LINKCMD([ATSAM_MEMORY_NOCACHE_SIZE],[size of NOCACHE section in bytes],[0x00001000])

AC_CONFIG_FILES([
Makefile
linkcmds.memory:../../../../../../bsps/arm/atsam/start/linkcmds.memory.in
])
AC_OUTPUT
