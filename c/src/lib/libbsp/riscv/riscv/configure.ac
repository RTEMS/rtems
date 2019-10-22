##
#
# @file
#
# @brief Configure script of LibBSP for riscv BSP.
#

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-riscv],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_BSPOPTS_SET([BSP_START_COPY_FDT_FROM_U_BOOT],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_START_COPY_FDT_FROM_U_BOOT],[copy the U-Boot provided FDT to an internal storage])

RTEMS_BSPOPTS_SET([BSP_FDT_BLOB_SIZE_MAX],[*],[65536])
RTEMS_BSPOPTS_HELP([BSP_FDT_BLOB_SIZE_MAX],[maximum size of the FDT blob in bytes])

RTEMS_BSPOPTS_SET([BSP_FDT_BLOB_READ_ONLY],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_FDT_BLOB_READ_ONLY],[place the FDT blob into the read-only data area])

RTEMS_BSPOPTS_SET([BSP_FDT_BLOB_COPY_TO_READ_ONLY_LOAD_AREA],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_FDT_BLOB_COPY_TO_READ_ONLY_LOAD_AREA],[copy the FDT blob into the read-only load area via bsp_fdt_copy()])

RTEMS_BSPOPTS_SET([BSP_CONSOLE_BAUD],[*],[115200])
RTEMS_BSPOPTS_HELP([BSP_CONSOLE_BAUD],[default baud for console driver devices (default 115200)])

RTEMS_BSPOPTS_SET([RISCV_MAXIMUM_EXTERNAL_INTERRUPTS],[*],[64])
RTEMS_BSPOPTS_HELP([RISCV_MAXIMUM_EXTERNAL_INTERRUPTS],[maximum number of external interrupts supported by the BSP (default 64)])

RTEMS_BSPOPTS_SET([RISCV_ENABLE_HTIF_SUPPORT],[*],[])
RTEMS_BSPOPTS_HELP([RISCV_ENABLE_HTIF_SUPPORT],[enables the HTIF support if defined to a non-zero value, otherwise it is disabled (disabled by default)])

RTEMS_BSPOPTS_SET([RISCV_CONSOLE_MAX_NS16550_DEVICES],[frdme310arty*],[])
RTEMS_BSPOPTS_SET([RISCV_CONSOLE_MAX_NS16550_DEVICES],[*],[2])
RTEMS_BSPOPTS_HELP([RISCV_CONSOLE_MAX_NS16550_DEVICES],[maximum number of NS16550 devices supported by the console driver (2 by default)])

RTEMS_BSPOPTS_SET([RISCV_ENABLE_FRDME310ARTY_SUPPORT],[frdme310arty*],[1])
RTEMS_BSPOPTS_SET([RISCV_ENABLE_FRDME310ARTY_SUPPORT],[*],[])
RTEMS_BSPOPTS_HELP([RISCV_ENABLE_FRDME310ARTY_SUPPORT],[enables support sifive Freedom E310 Arty board if defined to a non-zero value,otherwise it is disabled (disabled by default)])

RTEMS_BSP_CLEANUP_OPTIONS

case "${RTEMS_BSP}" in
  rv64*medany)
    RISCV_RAM_REGION_BEGIN_DEFAULT=0x80000000
    ;;
  rv64*)
    RISCV_RAM_REGION_BEGIN_DEFAULT=0x70000000
    ;;
  *)
    RISCV_RAM_REGION_BEGIN_DEFAULT=0x80000000
    ;;
esac

case "${RTEMS_BSP}" in
  frdm*310arty)
    RISCV_RAM_REGION_SIZE_DEFAULT=0x10000000
    ;;
  *)
    RISCV_RAM_REGION_SIZE_DEFAULT=0x04000000
    ;;
esac

AC_DEFUN([RISCV_LINKCMD],[
AC_ARG_VAR([$1],[$2])dnl
[$1]=[$]{[$1]:-[$3]}
])

RISCV_LINKCMD([RISCV_RAM_REGION_BEGIN],[begin of the RAM region for linker command file (default is 0x70000000 for 64-bit with -mcmodel=medlow and 0x80000000 for all other)],[${RISCV_RAM_REGION_BEGIN_DEFAULT}])
RISCV_LINKCMD([RISCV_RAM_REGION_SIZE],[size of the RAM region for linker command file (default is 256 MiB for frdme310arty and 64 MiB for all other)],[${RISCV_RAM_REGION_SIZE_DEFAULT}])

AC_CONFIG_FILES([
Makefile
linkcmds:../../../../../../bsps/riscv/riscv/start/linkcmds.in
])
AC_OUTPUT
