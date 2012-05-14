#
#  Shared base config file for GEN5200 BSPs
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU=powerpc
RTEMS_CPU_MODEL=mpc6xx

#  This contains the compiler options necessary to select the CPU model
#  and (hopefully) optimize for it. 
#
CPU_CFLAGS = -mcpu=603e -mstrict-align \
             -meabi -msdata -fno-common

# optimize flag: typically -O2
CFLAGS_OPTIMIZE_V = -O2 -g -fno-keep-inline-functions
