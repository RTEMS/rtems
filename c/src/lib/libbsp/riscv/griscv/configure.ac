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

RTEMS_BSPOPTS_SET([BSP_CONSOLE_BAUD],[*],[115200])
RTEMS_BSPOPTS_HELP([BSP_CONSOLE_BAUD],[default baud for console driver devices (default 115200)])

RTEMS_BSPOPTS_SET([RISCV_MAXIMUM_EXTERNAL_INTERRUPTS],[*],[64])
RTEMS_BSPOPTS_HELP([RISCV_MAXIMUM_EXTERNAL_INTERRUPTS],[maximum number of external interrupts supported by the BSP (default 64)])

RTEMS_BSP_CLEANUP_OPTIONS

RISCV_RAM_REGION_BEGIN_DEFAULT=0x40000000

AC_DEFUN([RISCV_LINKCMD],[
AC_ARG_VAR([$1],[$2])dnl
[$1]=[$]{[$1]:-[$3]}
])

case "${RTEMS_BSP}" in
  *clang)
    RISCV_LINKER_START_DIRECTIVE_DEFAULT="INPUT"
    RISCV_LINKER_ALIGN_DIRECTIVE_DEFAULT="ALIGN(8)"
    ;;
  *)
    RISCV_LINKER_START_DIRECTIVE_DEFAULT="STARTUP"
    RISCV_LINKER_ALIGN_DIRECTIVE_DEFAULT="ALIGN_WITH_INPUT"
    ;;
esac

AC_DEFUN([RISCV_LINKCMD],[
AC_ARG_VAR([$1],[$2])dnl
[$1]=[$]{[$1]:-[$3]}
])

RISCV_LINKCMD([RISCV_RAM_REGION_BEGIN],[begin of the RAM region for linker command file (default is 0x40000000)],[${RISCV_RAM_REGION_BEGIN_DEFAULT}])
RISCV_LINKCMD([RISCV_RAM_REGION_SIZE],[size of the RAM region for linker command file (default 16MiB)],[0x01000000])
RISCV_LINKCMD([RISCV_LINKER_START_DIRECTIVE],[GNU LD understands STARTUP directive while LLVM/LLD does not and needs INPUT instead],[${RISCV_LINKER_START_DIRECTIVE_DEFAULT}])
RISCV_LINKCMD([RISCV_LINKER_ALIGN_DIRECTIVE],[LLD does not support ALIGN_WITH_INPUT directive],[${RISCV_LINKER_ALIGN_DIRECTIVE_DEFAULT}])

AC_CONFIG_FILES([
Makefile
linkcmds:../../../../../../bsps/riscv/griscv/start/linkcmds.in
linkcmds.base:../../../../../../bsps/riscv/shared/start/linkcmds.base.in
])
AC_OUTPUT
