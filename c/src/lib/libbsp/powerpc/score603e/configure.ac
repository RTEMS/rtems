## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-powerpc-ep1a],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_BSPOPTS_SET_DATA_CACHE_ENABLED([*],[])
RTEMS_BSPOPTS_HELP_DATA_CACHE_ENABLED

RTEMS_BSPOPTS_SET_INSTRUCTION_CACHE_ENABLED([*],[])
RTEMS_BSPOPTS_HELP_INSTRUCTION_CACHE_ENABLED

## FIXME: This should be a 1 out of 3 selection
## and is somehow coupled to USE_DINK (cf. include/gen2.h)
RTEMS_BSPOPTS_SET([SCORE603E_USE_SDS],[*],[0])
RTEMS_BSPOPTS_HELP([SCORE603E_USE_SDS],
[use SDS ROM monitor])

RTEMS_BSPOPTS_SET([SCORE603E_OPEN_FIRMWARE],[*],[0])
RTEMS_BSPOPTS_HELP([SCORE603E_OPEN_FIRMWARE],
[use Open Firmware ROM monitor])

RTEMS_BSPOPTS_SET([SCORE603E_USE_NONE],[*],[0])
RTEMS_BSPOPTS_HELP([SCORE603E_USE_NONE],
[use no ROM monitor])

RTEMS_BSPOPTS_SET([SCORE603E_USE_DINK],[*],[1])
RTEMS_BSPOPTS_HELP([SCORE603E_USE_DINK],
[FIXME: Missing explanation.])

RTEMS_BSPOPTS_SET([CONSOLE_USE_INTERRUPTS],[*],[0])
RTEMS_BSPOPTS_HELP([CONSOLE_USE_INTERRUPTS],
[whether using console interrupts])

RTEMS_BSPOPTS_SET([HAS_PMC_PSC8],[*],[1])
RTEMS_BSPOPTS_HELP([HAS_PMC_PSC8],
[whether has a PSC8 PMC board attached to PMC slot])

RTEMS_BSPOPTS_SET([INITIALIZE_COM_PORTS],[*],[0])
RTEMS_BSPOPTS_HELP([INITIALIZE_COM_PORTS],
[FIXME: Missing explanation])

RTEMS_BSPOPTS_SET([PPC_USE_SPRG],[*],[0])
RTEMS_BSPOPTS_HELP([PPC_USE_SPRG],
[If defined, then the PowerPC specific code in RTEMS will use some
of the special purpose registers to slightly optimize interrupt
response time.  The use of these registers can conflict with
other tools like debuggers.])

RTEMS_BSPOPTS_SET([CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK], [*], [1])
RTEMS_BSPOPTS_HELP([CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK],
[If defined then the BSP may reduce the available memory size
initially. This can be useful for debugging (reduce the core
size) or dynamic loading (std gcc text offsets/jumps are < +/-32M).
Note that the policy can still be defined by the application
(see sbrk.c, BSP_sbrk_policy). By undefining
CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK this feature is removed
and a little memory is saved.])

RTEMS_BSPOPTS_SET([PPC_VECTOR_FILE_BASE],[*],[0x0100])
RTEMS_BSPOPTS_HELP([PPC_VECTOR_FILE_BASE],
[This defines the base address of the exception table.
 NOTE: Vectors are actually at 0xFFF00000 but file starts at offset.])

RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

RTEMS_BSP_CLEANUP_OPTIONS(0, 0)

# Explicitly list all Makefiles here
AC_CONFIG_FILES([Makefile])

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
