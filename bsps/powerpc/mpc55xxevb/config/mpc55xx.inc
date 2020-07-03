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

CPU_CFLAGS = -mcpu=8540 -msoft-float -meabi -msdata=sysv -fno-common \
    -mstrict-align

CFLAGS_OPTIMIZE_V = -O2 -g -fno-keep-inline-functions
CFLAGS_OPTIMIZE_V += -ffunction-sections -fdata-sections

LDFLAGS = -Wl,--gc-sections
