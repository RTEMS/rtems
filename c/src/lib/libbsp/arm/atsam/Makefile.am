ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

###############################################################################
#                  Data                                                       #
###############################################################################

dist_project_lib_DATA = ../../../../../../bsps/arm/atsam/start/bsp_specs

start.$(OBJEXT): ../../../../../../bsps/arm/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
project_lib_DATA += linkcmds.memory
dist_project_lib_DATA += ../../../../../../bsps/arm/atsam/start/linkcmds.intsram
dist_project_lib_DATA += ../../../../../../bsps/arm/atsam/start/linkcmds.sdram
dist_project_lib_DATA += ../../../../../../bsps/arm/atsam/start/linkcmds.qspiflash

###############################################################################
#                  Header                                                     #
###############################################################################

###############################################################################
#                  LibBSP                                                     #
###############################################################################

project_lib_LIBRARIES = librtemsbsp.a

librtemsbsp_a_SOURCES =
librtemsbsp_a_CPPFLAGS = $(AM_CPPFLAGS)
librtemsbsp_a_LIBADD =

# Shared
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c

# libchip
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/acc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/aes.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/afec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/afe_dma.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/dac_dma.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/efc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/exceptions.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/flashd.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/gmac.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/gmacd.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/hsmci.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/icm.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/isi.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/mcan.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/mediaLB.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/mpu.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/pio.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/pio_capture.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/pio_it.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/pmc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/pwmc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/qspi.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/qspi_dma.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/rstc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/rtc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/rtt.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/sdramc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/spi.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/spi_dma.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/ssc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/supc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/tc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/trng.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/twi.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/twid.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/uart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/uart_dma.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/usart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/usart_dma.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/wdt.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/xdmad.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libchip/source/xdma_hardware_interface.c

# libboard
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libboard/resources_v71/system_samv71.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libboard/source/board_lowlevel.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libboard/source/board_memories.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/contrib/libraries/libboard/source/dbg_console.c

# Startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/start/bspreset-armv7m.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cpucounter/cpucounter-armv7m.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/start/bspstarthooks.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/start/getentropy-trng.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/start/pin-config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/start/pmc-config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/start/power-clock.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/start/power-rtc.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/start/power-wait.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/start/power.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/start/restart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/start/sdram-config.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/irq/irq-armv7m.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/irq/irq-dispatch-armv7m.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/console/debug-console.c

# Clock
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/clock/clock-armv7m.c
librtemsbsp_a_SOURCES +=../../../../../../bsps/arm/atsam/clock/systick-freq.c

# Timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-v7m.c

# Network
if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/net/if_atsam.c
endif

# I2C
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/i2c/atsam_i2c_bus.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/i2c/atsam_i2c_init.c

# SPI
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/spi/atsam_spi_bus.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/spi/atsam_spi_init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/spi/sc16is752.c

# RTC
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/rtc/rtc-config.c

# Helper functions
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/atsam/start/iocopy.c

# Includes
librtemsbsp_a_CPPFLAGS += -I$(RTEMS_SOURCE_ROOT)/bsps/arm/atsam/contrib/libraries/libboard
librtemsbsp_a_CPPFLAGS += -I$(RTEMS_SOURCE_ROOT)/bsps/arm/atsam/contrib/libraries/libboard/include
librtemsbsp_a_CPPFLAGS += -I$(RTEMS_SOURCE_ROOT)/bsps/arm/atsam/contrib/libraries/libchip
librtemsbsp_a_CPPFLAGS += -I$(RTEMS_SOURCE_ROOT)/bsps/arm/atsam/contrib/libraries/libchip/include

# hack to get the BSP to build
librtemsbsp_a_CPPFLAGS += -I$(RTEMS_SOURCE_ROOT)/bsps/arm/atsam/include/libchip
librtemsbsp_a_CPPFLAGS += -I$(RTEMS_SOURCE_ROOT)/bsps/arm/atsam/include/libchip/include

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/atsam/headers.am
