include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = arm

CPU_CFLAGS = -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=hard -mtune=cortex-a9

CFLAGS_OPTIMIZE_V ?= -O2 -g
