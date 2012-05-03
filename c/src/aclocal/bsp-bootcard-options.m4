dnl
dnl BSP_BOOTCARD_OPTIONS - some autoconf voodoo to provide each BSPs'
dnl configure script with the standard options supported by boot_card()
dnl and other required parts of the BSP Framework.  Currently, this is
dnl
dnl   - Can optionally dirty memory at boot time.
dnl

dnl To be used in bsp-configure scripts

AC_DEFUN([RTEMS_BSP_BOOTCARD_OPTIONS],[
RTEMS_BSPOPTS_SET([BSP_DIRTY_MEMORY],[*],[0])
RTEMS_BSPOPTS_HELP([BSP_DIRTY_MEMORY],
[If defined, then the BSP Framework will put a non-zero pattern into
 the RTEMS Workspace and C program heap.  This should assist in finding
 code that assumes memory starts set to zero.])
])
