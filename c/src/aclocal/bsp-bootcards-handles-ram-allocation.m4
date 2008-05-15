dnl $Id$
dnl

dnl RTEMS_BSP_BOOTCARD_HANDLES_RAM_ALLOCATION - some autoconf voodoo to
dnl indicate that the BSP is using the shared RAM allocation code in 
dnl bootcard.c.  This is a new way of doing things that will reduce the
dnl size of each BSP's bspstart.c and open the door for another round
dnl of benefits for BSPs that use this scheme.
dnl
dnl Benefits:
dnl   - Reduced code in bspstart.c
dnl   - Can optionally dirty memory at boot time.
dnl
dnl NOTE: This macro is intended to be temporary.  When most of the
dnl       BSPs use the new shared method, we need to either bulk edit
dnl       the remaining BSPs to eliminate code using the old way. Or
dnl       switch the sense of this macro to be required on BSPs using
dnl       the old way.
dnl
dnl       AGAIN: This is intended to be temporary.  Please help by
dnl              updating BSPs to use the new scheme. 

dnl To be used in bsp-configure scripts

AC_DEFUN([RTEMS_BSP_BOOTCARD_HANDLES_RAM_ALLOCATION],
 [AC_DEFINE_UNQUOTED([BSP_BOOTCARD_HANDLES_RAM_ALLOCATION],
  [1],
  [BSP uses shared logic in bootcard.c]
)
RTEMS_BSPOPTS_SET([BSP_DIRTY_MEMORY],[*],[0])
RTEMS_BSPOPTS_HELP([BSP_DIRTY_MEMORY],
[If defined, then PSIM will put a non-zero pattern into the RTEMS
 Workspace and C program heap.  This should assist in finding
 code that assumes memory starts set to zero.])
])
