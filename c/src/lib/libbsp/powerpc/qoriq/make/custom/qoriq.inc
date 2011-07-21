#
#  Shared base config file for QorIQ BSPs
#
#  $Id$
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = powerpc

RTEMS_CPU_MODEL = qoriq

CPU_CFLAGS = -mcpu=8540 -meabi -msdata -fno-common -mfloat-gprs=double -mstrict-align \
	-D__ppc_generic \
	-Wextra -Wno-unused -Wpointer-arith -Wcast-qual -Wconversion -Wmissing-prototypes

CFLAGS_OPTIMIZE_V = -Os -g
