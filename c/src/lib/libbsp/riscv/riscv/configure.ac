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

AC_DEFUN([RISCV_LINKCMD],[
AC_ARG_VAR([$1],[$2])dnl
[$1]=[$]{[$1]:-[$3]}
])

RISCV_LINKCMD([RISCV_RAM_REGION_BEGIN],[begin of the RAM region for linker command file (default is 0x70000000 for 64-bit with -mcmodel=medlow and 0x80000000 for all other)],[${RISCV_RAM_REGION_BEGIN_DEFAULT}])
RISCV_LINKCMD([RISCV_RAM_REGION_SIZE],[size of the RAM region for linker command file (default 64MiB)],[0x04000000])

AC_CONFIG_FILES([
Makefile
linkcmds:../../../../../../bsps/riscv/riscv/start/linkcmds.in
])
AC_OUTPUT
