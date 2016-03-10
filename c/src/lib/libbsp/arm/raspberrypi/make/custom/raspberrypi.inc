#
#  Config file for Raspberry Pi variants.
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = arm

CFLAGS_OPTIMIZE_V ?= -O2 -g
CFLAGS_OPTIMIZE_V += -ffunction-sections -fdata-sections

LDFLAGS = -Wl,--gc-sections

# This defines the operations performed on the linked executable.
# is currently required.
define bsp-post-link
    $(OBJCOPY) -O binary --strip-all \
        $(basename $@)$(EXEEXT) $(basename $@)$(DOWNEXT)
    $(SIZE) $(basename $@)$(EXEEXT)
endef
