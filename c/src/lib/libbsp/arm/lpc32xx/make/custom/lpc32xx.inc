#
#  Config file for LPC32XX.
#
#  $Id$
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = arm

CPU_CFLAGS = -mstructure-size-boundary=8 -mcpu=arm926ej-s -mfpu=vfp -mfloat-abi=soft -mthumb \
	-fno-schedule-insns2

CFLAGS_OPTIMIZE_V = -O2 -g
