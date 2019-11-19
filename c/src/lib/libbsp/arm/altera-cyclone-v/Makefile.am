##
#
# @file
#
# @brief Makefile of LibBSP for the Altera Cyclone-V platform.
#

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/arm/altera-cyclone-v/start/bsp_specs

start.$(OBJEXT): ../../../../../../bsps/arm/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/arm/altera-cyclone-v/start/linkcmds.altcycv
dist_project_lib_DATA += ../../../../../../bsps/arm/altera-cyclone-v/start/linkcmds

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =

# for the Altera hwlib
CFLAGS += -Wno-missing-prototypes

# hwlib from Altera
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/contrib/hwlib/src/hwmgr/alt_16550_uart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/contrib/hwlib/src/hwmgr/alt_address_space.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/contrib/hwlib/src/hwmgr/alt_clock_manager.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/contrib/hwlib/src/hwmgr/alt_dma.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/contrib/hwlib/src/hwmgr/alt_dma_program.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/contrib/hwlib/src/hwmgr/alt_generalpurpose_io.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/contrib/hwlib/src/hwmgr/alt_i2c.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/contrib/hwlib/src/hwmgr/alt_qspi.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/contrib/hwlib/src/hwmgr/alt_reset_manager.c

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bsp-fdt.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm-cp15-set-exception-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm-cp15-set-ttb-entries.c

# Startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/start/bspclean.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/start/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/start/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/start/bspstarthooks.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/start/mmu-config.c
if HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/arm-a9mpcore-smp.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/start/bspsmp.c
endif

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/irq/irq-gic.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios-init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/console/console-config.c

# Clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/clock/clock-a9mpcore.c

# I2C
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/i2c/i2cdrv.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/i2c/i2cdrv-config.c
# RTC
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/altera-cyclone-v/rtc/rtc.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-l2c-310.c

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/altera-cyclone-v/headers.am
