## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-powerpc-motorola_powerpc],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_CHECK_TOOL(NM,nm,no)
RTEMS_PROG_CCAS

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

AS=$CC
AM_PROG_AS

RTEMS_BSPOPTS_SET_DATA_CACHE_ENABLED([*],[1])
RTEMS_BSPOPTS_HELP_DATA_CACHE_ENABLED

RTEMS_BSPOPTS_SET_INSTRUCTION_CACHE_ENABLED([*],[1])
RTEMS_BSPOPTS_HELP_INSTRUCTION_CACHE_ENABLED

RTEMS_BSPOPTS_SET([CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK], [*], [1])
RTEMS_BSPOPTS_HELP([CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK],
[If defined then the BSP may reduce the available memory size
initially. This can be useful for debugging (reduce the core
size) or dynamic loading (std gcc text offsets/jumps are < +/-32M).
Note that the policy can still be defined by the application
(see sbrk.c, BSP_sbrk_policy). By undefining
CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK this feature is removed
and a little memory is saved.])

RTEMS_BSPOPTS_SET([mvme2100],[mvme2100],[1])
RTEMS_BSPOPTS_SET([mvme2100],[*],[])
RTEMS_BSPOPTS_HELP([mvme2100],
[Defined for MVME2100 -- undefined for others])

RTEMS_BSPOPTS_SET([mpc8240],[mvme2100],[1])
RTEMS_BSPOPTS_SET([mpc8240],[*],[])
RTEMS_BSPOPTS_HELP([mpc8240],
[Defined for boards with MPC8240 -- undefined for others])

RTEMS_BSPOPTS_SET([qemu],[qemuprep*],[1])
RTEMS_BSPOPTS_SET([qemu],[*],[])
RTEMS_BSPOPTS_HELP([qemu],[Defined for QEMU BSP -- undefined for others])

RTEMS_BSP_CLEANUP_OPTIONS(0, 0)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])
case "${RTEMS_BSP}" in
	qemuprep* )
AC_CONFIG_FILES([qemu_fakerom/Makefile])
	;;
esac
AC_CONFIG_FILES([bootloader/Makefile])

RTEMS_PPC_EXCEPTIONS

AM_CONDITIONAL([QEMU],[case xx"${RTEMS_BSP}" in xxqemuprep*) true;; *) false;; esac])

AC_OUTPUT
