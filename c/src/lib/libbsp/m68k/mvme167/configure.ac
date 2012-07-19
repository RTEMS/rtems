## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-m68k-mvme167],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

# HACK: Propagate #define mvme167 into bspopts.h
# Used inside ../mvme162/include/mvme16x_hw.h
AC_DEFINE_UNQUOTED([mvme167],[1],
[If building the mvme167 BSP])

RTEMS_BSPOPTS_SET([CD2401_INT_LEVEL],[*],[1])
RTEMS_BSPOPTS_HELP([CD2401_INT_LEVEL],
[Interrupt level for the CD2401(when CD2401_IO_MODE == 1).])

RTEMS_BSPOPTS_SET([CD2401_IO_MODE],[*],[0])
RTEMS_BSPOPTS_HELP([CD2401_IO_MODE],
[0 for polled I/O, 1 for interrupt-driven.])

RTEMS_BSPOPTS_SET([CD2401_USE_TERMIOS],[*],[0])
RTEMS_BSPOPTS_HELP([CD2401_USE_TERMIOS],
[1 for using termios based console.])

RTEMS_BSPOPTS_SET([CONSOLE_MINOR],[*],[1])
RTEMS_BSPOPTS_HELP([CONSOLE_MINOR],
[Port to use for the RTEMS console.
      0 - /dev/tty0, Serial Port 1/Console on the MVME712M.
      1 - /dev/tty1, Serial Port 2/TTY01 on the MVME712M.
      2 - /dev/tty2, Serial Port 3 on the MVME712M.
      3 - /dev/tty3, Serial Port 4 on the MVME712M.])

RTEMS_BSPOPTS_SET([PRINTK_MINOR],[*],[1])
RTEMS_BSPOPTS_HELP([PRINTK_MINOR],
[Port to use for printk debugging output.
      0 - /dev/tty0, Serial Port 1/Console on the MVME712M.
      1 - /dev/tty1, Serial Port 2/TTY01 on the MVME712M.
      2 - /dev/tty2, Serial Port 3 on the MVME712M.
      3 - /dev/tty3, Serial Port 4 on the MVME712M.])

RTEMS_BSP_CLEANUP_OPTIONS(0, 0)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
