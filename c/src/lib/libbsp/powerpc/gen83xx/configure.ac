## Process this file with autoconf to produce a configure script.
## 
## $Id$

AC_PREREQ(2.60)
AC_INIT([rtems-c-src-lib-libbsp-powerpc-gen83xx],[_RTEMS_VERSION],[http://www.rtems.org/bugzilla])
AC_CONFIG_SRCDIR([bsp_specs])
RTEMS_TOP(../../../../../..)

RTEMS_CANONICAL_TARGET_CPU
AM_INIT_AUTOMAKE([no-define nostdinc foreign 1.10])
RTEMS_BSP_CONFIGURE

RTEMS_PROG_CC_FOR_TARGET([-ansi -fasm])
RTEMS_CANONICALIZE_TOOLS
RTEMS_PROG_CCAS

RTEMS_BSPOPTS_SET([DATA_CACHE_ENABLE],[*],[1])
RTEMS_BSPOPTS_HELP([DATA_CACHE_ENABLE],
[If defined, the data cache will be enabled after address translation
 is turned on.])

RTEMS_BSPOPTS_SET([INSTRUCTION_CACHE_ENABLE],[*],[1])
RTEMS_BSPOPTS_HELP([INSTRUCTION_CACHE_ENABLE],
[If defined, the instruction cache will be enabled after address translation
 is turned on.])

RTEMS_BSPOPTS_SET([MPC8313ERDB],[mpc8313erdb],[1])
RTEMS_BSPOPTS_HELP([MPC8313ERDB],
[If defined, use custom settings for the mpc8313erdb BSP.])

RTEMS_BSPOPTS_SET([MPC8349EAMDS],[mpc8349eamds],[1])
RTEMS_BSPOPTS_HELP([MPC8349EAMDS],
[If defined, use custom settings for the mpc8349eamds BSP.])

RTEMS_BSPOPTS_SET([HSC_CM01],[hsc_cm01],[1])
RTEMS_BSPOPTS_HELP([HSC_CM01],
[If defined, use custom settings for the hsc_cm01 BSP.])

RTEMS_BSPOPTS_SET([MPC8349],[mpc8313erdb],[1])
RTEMS_BSPOPTS_SET([MPC8349],[mpc8349eamds],[1])
RTEMS_BSPOPTS_SET([MPC8349],[hsc_cm01],[1])
RTEMS_BSPOPTS_HELP([MPC8349],
[If defined, use custom settings for the MPC8349 libcpu family.])


RTEMS_CHECK_NETWORKING
AM_CONDITIONAL(HAS_NETWORKING,test "$HAS_NETWORKING" = "yes")

# Explicitly list all Makefiles here 
AC_CONFIG_FILES([Makefile])

BSP_BOOTCARD_OPTIONS

RTEMS_PPC_EXCEPTIONS

AC_OUTPUT
