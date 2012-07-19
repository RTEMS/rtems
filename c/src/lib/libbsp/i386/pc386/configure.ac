## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-i386-pc386],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_CHECK_TOOL(OBJCOPY,objcopy,:)
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING
RTEMS_CHECK_SMP

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

if test X${CLOCK_DRIVER_USE_TSC} = X1 -a X${CLOCK_DRIVER_USE_8254} = X1 ; then
  AC_MSG_ERROR([pc386 both TSC and 8254 specified for clock driver])
fi

## The PC386 BSP supports SMP configurations if this is enabled.
RTEMS_BSPOPTS_SET([BSP_HAS_SMP],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_HAS_SMP],
[Always defined when on a pc386 to enable the pc386 support for
 determining the CPU core number in an SMP configuration.])

## if this is an i386, does gas have good code16 support?
RTEMS_I386_GAS_CODE16
AM_CONDITIONAL(RTEMS_GAS_CODE16,[test "$RTEMS_GAS_CODE16" = "yes"])

AM_CONDITIONAL(HAS_NETWORKING,[test "$HAS_NETWORKING" = "yes"])
AM_CONDITIONAL(HAS_SMP,[test "$rtems_cv_HAS_SMP" = "yes"])

AC_SUBST([RTEMS_ROOT],[${rtems_updir}'$(top_builddir)'])

RTEMS_CONFIG_BUILD_SUBDIRS(tools)

AC_SUBST(RTEMS_BSP)

RTEMS_BSP_CLEANUP_OPTIONS(1, 1)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
