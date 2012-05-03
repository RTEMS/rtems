dnl
dnl BSP_CLEANUP_OPTIONS - some autoconf voodoo to provide each BSPs'
dnl configure script with the standard options supported by the shared
dnl implementation of bsp_cleanup().
dnl
dnl   - Can optionally dirty memory at boot time.
dnl

dnl To be used in bsp-configure scripts

dnl USAGE:
dnl    RTEMS_BSP_CLEANUP_OPTIONS([0|1], [0|1]) 
dnl WHERE:
dnl    argument 1 indicates the default value for BSP_PRESS_KEY_FOR_RESET
dnl    argument 2 indicates the default value for BSP_RESET_BOARD_AT_EXIT

AC_DEFUN([RTEMS_BSP_CLEANUP_OPTIONS],[
RTEMS_BSPOPTS_SET([BSP_PRESS_KEY_FOR_RESET],[*],[$1])
RTEMS_BSPOPTS_HELP([BSP_PRESS_KEY_FOR_RESET],
[If defined, print a message and wait until pressed before resetting
 board when application exits.])

RTEMS_BSPOPTS_SET([BSP_RESET_BOARD_AT_EXIT],[*],[$2])
RTEMS_BSPOPTS_HELP([BSP_RESET_BOARD_AT_EXIT],
[If defined, reset the board when the application exits.])
])
