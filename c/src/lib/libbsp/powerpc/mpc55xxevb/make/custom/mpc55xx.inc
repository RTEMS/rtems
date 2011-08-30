##
#
# @file
#
# @ingroup mpc55xx_config
#
# @brief Shared configuration file for the MPC55xx board family.
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = powerpc

RTEMS_CPU_MODEL = mpc55xx

CPU_CFLAGS_FLOAT?=-msoft-float

# FIXME
CPU_CFLAGS = -mcpu=8540 -meabi -msdata -fno-common $(CPU_CFLAGS_FLOAT) \
    -D__ppc_generic -mstrict-align

CFLAGS_OPTIMIZE_V = -O2 -g -fno-keep-inline-functions
