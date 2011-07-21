#
#  Config file for LPC24XX.
#
#  $Id$
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = arm

CPU_CFLAGS = -mstructure-size-boundary=8 -mcpu=arm7tdmi-s -mfpu=vfp -mfloat-abi=soft -mthumb \
	-fno-schedule-insns2 -fno-peephole2

CFLAGS_OPTIMIZE_V = -Os -g
