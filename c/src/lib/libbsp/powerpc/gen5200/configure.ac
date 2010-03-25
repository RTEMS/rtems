## Process this file with autoconf to produce a configure script.
##
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-powerpc-gen5200],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.10])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_BSPOPTS_SET([DATA_CACHE_ENABLE],[*],[1])
RTEMS_BSPOPTS_HELP([DATA_CACHE_ENABLE],
[If defined, the data cache will be enabled after address translation
 is turned on.])

RTEMS_BSPOPTS_SET([INSTRUCTION_CACHE_ENABLE],[*],[1])
RTEMS_BSPOPTS_HELP([INSTRUCTION_CACHE_ENABLE],
[If defined, the instruction cache will be enabled after address translation
 is turned on.])

RTEMS_BSPOPTS_SET([HAS_UBOOT],[icecube],[1])
RTEMS_BSPOPTS_SET([HAS_UBOOT],[pm520_*],[1])
RTEMS_BSPOPTS_HELP([HAS_UBOOT],
[If defined, board boots via U-Boot])

RTEMS_BSPOPTS_SET([BENCHMARK_IRQ_PROCESSING],[*],[0])
RTEMS_BSPOPTS_HELP([BENCHMARK_IRQ_PROCESSING],
[If set to !0, enable code to benchmark IRQ processing.])

RTEMS_BSPOPTS_SET([ALLOW_IRQ_NESTING],[icecube],[0])
RTEMS_BSPOPTS_SET([ALLOW_IRQ_NESTING],[*],[1])
RTEMS_BSPOPTS_HELP([ALLOW_IRQ_NESTING],
[If set to !0, allow nested IRQ processing.])

RTEMS_BSPOPTS_SET([BSP_PRESS_KEY_FOR_RESET],[icecube],[1])
RTEMS_BSPOPTS_SET([BSP_RESET_BOARD_AT_EXIT],[pm520_cr825],[1])
RTEMS_BSPOPTS_SET([BSP_PRESS_KEY_FOR_RESET],[*],[0])
RTEMS_BSPOPTS_HELP([BSP_PRESS_KEY_FOR_RESET],
[If set to !0, print a message and wait until pressed before resetting
 board when application exits.])

RTEMS_BSPOPTS_SET([BSP_RESET_BOARD_AT_EXIT],[icecube],[1])
RTEMS_BSPOPTS_SET([BSP_RESET_BOARD_AT_EXIT],[pm520_*],[1])
RTEMS_BSPOPTS_SET([BSP_RESET_BOARD_AT_EXIT],[brs5l],[1])
RTEMS_BSPOPTS_SET([BSP_RESET_BOARD_AT_EXIT],[*],[0])
RTEMS_BSPOPTS_HELP([BSP_RESET_BOARD_AT_EXIT],
[If set to !0, reset the board when the application exits.])

RTEMS_BSPOPTS_SET([BSP_GPIOPCR_INITMASK],[pm520_ze30],[0x337F3F77])
RTEMS_BSPOPTS_SET([BSP_GPIOPCR_INITVAL],[pm520_ze30],[0x01552114])

RTEMS_BSPOPTS_SET([BSP_GPIOPCR_INITMASK],[pm520_cr825],[0x330F0F77])
RTEMS_BSPOPTS_SET([BSP_GPIOPCR_INITVAL],[pm520_cr825],[0x01050444])

RTEMS_BSPOPTS_SET([BSP_GPIOPCR_INITMASK],[brs5l],[0xb30F0F77])
RTEMS_BSPOPTS_SET([BSP_GPIOPCR_INITVAL],[brs5l],[0x91050444])

RTEMS_BSPOPTS_SET([BSP_GPIOPCR_INITMASK],[icecube],[0x330F0F77])
RTEMS_BSPOPTS_SET([BSP_GPIOPCR_INITVAL],[icecube],[0x01050444])

RTEMS_BSPOPTS_SET([BSP_GPIOPCR_INITMASK],[*],[0x330F0F77])
RTEMS_BSPOPTS_SET([BSP_GPIOPCR_INITVAL],[*],[0x01050444])

RTEMS_BSPOPTS_HELP([BSP_GPIOPCR_INITMASK],
[defines the bits modified in the MPC5200 GPIOPCR register during init. 
 Must match the hardware requirements])
RTEMS_BSPOPTS_HELP([BSP_GPIOPCR_INITVAL],
[defines the bit values written in the MPC5200 GPIOPCR register during init. 
 Must match the hardware requirements])

## on ze30, we have PSC1/4/5/6 ...
RTEMS_BSPOPTS_SET([BSP_UART_AVAIL_MASK],[pm520_ze30],[0x39])
## on cr825, we have PSC1/2/3
RTEMS_BSPOPTS_SET([BSP_UART_AVAIL_MASK],[pm520_cr825],[0x07])
## on brs5l, we have PSC1/2/3
RTEMS_BSPOPTS_SET([BSP_UART_AVAIL_MASK],[brs5l],[0x07])
## on icecube, we only have PSC1
RTEMS_BSPOPTS_SET([BSP_UART_AVAIL_MASK],[icecube],[0x01])
RTEMS_BSPOPTS_SET([BSP_UART_AVAIL_MASK],[*],[0x01])
RTEMS_BSPOPTS_HELP([BSP_UART_AVAIL_MASK],
[bit mask to specify the UARTS (PSCs), which should be enabled on this board. 
 Must match the hardware requirements. PSC1 corresponds to the LSB])

RTEMS_BSPOPTS_SET([PM520_ZE30],[pm520_ze30],[1])
RTEMS_BSPOPTS_HELP([PM520_ZE30],
[If defined, use custom settings of for the pm520_ze30 BSP])

RTEMS_BSPOPTS_SET([PM520_CR825],[pm520_cr825],[1])
RTEMS_BSPOPTS_HELP([PM520_CR825],
[If defined, use custom settings of for the pm520_cr825 BSP])

RTEMS_BSPOPTS_SET([icecube],[icecube],[1])
RTEMS_BSPOPTS_HELP([icecube],
[If defined, use custom settings of for the icecube BSP])

RTEMS_BSPOPTS_SET([BRS5L],[brs5l],[1])
RTEMS_BSPOPTS_HELP([BRS5L],
[If defined, use custom settings of for the brs5l BSP])


RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])

RTEMS_BSP_CLEANUP_OPTIONS(0, 0)
RTEMS_BSP_LINKCMDS

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
