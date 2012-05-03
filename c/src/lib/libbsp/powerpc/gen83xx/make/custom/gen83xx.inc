#
#  Shared base config file for GEN83xx BSPs
#

include $(RTEMS_ROOT)/make/custom/default.cfg
#
# FIXME: we might want to use the e300 CPU model explicitely,
# using RTEMS_CPU_MODEL=e300 and -me300
# but I doubt the compiler actually generates different code
#
RTEMS_CPU=powerpc
RTEMS_CPU_MODEL=mpc83xx

#  This contains the compiler options necessary to select the CPU model
#  and (hopefully) optimize for it. 
#
CPU_CFLAGS = -mcpu=603e -meabi -msdata -fno-common -mstrict-align

# optimize flag: typically -O2
CFLAGS_OPTIMIZE_V = -O2 -g -fno-keep-inline-functions
