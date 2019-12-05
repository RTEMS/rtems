## Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([rtems-c-src-lib-libbsp-arm-xen],[_RTEMS_VERSION],[https://devel.rtems.org/newticket])
RTEMS_TOP(../../../../../..)
RTEMS_SOURCE_TOP
RTEMS_BUILD_TOP

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.12.2])
RTEMS_BSP_CONFIGURE



RTEMS_BSPOPTS_SET([BSP_DATA_CACHE_ENABLED],[*qemu],[])
RTEMS_BSPOPTS_SET([BSP_DATA_CACHE_ENABLED],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_DATA_CACHE_ENABLED],[enable data cache])

RTEMS_BSPOPTS_SET([BSP_INSTRUCTION_CACHE_ENABLED],[*qemu],[])
RTEMS_BSPOPTS_SET([BSP_INSTRUCTION_CACHE_ENABLED],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_INSTRUCTION_CACHE_ENABLED],[enable instruction cache])

RTEMS_BSPOPTS_SET([ARM_GENERIC_TIMER_USE_VIRTUAL],[*],[1])
RTEMS_BSPOPTS_HELP([ARM_GENERIC_TIMER_USE_VIRTUAL],[use virtual ARM generic timer])

RTEMS_BSPOPTS_SET([ARM_GENERIC_TIMER_UNMASK_AT_TICK],[*],[1])
RTEMS_BSPOPTS_HELP([ARM_GENERIC_TIMER_UNMASK_AT_TICK],[unmask the timer in the tick handler, since Xen will mask the virtual timer before injecting the interrupt to the guest])

RTEMS_BSPOPTS_SET([BSP_START_ZIMAGE_HEADER],[*],[1])
RTEMS_BSPOPTS_HELP([BSP_START_ZIMAGE_HEADER],[include zImage boot header])

RTEMS_BSPOPTS_SET([BSP_XEN_RAM_LENGTH],[*],[8M])
RTEMS_BSPOPTS_HELP([BSP_XEN_RAM_LENGTH],[length of memory area available to the BSP])

RTEMS_BSPOPTS_SET([BSP_XEN_NOCACHE_LENGTH],[*],[1M])
RTEMS_BSPOPTS_HELP([BSP_XEN_NOCACHE_LENGTH],[length of nocache RAM region])

RTEMS_BSPOPTS_SET([BSP_XEN_RAM_BASE],[*],[0x40000000])
RTEMS_BSPOPTS_HELP([BSP_XEN_RAM_BASE],[base address of memory area available to the BSP])

RTEMS_BSPOPTS_SET([BSP_XEN_LOAD_OFFSET],[*],[0x8000])
RTEMS_BSPOPTS_HELP([BSP_XEN_LOAD_OFFSET],[offset of RAM region from memory area base])

RTEMS_BSPOPTS_SET([BSP_XEN_MMU_LENGTH],[*],[16k])
RTEMS_BSPOPTS_HELP([BSP_XEN_MMU_LENGTH],[length of MMU region])

AC_DEFUN([XEN_LINKCMD],[
AC_ARG_VAR([$1],[$2; default $3])dnl
[$1]=[$]{[$1]:-[$3]}
])

XEN_LINKCMD([BSP_XEN_RAM_BASE],[normal RAM region origin],[${BSP_XEN_RAM_BASE}])
XEN_LINKCMD([BSP_XEN_RAM_LENGTH],[normal RAM region length],[${BSP_XEN_RAM_LENGTH}])
XEN_LINKCMD([BSP_XEN_LOAD_OFFSET],[entry point of guest],[${BSP_XEN_LOAD_OFFSET}])
XEN_LINKCMD([BSP_XEN_MMU_LENGTH],[MMU region length],[${BSP_XEN_MMU_LENGTH}])
XEN_LINKCMD([BSP_XEN_NOCACHE_LENGTH],[length of nocache RAM region],[${BSP_XEN_NOCACHE_LENGTH}])

RTEMS_BSP_CLEANUP_OPTIONS

# Explicitly list all Makefiles here
AC_CONFIG_FILES([
Makefile
linkcmds:../../../../../../bsps/arm/xen/start/linkcmds.in])
AC_OUTPUT
