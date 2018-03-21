ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/compile.am

EXTRA_DIST =

noinst_PROGRAMS =

EXTRA_DIST += ppc403/README ppc403/vectors/README
if ppc4xx
# ppc403/clock
noinst_PROGRAMS += ppc403/clock.rel
ppc403_clock_rel_SOURCES = ppc403/clock/clock.c
ppc403_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
ppc403_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# ppc403/console
noinst_PROGRAMS += ppc403/console.rel
if ppc405
ppc403_console_rel_SOURCES = ppc403/console/console405.c
else
ppc403_console_rel_SOURCES = ppc403/console/console.c
endif
ppc403_console_rel_CPPFLAGS = $(AM_CPPFLAGS)
ppc403_console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# ppc403/tty_drv
if ppc405
noinst_PROGRAMS += ppc403/tty_drv.rel
ppc403_tty_drv_rel_SOURCES = ppc403/tty_drv/tty_drv.c ppc403/tty_drv/tty_drv.h
ppc403_tty_drv_rel_CPPFLAGS = $(AM_CPPFLAGS)
ppc403_tty_drv_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

endif # ppc4xx

if ppc405

## ppc4xx/include
endif # ppc405

if mpc6xx

# mpc6xx/mmu
noinst_PROGRAMS += mpc6xx/mmu.rel
mpc6xx_mmu_rel_SOURCES = mpc6xx/mmu/bat.c mpc6xx/mmu/bat.h \
    mpc6xx/mmu/pte121.c mpc6xx/mmu/pte121.h \
    mpc6xx/mmu/mmuAsm.S
mpc6xx_mmu_rel_CPPFLAGS = $(AM_CPPFLAGS)
mpc6xx_mmu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# mpc6xx/clock
noinst_PROGRAMS += mpc6xx/clock.rel
mpc6xx_clock_rel_SOURCES = mpc6xx/clock/c_clock.c mpc6xx/clock/c_clock.h
mpc6xx_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
mpc6xx_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# mpc6xx/altivec
noinst_PROGRAMS += mpc6xx/altivec.rel
mpc6xx_altivec_rel_SOURCES = mpc6xx/altivec/vec_sup.c mpc6xx/altivec/vec_sup_asm.S
mpc6xx_altivec_rel_CPPFLAGS = $(AM_CPPFLAGS)
mpc6xx_altivec_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif
EXTRA_DIST      += mpc6xx/altivec/README

# e500/clock
if e500_clock
noinst_PROGRAMS += e500/clock.rel
e500_clock_rel_SOURCES = mpc6xx/clock/c_clock.c mpc6xx/clock/c_clock.h
e500_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
e500_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

# e500/mmu
if e500_mmu
noinst_PROGRAMS += e500/mmu.rel
e500_mmu_rel_SOURCES = e500/mmu/mmu.c e500/mmu/e500_mmu.h
e500_mmu_rel_CPPFLAGS = $(AM_CPPFLAGS)
e500_mmu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

EXTRA_DIST += mpc8xx/README
if mpc8xx
# mpc8xx/clock
noinst_PROGRAMS += mpc8xx/clock.rel
mpc8xx_clock_rel_SOURCES = mpc8xx/clock/clock.c
mpc8xx_clock_rel_CPPFLAGS = $(AM_CPPFLAGS)
mpc8xx_clock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# mpc8xx/console-generic
noinst_PROGRAMS += mpc8xx/console-generic.rel
mpc8xx_console_generic_rel_SOURCES = mpc8xx/console-generic/console-generic.c
mpc8xx_console_generic_rel_CPPFLAGS = $(AM_CPPFLAGS)
mpc8xx_console_generic_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# mpc8xx/cpm
noinst_PROGRAMS += mpc8xx/cpm.rel
mpc8xx_cpm_rel_SOURCES = mpc8xx/cpm/cp.c mpc8xx/cpm/dpram.c
mpc8xx_cpm_rel_CPPFLAGS = $(AM_CPPFLAGS)
mpc8xx_cpm_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# mpc8xx/mmu
noinst_PROGRAMS += mpc8xx/mmu.rel
mpc8xx_mmu_rel_SOURCES = mpc8xx/mmu/mmu.c
mpc8xx_mmu_rel_CPPFLAGS = $(AM_CPPFLAGS)
mpc8xx_mmu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

EXTRA_DIST += mpc8260/README
if mpc8260
# mpc8260/console-generic
noinst_PROGRAMS += mpc8260/console-generic.rel
mpc8260_console_generic_rel_SOURCES = mpc8260/console-generic/console-generic.c \
    mpc8260/include/console.h
mpc8260_console_generic_rel_CPPFLAGS = $(AM_CPPFLAGS)
mpc8260_console_generic_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# mpc8260/cpm
noinst_PROGRAMS += mpc8260/cpm.rel
mpc8260_cpm_rel_SOURCES = mpc8260/cpm/cp.c mpc8260/cpm/dpram.c mpc8260/cpm/brg.c \
    mpc8260/include/cpm.h
mpc8260_cpm_rel_CPPFLAGS = $(AM_CPPFLAGS)
mpc8260_cpm_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# mpc8260/mmu
noinst_PROGRAMS += mpc8260/mmu.rel
mpc8260_mmu_rel_SOURCES = mpc8260/mmu/mmu.c \
                          mpc8260/include/mmu.h
mpc8260_mmu_rel_CPPFLAGS = $(AM_CPPFLAGS)
mpc8260_mmu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

##############################################################################
# START: MPC83XX                                                             #
##############################################################################
if mpc83xx

# Includes

# Network
if HAS_NETWORKING
noinst_PROGRAMS += mpc83xx/tsec.rel
mpc83xx_tsec_rel_SOURCES = mpc83xx/network/tsec.c
mpc83xx_tsec_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

# i2c
noinst_PROGRAMS += mpc83xx/i2c.rel
mpc83xx_i2c_rel_SOURCES = mpc83xx/i2c/mpc83xx_i2cdrv.c \
	mpc83xx/i2c/mpc83xx_i2cdrv.h
mpc83xx_i2c_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# SPI
noinst_PROGRAMS += mpc83xx/spi.rel
mpc83xx_spi_rel_SOURCES = mpc83xx/spi/mpc83xx_spidrv.c \
	mpc83xx/spi/mpc83xx_spidrv.h
mpc83xx_spi_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# GTM
noinst_PROGRAMS += mpc83xx/gtm.rel
mpc83xx_gtm_rel_SOURCES = mpc83xx/gtm/gtm.c \
	mpc83xx/include/gtm.h
mpc83xx_gtm_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

endif
##############################################################################
# END: MPC83XX                                                               #
##############################################################################

##############################################################################
# START: MPC55XX                                                             #
##############################################################################
if mpc55xx

# IRQ
noinst_PROGRAMS += mpc55xx/irq.rel
mpc55xx_irq_rel_SOURCES = mpc55xx/irq/irq.c
mpc55xx_irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# FEC
noinst_PROGRAMS += mpc55xx/fec.rel
mpc55xx_fec_rel_SOURCES = mpc55xx/fec/fec.c
mpc55xx_fec_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# eDMA
noinst_PROGRAMS += mpc55xx/edma.rel
mpc55xx_edma_rel_SOURCES = mpc55xx/edma/edma.c
mpc55xx_edma_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# eMIOS
noinst_PROGRAMS += mpc55xx/emios.rel
mpc55xx_emios_rel_SOURCES = mpc55xx/emios/emios.c
mpc55xx_emios_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# SIU
noinst_PROGRAMS += mpc55xx/siu.rel
mpc55xx_siu_rel_SOURCES = mpc55xx/siu/siu.c
mpc55xx_siu_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# DSPI
noinst_PROGRAMS += mpc55xx/dspi.rel
mpc55xx_dspi_rel_SOURCES = mpc55xx/dspi/dspi.c
mpc55xx_dspi_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

# Misc
noinst_PROGRAMS += mpc55xx/misc.rel
mpc55xx_misc_rel_SOURCES = mpc55xx/misc/copy.S \
	mpc55xx/misc/flash_support.c
mpc55xx_misc_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

endif
##############################################################################
# END: MPC55XX                                                               #
##############################################################################

##############################################################################
# START: QorIQ                                                               #
##############################################################################
if qoriq

# Network
if HAS_NETWORKING
noinst_PROGRAMS += tsec.rel
tsec_rel_SOURCES = mpc83xx/network/tsec.c
tsec_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

endif
##############################################################################
# END: QorIQ                                                                 #
##############################################################################

include $(top_srcdir)/../../../automake/local.am
