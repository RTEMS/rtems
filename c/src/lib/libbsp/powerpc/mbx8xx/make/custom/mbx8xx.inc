#
#  Config file for a PowerPC MPC821- or MPC860-based MBX card
#
#  This file is derived from:
#
#  Config file for a PowerPC 403 based helas403 card
#  Config file for MPC860 based Ethernet Comm Board
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU=powerpc
RTEMS_CPU_MODEL=mpc$(8XX_CPU_TYPE)

#  This section makes the target dependent options file.
#
#  Note that RTEMS_BSP matches the RTEMS_MBX_MODEL. Its value must be
#  defined in targopts.h, so the few places that require different code
#  for different MBX models can be distinguished. The value of
#  RTEMS_BSP is already defined in targopts.h and is one of:
#     mbx860_001    mbx821_001
#     mbx860_002    mbx821_002
#     mbx860_003    mbx821_003
#     mbx860_004    mbx821_004
#     mbx860_005    mbx821_005
#     mbx860_001b   mbx821_001b
#     mbx860_002b   mbx821_002b
#     mbx860_003b   mbx821_003b
#     mbx860_004b   mbx821_004b
#     mbx860_005b   mbx821_005b
#     mbx860_006b   mbx821_006b

#  The specific CPU model is defined, so the few places that require
#  different code for the MPC860 and MPC821 can be distinguished.
#  Either mpc860 or mpc821 is defined.
#
#  MBX8xx-specific options:
#

#  This contains the compiler options necessary to select the CPU model
#  and (hopefully) optimize for it.
#
CPU_CFLAGS = -mcpu=$(8XX_CPU_TYPE) -Dmpc$(8XX_CPU_TYPE) -D$(RTEMS_MBX_MODEL) \
	-meabi -msdata -fno-common

# optimize flag: typically -O2
CFLAGS_OPTIMIZE_V = -O2 -g -fno-keep-inline-functions

LDFLAGS += $(MBX8xx_LDFLAGS)

define bsp-post-link
	cp $(basename $@).exe $(basename $@)$(DOWNEXT) 
	$(STRIP) $(basename $@)$(DOWNEXT)
	$(default-bsp-post-link)
endef
