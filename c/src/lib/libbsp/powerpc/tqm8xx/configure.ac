## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-powerpc-tqm8xx],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.10])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSPOPTS_SET([PPC_USE_DATA_CACHE],[*],[1])
RTEMS_BSPOPTS_HELP([PPC_USE_DATA_CACHE],
[If defined, then the PowerPC specific code in RTEMS will use
 data cache instructions to optimize the context switch code.
 This code can conflict with debuggers or emulators.])

RTEMS_BSPOPTS_SET([DATA_CACHE_ENABLE],[*],[1])
RTEMS_BSPOPTS_HELP([DATA_CACHE_ENABLE],
[If defined, the data cache will be enabled after address translation
 is turned on.])

RTEMS_BSPOPTS_SET([INSTRUCTION_CACHE_ENABLE],[*],[1])
RTEMS_BSPOPTS_HELP([INSTRUCTION_CACHE_ENABLE],
[If defined, the instruction cache will be enabled after address translation
 is turned on.])

RTEMS_BSPOPTS_SET([CONSOLE_CHN],[*],[CONS_CHN_SMC1])
RTEMS_BSPOPTS_HELP([CONSOLE_CHN],
[(BSP--console driver) Must be defined to be one of CONS_CHN_SMC1, 
 CONS_CHN_SMC2, CONS_CHN_SCC1, CONS_CHN_SCC2, CONS_CHN_SCC3, or CONS_CHN_SCC4. 
 Determines which device will be registered as /dev/console.])

RTEMS_BSPOPTS_SET([PRINTK_CHN],[*],[CONS_CHN_SMC1])
RTEMS_BSPOPTS_HELP([PRINTK_CHN],
[(BSP--console driver)
 Must be defined to be one of CONS_CHN_SMC1, CONS_CHN_SMC2, CONS_CHN_SCC2,
 CONS_CHN_SCC3, or CONS_CHN_SCC4. Determines which device is used for output
 by printk(). If the port that printk() uses is also used for other
 I/O (e.g. if  PRINTK_CHN == CONSOLE_CHN), then both ports should 
 use the same type of I/O, otherwise the drivers will likely conflict with
 each other.])

RTEMS_BSPOPTS_SET([CONS_SMC1_MODE],[tqm8xx_stk8xx],[CONS_MODE_POLLED])
RTEMS_BSPOPTS_SET([CONS_SMC1_MODE],[pghplus],[CONS_MODE_POLLED])
RTEMS_BSPOPTS_SET([CONS_SMC1_MODE],[*],[CONS_MODE_UNUSED])
RTEMS_BSPOPTS_HELP([CONS_SMC1_MODE],
[(BSP--SMC1 UART IF mode) Must be defined if SMC1 is used as a tty (UART) 
 channel. Set it to CONS_MODE_POLLED for polled operation, CONS_MODE_IRQ for 
 interrupt driven (spooled) operation. Set it to CONS_MODE_UNUSED, if not used])

RTEMS_BSPOPTS_SET([CONS_SMC2_MODE],[tqm8xx_stk8xx],[CONS_MODE_POLLED])
RTEMS_BSPOPTS_SET([CONS_SMC2_MODE],[*],[CONS_MODE_UNUSED])
RTEMS_BSPOPTS_HELP([CONS_SMC2_MODE],
[(BSP--SMC2 UART IF mode) Must be defined if SMC2 is used as a tty (UART) 
 channel. Set it to CONS_MODE_POLLED for polled operation, CONS_MODE_IRQ for 
 interrupt driven (spooled) operation. Set it to CONS_MODE_UNUSED, if not used])

RTEMS_BSPOPTS_SET([CONS_SCC1_MODE],[*],[CONS_MODE_UNUSED])
RTEMS_BSPOPTS_HELP([CONS_SCC1_MODE],
[(BSP--SCC1 UART IF mode) Must be defined if SCC1 is used as a tty (UART) 
 channel. Set it to CONS_MODE_POLLED for polled operation, CONS_MODE_IRQ for 
 interrupt driven (spooled) operation. Set it to CONS_MODE_UNUSED, if not used])

RTEMS_BSPOPTS_SET([CONS_SCC2_MODE],[*],[CONS_MODE_UNUSED])
RTEMS_BSPOPTS_HELP([CONS_SCC2_MODE],
[(BSP--SCC2 UART IF mode) Must be defined if SCC2 is used as a tty (UART) 
 channel. Set it to CONS_MODE_POLLED for polled operation, CONS_MODE_IRQ for 
 interrupt driven (spooled) operation. Set it to CONS_MODE_UNUSED, if not used])

RTEMS_BSPOPTS_SET([CONS_SCC3_MODE],[*],[CONS_MODE_UNUSED])
RTEMS_BSPOPTS_HELP([CONS_SCC3_MODE],
[(BSP--SCC3 UART IF mode) Must be defined if SCC3 is used as a tty (UART) 
 channel. Set it to CONS_MODE_POLLED for polled operation, CONS_MODE_IRQ for 
 interrupt driven (spooled) operation. Set it to CONS_MODE_UNUSED, if not used])

RTEMS_BSPOPTS_SET([CONS_SCC4_MODE],[*],[CONS_MODE_UNUSED])
RTEMS_BSPOPTS_HELP([CONS_SCC4_MODE],
[(BSP--SCC4 UART IF mode) Must be defined if SCC4 is used as a tty (UART) 
 channel. Set it to CONS_MODE_POLLED for polled operation, CONS_MODE_IRQ for 
 interrupt driven (spooled) operation. Set it to CONS_MODE_UNUSED, if not used])

# add selection of primary network interface
RTEMS_BSPOPTS_SET([BSP_USE_NETWORK_FEC],[pghplus],[1])
RTEMS_BSPOPTS_SET([BSP_USE_NETWORK_FEC],[*],[0])
RTEMS_BSPOPTS_HELP([BSP_USE_NETWORK_FEC],
[If defined, then the BSP will use the Fast Ethernet Controller
 for 10/100MBit networking and used as primary networking interface.])

RTEMS_BSPOPTS_SET([BSP_USE_NETWORK_SCC],[pghplus],[0])
RTEMS_BSPOPTS_SET([BSP_USE_NETWORK_SCC],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_USE_NETWORK_SCC],
[If defined, then the BSP will use the Serial Communications Controller (SCC1)
 for 10MBit networking.])


# Explicitly list a Makefile here 
AC_CONFIG_FILES([Makefile])

RTEMS_BSP_BOOTCARD_HANDLES_RAM_ALLOCATION

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
