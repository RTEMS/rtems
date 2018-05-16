#
#  Config file for a PowerPC MPC8xx-based TQM8xx carrier card
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU=powerpc

CPU_CFLAGS = -mcpu=860 -mstrict-align -fno-strict-aliasing \
             -meabi -msdata=sysv -fno-common

# optimize flag: typically -O2
CFLAGS_OPTIMIZE_V = -O2 -g -fno-keep-inline-functions
CFLAGS_OPTIMIZE_V += -ffunction-sections -fdata-sections

LDFLAGS = -Wl,--gc-sections

define bsp-post-link
	cp $(basename $@)$(EXEEXT) $(basename $@)$(DOWNEXT) 
	$(STRIP) $(basename $@)$(DOWNEXT)
	$(default-bsp-post-link)
endef
