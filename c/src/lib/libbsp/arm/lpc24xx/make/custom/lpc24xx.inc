#
#  Config file for LPC24XX.
#
#  $Id$
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = arm

CPU_CFLAGS = -mcpu=arm7tdmi-s -mstructure-size-boundary=8 -mthumb

CFLAGS_OPTIMIZE_V = -Os -g

LDFLAGS += -qnolinkcmds -T $(LPC24XX_LINKCMDS) 
