#
#  Config file for LPC32XX.
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = arm

CPU_CFLAGS = -mcpu=arm926ej-s -mthumb

CFLAGS_OPTIMIZE_V ?= -O2 -g
