#
#  Config file for BeagleBoard.
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = arm

CPU_CFLAGS = -mcpu=cortex-a8

CFLAGS_OPTIMIZE_V ?= -O2 -g
