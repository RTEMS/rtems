#
#  Shared base config file for QorIQ BSPs
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = powerpc

RTEMS_CPU_MODEL = qoriq

CPU_CFLAGS = -mcpu=8540 -meabi -msdata -fno-common -mstrict-align \
	-mspe -mabi=spe -mfloat-gprs=double \
	-D__ppc_generic

CFLAGS_OPTIMIZE_V = -Os -g
