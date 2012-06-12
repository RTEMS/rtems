#
#  Shared config file for the v850 GDB Simulator
#
#  CPU_CFLAGS is set by each specific variant.

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU=v850
RTEMS_CPU_MODEL=v850

# optimize flag: typically -O2
CFLAGS_OPTIMIZE_V = -O2 -g
