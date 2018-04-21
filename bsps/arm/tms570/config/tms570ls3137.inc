#
# Config file for TMS570LS3137 board.
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = arm

CPU_CFLAGS  = -march=armv7-r -mthumb -mbig-endian
CPU_CFLAGS += -mfpu=vfpv3-d16 -mfloat-abi=hard

CFLAGS_OPTIMIZE_V = -O2 -ggdb
CFLAGS_OPTIMIZE_V += -ffunction-sections -fdata-sections

LDFLAGS = -Wl,--gc-sections

BINEXT?=.bin

# This defines the operations performed on the linked executable.
# is currently required.
define bsp-post-link
    $(OBJCOPY) -O binary --strip-all \
        $(basename $@)$(EXEEXT) $(basename $@)$(BINEXT)
    $(SIZE) $(basename $@)$(EXEEXT)
endef
