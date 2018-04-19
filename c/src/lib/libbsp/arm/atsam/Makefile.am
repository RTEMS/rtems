ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

###############################################################################
#                  Data                                                       #
###############################################################################

dist_project_lib_DATA = startup/bsp_specs

start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
project_lib_DATA += linkcmds.memory
dist_project_lib_DATA += startup/linkcmds.intsram
dist_project_lib_DATA += startup/linkcmds.sdram
dist_project_lib_DATA += startup/linkcmds.qspiflash

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
librtemsbsp_a_SOURCES += ../../shared/cpucounterdiff.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/stackalloc.c

# libchip
librtemsbsp_a_SOURCES += libraries/libchip/source/acc.c
librtemsbsp_a_SOURCES += libraries/libchip/source/aes.c
librtemsbsp_a_SOURCES += libraries/libchip/source/afec.c
librtemsbsp_a_SOURCES += libraries/libchip/source/afe_dma.c
librtemsbsp_a_SOURCES += libraries/libchip/source/dac_dma.c
librtemsbsp_a_SOURCES += libraries/libchip/source/efc.c
librtemsbsp_a_SOURCES += libraries/libchip/source/exceptions.c
librtemsbsp_a_SOURCES += libraries/libchip/source/flashd.c
librtemsbsp_a_SOURCES += libraries/libchip/source/gmac.c
librtemsbsp_a_SOURCES += libraries/libchip/source/gmacd.c
librtemsbsp_a_SOURCES += libraries/libchip/source/hsmci.c
librtemsbsp_a_SOURCES += libraries/libchip/source/icm.c
librtemsbsp_a_SOURCES += libraries/libchip/source/isi.c
librtemsbsp_a_SOURCES += libraries/libchip/source/mcan.c
librtemsbsp_a_SOURCES += libraries/libchip/source/mediaLB.c
librtemsbsp_a_SOURCES += libraries/libchip/source/mpu.c
librtemsbsp_a_SOURCES += libraries/libchip/source/pio.c
librtemsbsp_a_SOURCES += libraries/libchip/source/pio_capture.c
librtemsbsp_a_SOURCES += libraries/libchip/source/pio_it.c
librtemsbsp_a_SOURCES += libraries/libchip/source/pmc.c
librtemsbsp_a_SOURCES += libraries/libchip/source/pwmc.c
librtemsbsp_a_SOURCES += libraries/libchip/source/qspi.c
librtemsbsp_a_SOURCES += libraries/libchip/source/qspi_dma.c
librtemsbsp_a_SOURCES += libraries/libchip/source/rstc.c
librtemsbsp_a_SOURCES += libraries/libchip/source/rtc.c
librtemsbsp_a_SOURCES += libraries/libchip/source/rtt.c
librtemsbsp_a_SOURCES += libraries/libchip/source/sdramc.c
librtemsbsp_a_SOURCES += libraries/libchip/source/spi.c
librtemsbsp_a_SOURCES += libraries/libchip/source/spi_dma.c
librtemsbsp_a_SOURCES += libraries/libchip/source/ssc.c
librtemsbsp_a_SOURCES += libraries/libchip/source/supc.c
librtemsbsp_a_SOURCES += libraries/libchip/source/tc.c
librtemsbsp_a_SOURCES += libraries/libchip/source/trng.c
librtemsbsp_a_SOURCES += libraries/libchip/source/twi.c
librtemsbsp_a_SOURCES += libraries/libchip/source/twid.c
librtemsbsp_a_SOURCES += libraries/libchip/source/uart.c
librtemsbsp_a_SOURCES += libraries/libchip/source/uart_dma.c
librtemsbsp_a_SOURCES += libraries/libchip/source/usart.c
librtemsbsp_a_SOURCES += libraries/libchip/source/usart_dma.c
librtemsbsp_a_SOURCES += libraries/libchip/source/wdt.c
librtemsbsp_a_SOURCES += libraries/libchip/source/xdmad.c
librtemsbsp_a_SOURCES += libraries/libchip/source/xdma_hardware_interface.c

# libboard
librtemsbsp_a_SOURCES += libraries/libboard/resources_v71/system_samv71.c
librtemsbsp_a_SOURCES += libraries/libboard/source/board_lowlevel.c
librtemsbsp_a_SOURCES += libraries/libboard/source/board_memories.c
librtemsbsp_a_SOURCES += libraries/libboard/source/dbg_console.c

# Startup
librtemsbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S
librtemsbsp_a_SOURCES += ../shared/armv7m/startup/bspreset.c
librtemsbsp_a_SOURCES += ../shared/armv7m/startup/armv7m-cpucounter.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += startup/bspstarthooks.c
librtemsbsp_a_SOURCES += startup/getentropy-trng.c
librtemsbsp_a_SOURCES += startup/pin-config.c
librtemsbsp_a_SOURCES += startup/pmc-config.c
librtemsbsp_a_SOURCES += startup/power-clock.c
librtemsbsp_a_SOURCES += startup/power-rtc.c
librtemsbsp_a_SOURCES += startup/power.c
librtemsbsp_a_SOURCES += startup/power-rtc.c
librtemsbsp_a_SOURCES += startup/power-clock.c
librtemsbsp_a_SOURCES += startup/restart.c
librtemsbsp_a_SOURCES += startup/sdram-config.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq.c
librtemsbsp_a_SOURCES += ../shared/armv7m/irq/armv7m-irq-dispatch.c

# Console
librtemsbsp_a_SOURCES += ../../shared/console-termios.c
librtemsbsp_a_SOURCES += console/console.c
librtemsbsp_a_SOURCES += console/debug-console.c

# Clock
librtemsbsp_a_SOURCES += ../shared/armv7m/clock/armv7m-clock-config.c
librtemsbsp_a_SOURCES += clock/systick-freq.c

# Timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-v7m.c

# Network
if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/if_atsam.c
endif

# I2C
librtemsbsp_a_SOURCES += i2c/atsam_i2c_bus.c
librtemsbsp_a_SOURCES += i2c/atsam_i2c_init.c

# SPI
librtemsbsp_a_SOURCES += spi/atsam_spi_bus.c
librtemsbsp_a_SOURCES += spi/atsam_spi_init.c
librtemsbsp_a_SOURCES += spi/sc16is752.c

# RTC
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += rtc/rtc-config.c

# Helper functions
librtemsbsp_a_SOURCES += utils/iocopy.c

# Includes
librtemsbsp_a_CPPFLAGS += -I$(srcdir)/../shared/CMSIS/Include
librtemsbsp_a_CPPFLAGS += -I$(srcdir)/libraries/libboard
librtemsbsp_a_CPPFLAGS += -I$(srcdir)/libraries/libboard/include
librtemsbsp_a_CPPFLAGS += -I$(srcdir)/libraries/libchip
librtemsbsp_a_CPPFLAGS += -I$(srcdir)/libraries/libchip/include

# hack to get the BSP to build
librtemsbsp_a_CPPFLAGS += -I$(RTEMS_SOURCE_ROOT)/bsps/arm/atsam/include/libchip
librtemsbsp_a_CPPFLAGS += -I$(RTEMS_SOURCE_ROOT)/bsps/arm/atsam/include/libchip/include

###############################################################################
#                  Special Rules                                              #
###############################################################################


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/atsam/headers.am
