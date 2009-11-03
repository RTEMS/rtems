#
#  Config file for the genmcf548x BSP
#

RTEMS_CPU=m68k
RTEMS_CPU_MODEL=mcf548x

include $(RTEMS_ROOT)/make/custom/default.cfg

#  This contains the compiler options necessary to select the CPU model
#  and (hopefully) optimize for it. 
CPU_CFLAGS = -mcfv4e -Wa,-memac

# optimize flag: typically -O2
CFLAGS_OPTIMIZE_V = -O2 -g -fomit-frame-pointer 

define bsp-post-link
	$(OBJCOPY) -O binary --strip-all $(basename $@).exe \
	    $(basename $@)$(DOWNEXT)
	$(default-bsp-post-link)
endef
