dnl $Id$
dnl

dnl BSP_BOOTCARD_OPTIONS - some autoconf voodoo to
dnl provide each BSPs configure script with the standard options allowed
dnl by boot_card().  Currently, this is
dnl
dnl   - Can optionally dirty memory at boot time.
dnl

dnl To be used in bsp-configure scripts

AC_DEFUN([BSP_BOOTCARD_OPTIONS],[
RTEMS_BSPOPTS_SET([BSP_DIRTY_MEMORY],[*],[0])
RTEMS_BSPOPTS_HELP([BSP_DIRTY_MEMORY],
[If defined, then PSIM will put a non-zero pattern into the RTEMS
 Workspace and C program heap.  This should assist in finding
 code that assumes memory starts set to zero.])
])
