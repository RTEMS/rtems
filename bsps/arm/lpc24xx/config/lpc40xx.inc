#
# Config file for LPC40XX.
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = arm

CPU_CFLAGS = -mthumb -march=armv7-m -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mtune=cortex-m4

CFLAGS_OPTIMIZE_V = -O2 -g
CFLAGS_OPTIMIZE_V += -ffunction-sections -fdata-sections

LDFLAGS = -Wl,--gc-sections
