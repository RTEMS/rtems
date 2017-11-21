dnl
dnl RTEMS_BSP_CLEANUP_OPTIONS - some autoconf voodoo to provide each BSPs'
dnl configure script with the standard options supported by the shared
dnl implementation of bsp_fatal_extension().
dnl
dnl To be used in bsp-configure scripts
dnl
dnl USAGE:
dnl    RTEMS_BSP_CLEANUP_OPTIONS

AC_DEFUN([RTEMS_BSP_CLEANUP_OPTIONS],[
RTEMS_BSPOPTS_SET([BSP_PRESS_KEY_FOR_RESET],[*],[0])
RTEMS_BSPOPTS_HELP([BSP_PRESS_KEY_FOR_RESET],
[If defined to a non-zero value, print a message and wait until pressed before
resetting board when application exits.])

RTEMS_BSPOPTS_SET([BSP_RESET_BOARD_AT_EXIT],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_RESET_BOARD_AT_EXIT],
[If defined to a non-zero value, reset the board when the application exits.])

RTEMS_BSPOPTS_SET([BSP_PRINT_EXCEPTION_CONTEXT],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_PRINT_EXCEPTION_CONTEXT],
[If defined to a non-zero value, prints the exception context when an
unexpected exception occurs.])

RTEMS_BSPOPTS_SET([BSP_VERBOSE_FATAL_EXTENSION],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_VERBOSE_FATAL_EXTENSION],
[If defined to a non-zero value, prints the some information in case of a fatal
error.])])
