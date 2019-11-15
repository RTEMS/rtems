## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-i386-pc386],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP
RTEMS_BSP_LINKCMDS

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_CHECK_TOOL(OBJCOPY,objcopy,:)


RTEMS_BSPOPTS_SET([BSP_ENABLE_VGA],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_ENABLE_VGA],
[Set if the VGA and keyboard console support is enabled.])
AM_CONDITIONAL(RTEMS_VGA,[test "$BSP_ENABLE_VGA" = "1"])

RTEMS_BSPOPTS_SET([BSP_ENABLE_COM1_COM4],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_ENABLE_COM1_COM4],
[Set if COM1..COM4 support is enabled.])

RTEMS_BSPOPTS_SET([BSP_ENABLE_IDE],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_ENABLE_IDE],
[Set if IDE support is enabled.])
AM_CONDITIONAL(HAS_IDE,[test "$BSP_ENABLE_IDE" = "1"])

RTEMS_BSPOPTS_SET([USE_COM1_AS_CONSOLE],[*],[0])
RTEMS_BSPOPTS_HELP([USE_COM1_AS_CONSOLE],
[Determines, whether the console will be associated with the standard
 VGA display or with the COM1 serial port.  Currently only the VGA
 display and COM1 support printk.])

RTEMS_BSPOPTS_SET([IDE_USE_PRIMARY_INTERFACE],[*],[1])
RTEMS_BSPOPTS_HELP([IDE_USE_PRIMARY_INTERFACE],
[Determines, whether RTEMS will try to use the primary IDE interface.
 Disable it, if:
 - you have no primary IDE interface
 - or you have no disk attached to this interface
 - or you do not want to access disks attached to this interface])

RTEMS_BSPOPTS_SET([IDE_USE_SECONDARY_INTERFACE],[*],[0])
RTEMS_BSPOPTS_HELP([IDE_USE_SECONDARY_INTERFACE],
[Determines, whether RTEMS will try to use the secondary IDE interface.
 Enable it, if:
 - you have a secondary IDE interface
 - and you have at least one disk attached to this interface
 - and you do want to access disks attached to this interface])

RTEMS_BSPOPTS_SET([BSP_VIDEO_80x50],[*],[0])
RTEMS_BSPOPTS_HELP([BSP_VIDEO_80x50],
[If defined, set the VGA display to 80x50.])

RTEMS_BSPOPTS_SET([CLOCK_DRIVER_USE_TSC],[*],[0])
RTEMS_BSPOPTS_HELP([CLOCK_DRIVER_USE_TSC],
[If enabled, the clock driver will use the TSC register available
 with Pentium-class CPUs to report close to nanosecond-accuracy
 clock times.
 Enable it, if:
 - you have nanosecond timing enabled (you do NOT have
   USE_TICKS_FOR_CPU_USAGE_STATISTICS enabled)
 - you do NOT have CLOCK_DRIVER_USE_8254 enabled (use one, the other,
   or neither)
 - you have a Pentium which supports TSC (all Intels, and probably
   all or most clones)
 - you do not have a variable-speed CPU clock. Note that some
   motherboard BIOS will automatically vary clock speed for thermal
   control. Note also, however, that really new Pentium-class chips
   from Intel and AMD will maintain a constant-rate TSC regardless.
])

#define CLOCK_DRIVER_USE_8254 $CLOCK_DRIVER_USE_8254
RTEMS_BSPOPTS_SET([CLOCK_DRIVER_USE_8254],[*],[0])
RTEMS_BSPOPTS_HELP([CLOCK_DRIVER_USE_8254],
[If enabled, the clock driver will use the good old 8254 chip
 to report microsecond-accuracy clock times.
 Enable it, if:
 - you have nanosecond timing enabled (you do NOT have
   USE_TICKS_FOR_CPU_USAGE_STATISTICS enabled)
 - you do NOT have CLOCK_DRIVER_USE_TSC enabled (use one, the other,
   or neither)
 - you do not mind adding roughly 5 microseconds to each context switch.
])

if test "${USE_CIRRUS_GD5446}" != "1" -a "${USE_VGA}" != "1"; then
  USE_VBE_RM=1;
  if test -z "${NUM_APP_DRV_GDT_DESCRIPTORS}"; then
      NUM_APP_DRV_GDT_DESCRIPTORS=2 ;
  else
      NUM_APP_DRV_GDT_DESCRIPTORS+=2 ;
  fi
fi
RTEMS_BSPOPTS_SET([NUM_APP_DRV_GDT_DESCRIPTORS],[*],[0])
RTEMS_BSPOPTS_HELP([NUM_APP_DRV_GDT_DESCRIPTORS],
[Defines how many descriptors in GDT may be allocated for application or
 driver usage.])

RTEMS_BSPOPTS_SET([USE_CIRRUS_GD5446],[*],[0])
RTEMS_BSPOPTS_HELP([USE_CIRRUS_GD5446],
[If defined, enables use of the Cirrus GD5446 controller rather than the
 controller utilizing Vesa Bios Extensions - real mode interface.

 NOTE: This has only been tested on Qemu.])
AM_CONDITIONAL(USE_CIRRUS_GD5446,test "$USE_CIRRUS_GD5446" = "1")

RTEMS_BSPOPTS_SET([USE_VGA],[*],[0])
RTEMS_BSPOPTS_HELP([USE_VGA],
[If defined, enables use of the very basic VGA driver for framebuffer support
 rather than the controller utilizing Vesa Bios Extensions - real mode
 interface.])
AM_CONDITIONAL(USE_VGA,test "$USE_VGA" = "1")

RTEMS_BSPOPTS_SET([USE_VBE_RM],[*],[0])
RTEMS_BSPOPTS_HELP([USE_VBE_RM],
[Defined by default. Enables use of the Vesa Bios Extensions - real mode
 interface, which enables graphical mode and introduce it upon bootup.])
AM_CONDITIONAL(USE_VBE_RM,test "$USE_VBE_RM" = "1")

if test X${CLOCK_DRIVER_USE_TSC} = X1 -a X${CLOCK_DRIVER_USE_8254} = X1 ; then
  AC_MSG_ERROR([pc386 both TSC and 8254 specified for clock driver])
fi

RTEMS_BSPOPTS_SET([BSP_GDB_STUB],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_GDB_STUB],
[Defined by default. Enables use of the GDB stub for debugging via a
 serial port.])
AM_CONDITIONAL(BSP_GDB_STUB, test "$BSP_GDB_STUB" = "1")

## if this is an i386, does gas have good code16 support?
RTEMS_I386_GAS_CODE16
AM_CONDITIONAL(RTEMS_GAS_CODE16,[test "$RTEMS_GAS_CODE16" = "yes"])


AC_SUBST([RTEMS_ROOT],[${rtems_updir}'$(top_builddir)'])

AC_SUBST(RTEMS_BSP)

RTEMS_BSP_CLEANUP_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
