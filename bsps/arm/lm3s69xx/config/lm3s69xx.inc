#
#  Config file for LM3S69XX.
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = arm

CPU_CFLAGS = -mthumb -mcpu=cortex-m3

CFLAGS_OPTIMIZE_V = -O2 -g

# Add CFLAGS and LDFLAGS for compiling and linking with per item sections
CFLAGS_OPTIMIZE_V += -ffunction-sections -fdata-sections
LDFLAGS            = -Wl,--gc-sections

define bsp-post-link
	$(default-bsp-post-link)
	$(OBJCOPY) -O binary $(basename $@)$(EXEEXT) $(basename $@)$(DOWNEXT)
endef
