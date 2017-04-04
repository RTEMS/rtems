#
#  Shared base config file for QorIQ BSPs
#

include $(RTEMS_ROOT)/make/custom/default.cfg

RTEMS_CPU = powerpc

RTEMS_CPU_MODEL = qoriq

CPU_CFLAGS = -mcpu=8540 -meabi -msdata=sysv -fno-common -mstrict-align \
	-mspe -mabi=spe -mfloat-gprs=double \
	-D__ppc_generic

CFLAGS_OPTIMIZE_V = -O2 -g
CFLAGS_OPTIMIZE_V += -ffunction-sections -fdata-sections

LDFLAGS = -Wl,--gc-sections

# define bsp-post-link
#   $(OBJCOPY) -O binary '$@' '$(basename $@).bin'
#   gzip -f -9 '$(basename $@).bin'
#   mkimage -A ppc -O linux -T kernel -a 0x4000 -e 0x4000  -name '$(notdir $@)' -d '$(basename $@).bin.gz' '$(basename $@).img'
#   $(default-bsp-post-link)
# endef
