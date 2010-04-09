#
#  Config file for LPC24XX.
#
#  $Id$
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = arm

CPU_CFLAGS = -mcpu=arm7tdmi-s -mthumb -mstructure-size-boundary=8 \
	-Wextra -Wno-unused -Wpointer-arith -Wcast-qual -Wconversion -Wmissing-prototypes

CFLAGS_OPTIMIZE_V = -Os -g
