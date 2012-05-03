#
#  Config file for LPC24XX.
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = arm

CPU_CFLAGS = -mcpu=arm7tdmi-s -mthumb

CFLAGS_OPTIMIZE_V = -Os -g
